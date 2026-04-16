#include "media_bridge.h"

#include <stdio.h>
#include <string.h>

static beast2_media_library_context s_context;
static int s_ready;
static char s_tags[256][BEAST2_MEDIA_MAX_TAG_NAME];
static size_t s_tag_count;

int media_bridge_init(const char *workspace_root) {
    char err[512];

    s_ready = 0;
    s_tag_count = 0;
    memset(s_tags, 0, sizeof(s_tags));

    if (workspace_root == NULL || workspace_root[0] == '\0') {
        return -1;
    }

    if (beast2_media_library_init(&s_context, workspace_root, err, sizeof err) != 0) {
        return -1;
    }

    s_ready = 1;
    media_bridge_refresh_tag_names();
    return 0;
}

void media_bridge_shutdown(void) {
    if (s_ready) {
        beast2_media_library_shutdown(&s_context);
    }

    s_ready = 0;
    s_tag_count = 0;
    memset(&s_context, 0, sizeof(s_context));
}

int media_bridge_ready(void) {
    return s_ready;
}

beast2_media_library_context *media_bridge_context(void) {
    if (!s_ready) {
        return NULL;
    }

    return &s_context;
}

void media_bridge_refresh_tag_names(void) {
    char err[256];

    s_tag_count = 0;
    if (!s_ready) {
        return;
    }

    if (
        beast2_media_library_list_tags(
            &s_context,
            s_tags,
            sizeof(s_tags) / sizeof(s_tags[0]),
            &s_tag_count,
            err,
            sizeof err
        ) != 0
    ) {
        s_tag_count = 0;
    }
}

size_t media_bridge_tag_count(void) {
    return s_tag_count;
}

const char *media_bridge_tag_name(size_t index) {
    if (index >= s_tag_count) {
        return NULL;
    }

    return s_tags[index];
}

int media_bridge_add_tag_to_absolute_file(
    const char *absolute_file_path,
    const char *tag_name,
    char *error_message,
    size_t error_message_size
) {
    long long media_id = 0;

    if (!s_ready) {
        if (error_message != NULL && error_message_size > 0) {
            snprintf(error_message, error_message_size, "media library not initialized");
        }
        return -1;
    }

    if (
        beast2_media_library_ensure_media_for_absolute_file(
            &s_context,
            absolute_file_path,
            &media_id,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    if (beast2_media_library_add_tag_for_media_id(&s_context, media_id, tag_name, error_message, error_message_size) != 0) {
        return -1;
    }

    media_bridge_refresh_tag_names();
    return 0;
}
