/*
 * Resolve workspace from config, pick a browse directory, list + sort media files.
 */
#include "gallery_model.h"

#include "media_bridge.h"

#include "beast2/c_compat.h"
#include "beast2/config.h"
#include "beast2/filesystem.h"
#include "beast2/media_library.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

enum {
    kGalleryModelCap = 4096,
};

static char s_browse_root[BEAST2_MAX_PATH_LENGTH];
static char s_workspace_root[BEAST2_MAX_PATH_LENGTH];
static char s_status_line[512];
static char (*s_full_paths)[BEAST2_MAX_PATH_LENGTH];
static size_t s_file_count;
static unsigned char *s_is_image_flags;

static int beast2_gallery_ext_equals_icase(const char *name, const char *ext_with_dot) {
    const size_t nl = strlen(name);
    const size_t el = strlen(ext_with_dot);
    size_t i;

    if (nl < el) {
        return 0;
    }

    name += nl - el;
    for (i = 0; i < el; i++) {
        if (tolower((unsigned char) name[i]) != tolower((unsigned char) ext_with_dot[i])) {
            return 0;
        }
    }

    return 1;
}

static int beast2_gallery_basename_is_image(const char *basename) {
    static const char *const exts[] = {
        ".png", ".jpg", ".jpeg", ".bmp", ".gif", ".webp", ".tga", ".hdr", ".qoi",
    };
    size_t i;

    for (i = 0; i < sizeof(exts) / sizeof(exts[0]); i++) {
        if (beast2_gallery_ext_equals_icase(basename, exts[i])) {
            return 1;
        }
    }

    return 0;
}

static int beast2_gallery_basename_is_video(const char *basename) {
    static const char *const exts[] = {
        ".mp4", ".webm", ".mov", ".mkv", ".avi", ".m4v", ".wmv",
    };
    size_t i;

    for (i = 0; i < sizeof(exts) / sizeof(exts[0]); i++) {
        if (beast2_gallery_ext_equals_icase(basename, exts[i])) {
            return 1;
        }
    }

    return 0;
}

static int beast2_gallery_basename_is_media(const char *basename) {
    return beast2_gallery_basename_is_image(basename) || beast2_gallery_basename_is_video(basename);
}

static int cmp_basenames(const void *a, const void *b) {
    return strcmp((const char *) a, (const char *) b);
}

static int beast2_gallery_try_browse_subdir(
    const char *workspace,
    const char *subdir,
    char *out,
    size_t out_size
) {
    char candidate[BEAST2_MAX_PATH_LENGTH];
    struct stat st;

    if (beast2_fs_join_path(candidate, sizeof(candidate), workspace, subdir) != 0) {
        return -1;
    }

    if (stat(candidate, &st) != 0 || !S_ISDIR(st.st_mode)) {
        return -1;
    }

    snprintf(out, out_size, "%s", candidate);
    return 0;
}

static void beast2_gallery_pick_browse_root(const beast2_config *cfg, char *out, size_t out_size) {
    static const char *const candidates[] = {
        "outputs/images",
        "outputs",
        "thumbs",
    };
    size_t i;
    struct stat st;

    for (i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
        if (beast2_gallery_try_browse_subdir(cfg->workspace_root, candidates[i], out, out_size) == 0) {
            return;
        }
    }

    if (stat(cfg->workspace_root, &st) == 0 && S_ISDIR(st.st_mode)) {
        snprintf(out, out_size, "%s", cfg->workspace_root);
        return;
    }

    snprintf(out, out_size, ".");
}

static void gallery_model_release_paths(void) {
    free(s_full_paths);
    free(s_is_image_flags);
    s_full_paths = NULL;
    s_is_image_flags = NULL;
    s_file_count = 0;
}

static void beast2_gallery_load_from_tag(const char *tag_name) {
    char (*rels)[BEAST2_MAX_PATH_LENGTH] = NULL;
    size_t n = 0;
    size_t i;
    size_t m = 0;
    size_t k = 0;
    char err[256];
    beast2_media_library_context *ctx = NULL;

    gallery_model_release_paths();

    if (!media_bridge_ready() || tag_name == NULL || tag_name[0] == '\0') {
        snprintf(
            s_status_line,
            sizeof s_status_line,
            "Tag view — select a tag above or add tags via right-click (Folder view)."
        );
        return;
    }

    ctx = media_bridge_context();
    rels = (char (*)[BEAST2_MAX_PATH_LENGTH]) calloc(kGalleryModelCap, sizeof(*rels));
    if (rels == NULL) {
        snprintf(s_status_line, sizeof s_status_line, "out of memory loading tag paths");
        return;
    }

    if (
        beast2_media_library_list_relative_paths_for_tag(
            ctx,
            tag_name,
            rels,
            kGalleryModelCap,
            &n,
            err,
            sizeof err
        ) != 0
    ) {
        snprintf(s_status_line, sizeof s_status_line, "tag query failed: %s", err);
        free(rels);
        return;
    }

    for (i = 0; i < n; i++) {
        const char *slash = strrchr(rels[i], '/');
        const char *base = slash != NULL ? slash + 1 : rels[i];

        if (beast2_gallery_basename_is_media(base)) {
            m++;
        }
    }

    if (m == 0) {
        free(rels);
        snprintf(
            s_status_line,
            sizeof s_status_line,
            "%s — no gallery media for tag \"%s\"",
            s_browse_root,
            tag_name
        );
        return;
    }

    s_full_paths = (char (*)[BEAST2_MAX_PATH_LENGTH]) calloc(m, sizeof(*s_full_paths));
    s_is_image_flags = (unsigned char *) calloc(m, sizeof(*s_is_image_flags));
    if (s_full_paths == NULL || s_is_image_flags == NULL) {
        free(rels);
        free(s_full_paths);
        free(s_is_image_flags);
        s_full_paths = NULL;
        s_is_image_flags = NULL;
        snprintf(s_status_line, sizeof s_status_line, "out of memory for tag file table");
        return;
    }

    for (i = 0; i < n; i++) {
        const char *slash = strrchr(rels[i], '/');
        const char *base = slash != NULL ? slash + 1 : rels[i];

        if (!beast2_gallery_basename_is_media(base)) {
            continue;
        }

        if (beast2_fs_join_path(s_full_paths[k], BEAST2_MAX_PATH_LENGTH, ctx->workspace_root, rels[i]) != 0) {
            continue;
        }

        s_is_image_flags[k] = (unsigned char) (beast2_gallery_basename_is_image(base) ? 1 : 0);
        k++;
    }

    free(rels);
    s_file_count = k;

    snprintf(
        s_status_line,
        sizeof s_status_line,
        "Tag \"%s\" — %zu media file(s)",
        tag_name,
        s_file_count
    );
}

static void beast2_gallery_load_from_disk(void) {
    char (*basenames)[BEAST2_MAX_PATH_LENGTH] = NULL;
    size_t raw_count = 0;
    char err[256];
    size_t i;
    size_t m = 0;

    gallery_model_release_paths();

    basenames = (char (*)[BEAST2_MAX_PATH_LENGTH]) calloc(kGalleryModelCap, sizeof(*basenames));
    if (basenames == NULL) {
        snprintf(s_status_line, sizeof s_status_line, "out of memory listing files");
        return;
    }

    if (
        beast2_fs_list_regular_files(
            s_browse_root,
            basenames,
            kGalleryModelCap,
            &raw_count,
            err,
            sizeof err
        ) != 0
    ) {
        snprintf(s_status_line, sizeof s_status_line, "list failed: %s", err);
        free(basenames);
        return;
    }

    qsort(basenames, raw_count, sizeof(*basenames), cmp_basenames);

    for (i = 0; i < raw_count; i++) {
        if (beast2_gallery_basename_is_media(basenames[i])) {
            m++;
        }
    }

    if (m == 0) {
        free(basenames);
        s_full_paths = NULL;
        s_is_image_flags = NULL;
        s_file_count = 0;
        snprintf(
            s_status_line,
            sizeof s_status_line,
            "%s — 0 media file(s)",
            s_browse_root
        );
        return;
    }

    s_full_paths = (char (*)[BEAST2_MAX_PATH_LENGTH]) calloc(m, sizeof(*s_full_paths));
    s_is_image_flags = (unsigned char *) calloc(m, sizeof(*s_is_image_flags));
    if (s_full_paths == NULL || s_is_image_flags == NULL) {
        free(basenames);
        free(s_full_paths);
        free(s_is_image_flags);
        s_full_paths = NULL;
        s_is_image_flags = NULL;
        snprintf(s_status_line, sizeof s_status_line, "out of memory for file table");
        return;
    }

    m = 0;
    for (i = 0; i < raw_count; i++) {
        if (!beast2_gallery_basename_is_media(basenames[i])) {
            continue;
        }

        if (beast2_fs_join_path(s_full_paths[m], BEAST2_MAX_PATH_LENGTH, s_browse_root, basenames[i]) != 0) {
            continue;
        }

        s_is_image_flags[m] = (unsigned char) (beast2_gallery_basename_is_image(basenames[i]) ? 1 : 0);
        m++;
    }

    s_file_count = m;
    free(basenames);

    snprintf(
        s_status_line,
        sizeof s_status_line,
        "%s — %zu media file(s) (images load via raylib; videos: placeholder)",
        s_browse_root,
        s_file_count
    );
}

void gallery_model_reload(int is_folder_view, const char *active_tag_name) {
    if (is_folder_view) {
        beast2_gallery_load_from_disk();
    } else {
        beast2_gallery_load_from_tag(active_tag_name);
    }
}

void gallery_model_init(const char *config_path) {
    beast2_config cfg;
    char err[512];

    snprintf(s_workspace_root, sizeof s_workspace_root, ".");
    snprintf(s_browse_root, sizeof s_browse_root, ".");
    snprintf(
        s_status_line,
        sizeof s_status_line,
        "loading %s…",
        config_path != NULL ? config_path : "config"
    );

    beast2_config_set_defaults(&cfg);
    if (config_path != NULL && beast2_config_load(&cfg, config_path, err, sizeof err) == 0) {
        snprintf(s_workspace_root, sizeof s_workspace_root, "%s", cfg.workspace_root);
        beast2_gallery_pick_browse_root(&cfg, s_browse_root, sizeof s_browse_root);
        if (media_bridge_init(cfg.workspace_root) != 0) {
            snprintf(
                s_status_line,
                sizeof s_status_line,
                "media library init failed — tag features disabled (%s)",
                cfg.workspace_root
            );
        }
    } else {
        if (config_path != NULL) {
            snprintf(
                s_status_line,
                sizeof s_status_line,
                "config: %s (%s) — browsing .",
                config_path,
                err
            );
        } else {
            snprintf(s_status_line, sizeof s_status_line, "no config path — browsing .");
        }
    }

    beast2_gallery_load_from_disk();
}

void gallery_model_shutdown(void) {
    gallery_model_release_paths();
    media_bridge_shutdown();
}

size_t gallery_model_file_count(void) {
    return s_file_count;
}

const char *gallery_model_full_path(size_t index) {
    if (index >= s_file_count || s_full_paths == NULL) {
        return NULL;
    }

    return s_full_paths[index];
}

const char *gallery_model_basename(size_t index) {
    const char *full = gallery_model_full_path(index);
    const char *slash = NULL;

    if (full == NULL) {
        return NULL;
    }

    slash = strrchr(full, '/');
    if (slash == NULL) {
        slash = strrchr(full, '\\');
    }

    return slash != NULL ? slash + 1 : full;
}

const char *gallery_model_browse_root(void) {
    return s_browse_root;
}

const char *gallery_model_workspace_root(void) {
    return s_workspace_root;
}

const char *gallery_model_status_line(void) {
    return s_status_line;
}

int gallery_model_file_is_image(size_t index) {
    if (index >= s_file_count || s_is_image_flags == NULL) {
        return 0;
    }

    return s_is_image_flags[index] != 0;
}
