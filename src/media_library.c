#include "beast2/media_library.h"

#include <ctype.h>
#include <errno.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#if !defined(_WIN32)
#include <unistd.h>
#endif

#include "beast2/c_compat.h"
#include "beast2/filesystem.h"

static void beast2_media_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static uint64_t beast2_media_hash_append(uint64_t hash, const char *value) {
    const unsigned char *cursor = (const unsigned char *) value;

    while (*cursor != '\0') {
        hash ^= (uint64_t) *cursor;
        hash *= 1099511628211ULL;
        cursor++;
    }

    return hash;
}

static void beast2_media_current_timestamp(char *destination, size_t destination_size) {
    time_t now = time(NULL);
    struct tm utc_time;
    struct tm *resolved = NULL;

    memset(&utc_time, 0, sizeof(utc_time));
#if defined(_WIN32)
    gmtime_s(&utc_time, &now);
#else
    resolved = gmtime(&now);
    if (resolved != NULL) {
        utc_time = *resolved;
    }
#endif
    strftime(destination, destination_size, "%Y-%m-%dT%H:%M:%SZ", &utc_time);
}

static int beast2_media_exec_sql(
    sqlite3 *db,
    const char *sql,
    char *error_message,
    size_t error_message_size
) {
    char *sqlite_error = NULL;

    if (sqlite3_exec(db, sql, NULL, NULL, &sqlite_error) != SQLITE_OK) {
        snprintf(
            error_message,
            error_message_size,
            "sqlite error: %s",
            sqlite_error != NULL ? sqlite_error : "unknown"
        );
        sqlite3_free(sqlite_error);
        return -1;
    }

    return 0;
}

static int beast2_media_prepare_statement(
    sqlite3 *db,
    sqlite3_stmt **statement,
    const char *sql,
    char *error_message,
    size_t error_message_size
) {
    if (sqlite3_prepare_v2(db, sql, -1, statement, NULL) != SQLITE_OK) {
        snprintf(error_message, error_message_size, "sqlite prepare error: %s", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

static int beast2_media_step_done(
    sqlite3 *db,
    sqlite3_stmt *statement,
    char *error_message,
    size_t error_message_size
) {
    int step_code = sqlite3_step(statement);

    if (step_code != SQLITE_DONE) {
        snprintf(error_message, error_message_size, "sqlite step error: %s", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

static long long beast2_media_upsert_generator(
    sqlite3 *db,
    const beast2_media_record *record,
    char *error_message,
    size_t error_message_size
) {
    static const char *sql =
        "INSERT INTO generators (name, description, creation_date, source_path, engine, checkpoint) "
        "VALUES (?, ?, ?, ?, ?, ?) "
        "ON CONFLICT(name) DO UPDATE SET "
        "source_path=excluded.source_path, "
        "engine=excluded.engine, "
        "checkpoint=excluded.checkpoint;";
    sqlite3_stmt *statement = NULL;
    sqlite3_stmt *query_statement = NULL;
    char timestamp[64];
    long long generator_id = 0;

    beast2_media_current_timestamp(timestamp, sizeof(timestamp));

    if (beast2_media_prepare_statement(db, &statement, sql, error_message, error_message_size) != 0) {
        return 0;
    }

    sqlite3_bind_text(statement, 1, record->generator_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, "", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, timestamp, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, record->generator_source_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, record->engine, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 6, record->checkpoint, -1, SQLITE_TRANSIENT);

    if (beast2_media_step_done(db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return 0;
    }

    sqlite3_finalize(statement);

    if (
        beast2_media_prepare_statement(
            db,
            &query_statement,
            "SELECT generator_id FROM generators WHERE name = ?;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return 0;
    }

    sqlite3_bind_text(query_statement, 1, record->generator_name, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(query_statement) == SQLITE_ROW) {
        generator_id = sqlite3_column_int64(query_statement, 0);
    } else {
        snprintf(error_message, error_message_size, "failed to query generator id");
    }

    sqlite3_finalize(query_statement);
    return generator_id;
}

static int beast2_media_generate_thumbnail(
    beast2_media_library_context *context,
    const beast2_media_record *record,
    beast2_media_record_result *result,
    char *error_message,
    size_t error_message_size
) {
    const char *relative_path = record->output_relative_path;
    const char *slash = strrchr(relative_path, '/');
    const char *shard = NULL;
    char directory[BEAST2_MAX_PATH_LENGTH];
    char base_name[BEAST2_MAX_PATH_LENGTH];
    char stem[BEAST2_MAX_PATH_LENGTH];
    char contents[4096];
    size_t stem_length = 0;
    uint64_t hash = 1469598103934665603ULL;
    FILE *file = NULL;

    if (slash == NULL) {
        beast2_media_set_error(error_message, error_message_size, "invalid media path for thumbnail generation");
        return -1;
    }

    snprintf(directory, sizeof(directory), "%.*s", (int) (slash - relative_path), relative_path);
    snprintf(base_name, sizeof(base_name), "%s", slash + 1);
    shard = strrchr(directory, '/');
    shard = shard != NULL ? shard + 1 : directory;

    stem_length = strcspn(base_name, ".");
    snprintf(stem, sizeof(stem), "%.*s", (int) stem_length, base_name);

    if (strcmp(record->output_kind, "image") == 0) {
        if (
            snprintf(
                result->thumbnail_relative_path,
                sizeof(result->thumbnail_relative_path),
                "thumbs/images/%s/%s.ppm",
                shard,
                stem
            ) >= (int) sizeof(result->thumbnail_relative_path)
        ) {
            beast2_media_set_error(error_message, error_message_size, "thumbnail path exceeds supported length");
            return -1;
        }
    } else {
        if (
            snprintf(
                result->thumbnail_relative_path,
                sizeof(result->thumbnail_relative_path),
                "thumbs/%s/%s/%s.txt",
                strcmp(record->output_kind, "video_manifest") == 0 ? "videos" : "text",
                shard,
                stem
            ) >= (int) sizeof(result->thumbnail_relative_path)
        ) {
            beast2_media_set_error(error_message, error_message_size, "thumbnail path exceeds supported length");
            return -1;
        }
    }

    if (
        beast2_fs_join_path(
            result->thumbnail_path,
            sizeof(result->thumbnail_path),
            context->workspace_root,
            result->thumbnail_relative_path
        ) != 0
    ) {
        beast2_media_set_error(error_message, error_message_size, "failed to build thumbnail path");
        return -1;
    }

    if (strcmp(record->output_kind, "image") == 0) {
        hash = beast2_media_hash_append(hash, record->prompt_hash);
        hash = beast2_media_hash_append(hash, record->checkpoint);
        if (
            snprintf(
                contents,
                sizeof(contents),
                "P3\n"
                "# beast2_thumbnail\n"
                "# source %s\n"
                "2 2\n"
                "255\n"
                "%u %u %u\n"
                "%u %u %u\n"
                "%u %u %u\n"
                "%u %u %u\n",
                record->output_relative_path,
                (unsigned int) ((hash >> 8) & 0xff),
                (unsigned int) ((hash >> 16) & 0xff),
                (unsigned int) ((hash >> 24) & 0xff),
                (unsigned int) ((hash >> 12) & 0xff),
                (unsigned int) ((hash >> 20) & 0xff),
                (unsigned int) ((hash >> 28) & 0xff),
                (unsigned int) ((hash >> 4) & 0xff),
                (unsigned int) ((hash >> 10) & 0xff),
                (unsigned int) ((hash >> 18) & 0xff),
                (unsigned int) ((hash >> 2) & 0xff),
                (unsigned int) ((hash >> 14) & 0xff),
                (unsigned int) ((hash >> 22) & 0xff)
            ) >= (int) sizeof(contents)
        ) {
            beast2_media_set_error(error_message, error_message_size, "thumbnail contents exceeded supported length");
            return -1;
        }
    } else {
        if (
            snprintf(
                contents,
                sizeof(contents),
                "BEAST2_PREVIEW\n"
                "kind: %s\n"
                "source: %s\n"
                "generator: %s\n"
                "checkpoint: %s\n",
                record->output_kind,
                record->output_relative_path,
                record->generator_name,
                record->checkpoint
            ) >= (int) sizeof(contents)
        ) {
            beast2_media_set_error(error_message, error_message_size, "thumbnail contents exceeded supported length");
            return -1;
        }
    }

    {
        char parent_path[BEAST2_MAX_PATH_LENGTH];

        if (beast2_fs_parent_directory(result->thumbnail_path, parent_path, sizeof(parent_path)) != 0) {
            beast2_media_set_error(error_message, error_message_size, "failed to derive thumbnail directory");
            return -1;
        }

        if (beast2_fs_mkdirs(parent_path, error_message, error_message_size) != 0) {
            return -1;
        }
    }

    file = fopen(result->thumbnail_path, "w");

    if (file == NULL) {
        snprintf(error_message, error_message_size, "failed to open thumbnail file: %s", result->thumbnail_path);
        return -1;
    }

    if (fputs(contents, file) == EOF) {
        fclose(file);
        snprintf(error_message, error_message_size, "failed to write thumbnail file: %s", result->thumbnail_path);
        return -1;
    }

    if (fclose(file) != 0) {
        snprintf(error_message, error_message_size, "failed to close thumbnail file: %s", result->thumbnail_path);
        return -1;
    }

    return 0;
}

static long long beast2_media_lookup_media_id(
    sqlite3 *db,
    const char *path,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    long long media_id = 0;

    if (
        beast2_media_prepare_statement(
            db,
            &statement,
            "SELECT media_id FROM media WHERE path = ?;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return 0;
    }

    sqlite3_bind_text(statement, 1, path, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) == SQLITE_ROW) {
        media_id = sqlite3_column_int64(statement, 0);
    } else {
        snprintf(error_message, error_message_size, "failed to lookup media id");
    }

    sqlite3_finalize(statement);
    return media_id;
}

static int beast2_media_upsert_tag(
    sqlite3 *db,
    const char *tag_name,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    int result = 0;

    if (
        beast2_media_prepare_statement(
            db,
            &statement,
            "INSERT INTO tags (tag_name, tag_type) VALUES (?, ?) ON CONFLICT(tag_name) DO NOTHING;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_text(statement, 1, tag_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, "user", -1, SQLITE_TRANSIENT);

    result = beast2_media_step_done(db, statement, error_message, error_message_size);
    sqlite3_finalize(statement);
    return result;
}

static long long beast2_media_lookup_tag_id(
    sqlite3 *db,
    const char *tag_name,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    long long tag_id = 0;

    if (
        beast2_media_prepare_statement(
            db,
            &statement,
            "SELECT tag_id FROM tags WHERE tag_name = ?;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return 0;
    }

    sqlite3_bind_text(statement, 1, tag_name, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) == SQLITE_ROW) {
        tag_id = sqlite3_column_int64(statement, 0);
    } else {
        snprintf(error_message, error_message_size, "failed to lookup tag id");
    }

    sqlite3_finalize(statement);
    return tag_id;
}

static int beast2_media_insert_media_tags(
    sqlite3 *db,
    long long media_id,
    const char *tags_csv,
    char *error_message,
    size_t error_message_size
) {
    char buffer[2048];
    char *token = NULL;
    char *cursor = NULL;

    if (tags_csv == NULL || *tags_csv == '\0') {
        return 0;
    }

    if (strlen(tags_csv) >= sizeof(buffer)) {
        beast2_media_set_error(error_message, error_message_size, "tag list exceeds supported length");
        return -1;
    }

    snprintf(buffer, sizeof(buffer), "%s", tags_csv);
    cursor = buffer;

    while ((token = strtok(cursor, ",")) != NULL) {
        sqlite3_stmt *statement = NULL;
        char *trimmed = token;
        long long tag_id = 0;

        cursor = NULL;

        while (*trimmed == ' ') {
            trimmed++;
        }

        if (*trimmed == '\0') {
            continue;
        }

        if (beast2_media_upsert_tag(db, trimmed, error_message, error_message_size) != 0) {
            return -1;
        }

        tag_id = beast2_media_lookup_tag_id(db, trimmed, error_message, error_message_size);
        if (tag_id == 0) {
            return -1;
        }

        if (
            beast2_media_prepare_statement(
                db,
                &statement,
                "INSERT INTO media_tags (media_id, tag_id, confidence) VALUES (?, ?, ?) "
                "ON CONFLICT(media_id, tag_id) DO UPDATE SET confidence = excluded.confidence;",
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }

        sqlite3_bind_int64(statement, 1, media_id);
        sqlite3_bind_int64(statement, 2, tag_id);
        sqlite3_bind_double(statement, 3, 1.0);

        if (beast2_media_step_done(db, statement, error_message, error_message_size) != 0) {
            sqlite3_finalize(statement);
            return -1;
        }

        sqlite3_finalize(statement);
    }

    return 0;
}

static int beast2_media_insert_generator_history(
    sqlite3 *db,
    long long generator_id,
    long long media_id,
    const char *artifact_relative_path,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    char timestamp[64];

    beast2_media_current_timestamp(timestamp, sizeof(timestamp));

    if (
        beast2_media_prepare_statement(
            db,
            &statement,
            "INSERT INTO generator_history (generator_id, media_id, artifact_path, created_time) VALUES (?, ?, ?, ?);",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_int64(statement, 1, generator_id);
    sqlite3_bind_int64(statement, 2, media_id);
    sqlite3_bind_text(statement, 3, artifact_relative_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, timestamp, -1, SQLITE_TRANSIENT);

    if (beast2_media_step_done(db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return -1;
    }

    sqlite3_finalize(statement);
    return 0;
}

#if defined(_WIN32)
static int beast2_media_canonical_path(const char *input_path, char *out_path, size_t out_size) {
    if (_fullpath(out_path, input_path, (int) out_size) == NULL) {
        return -1;
    }

    return 0;
}
#else
static int beast2_media_canonical_path(const char *input_path, char *out_path, size_t out_size) {
    if (realpath(input_path, out_path) == NULL) {
        return -1;
    }

    return 0;
}
#endif

static void beast2_media_strip_trailing_separators(char *path) {
    size_t n;

    if (path == NULL) {
        return;
    }

    n = strlen(path);
    while (n > 0 && (path[n - 1] == '/' || path[n - 1] == '\\')) {
        path[--n] = '\0';
    }
}

static int beast2_media_relative_under_workspace(
    const char *workspace_canonical,
    const char *file_canonical,
    char *out_relative,
    size_t out_size
) {
    char workspace_copy[BEAST2_MAX_PATH_LENGTH];
    size_t prefix_len;

    snprintf(workspace_copy, sizeof(workspace_copy), "%s", workspace_canonical);
    beast2_media_strip_trailing_separators(workspace_copy);
    prefix_len = strlen(workspace_copy);

#if defined(_WIN32)
    if (_strnicmp(file_canonical, workspace_copy, prefix_len) != 0) {
        return -1;
    }
#else
    if (strncmp(file_canonical, workspace_copy, prefix_len) != 0) {
        return -1;
    }
#endif

    if (file_canonical[prefix_len] != '\0' && file_canonical[prefix_len] != '/' && file_canonical[prefix_len] != '\\') {
        return -1;
    }

    {
        size_t i = prefix_len;
        while (file_canonical[i] == '/' || file_canonical[i] == '\\') {
            i++;
        }

        if (snprintf(out_relative, out_size, "%s", file_canonical + i) >= (int) out_size) {
            return -1;
        }
    }

    for (; *out_relative != '\0'; out_relative++) {
        if (*out_relative == '\\') {
            *out_relative = '/';
        }
    }

    return 0;
}

static long long beast2_media_query_media_id_if_exists(sqlite3 *db, const char *relative_path) {
    sqlite3_stmt *statement = NULL;
    long long media_id = 0;

    if (sqlite3_prepare_v2(db, "SELECT media_id FROM media WHERE path = ?;", -1, &statement, NULL) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_text(statement, 1, relative_path, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) == SQLITE_ROW) {
        media_id = sqlite3_column_int64(statement, 0);
    }

    sqlite3_finalize(statement);
    return media_id;
}

static int beast2_media_ext_equals_icase(const char *name, const char *ext_with_dot) {
    const size_t nl = strlen(name);
    const size_t el = strlen(ext_with_dot);
    size_t i;

    if (nl < el) {
        return 0;
    }

    name += nl - el;
    for (i = 0; i < el; i++) {
        if (
            (unsigned char) tolower((unsigned char) name[i]) !=
            (unsigned char) tolower((unsigned char) ext_with_dot[i])
        ) {
            return 0;
        }
    }

    return 1;
}

static const char *beast2_media_guess_type_from_basename(const char *basename) {
    static const char *const video_exts[] = {
        ".mp4", ".webm", ".mov", ".mkv", ".avi", ".m4v", ".wmv",
    };
    size_t i;

    for (i = 0; i < sizeof(video_exts) / sizeof(video_exts[0]); i++) {
        if (beast2_media_ext_equals_icase(basename, video_exts[i])) {
            return "video_manifest";
        }
    }

    return "image";
}

static int beast2_media_link_tag_to_media(
    sqlite3 *db,
    long long media_id,
    const char *tag_name,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    long long tag_id = 0;

    if (beast2_media_upsert_tag(db, tag_name, error_message, error_message_size) != 0) {
        return -1;
    }

    tag_id = beast2_media_lookup_tag_id(db, tag_name, error_message, error_message_size);
    if (tag_id == 0) {
        return -1;
    }

    if (
        beast2_media_prepare_statement(
            db,
            &statement,
            "INSERT INTO media_tags (media_id, tag_id, confidence) VALUES (?, ?, ?) "
            "ON CONFLICT(media_id, tag_id) DO UPDATE SET confidence = excluded.confidence;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_int64(statement, 1, media_id);
    sqlite3_bind_int64(statement, 2, tag_id);
    sqlite3_bind_double(statement, 3, 1.0);

    if (beast2_media_step_done(db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return -1;
    }

    sqlite3_finalize(statement);
    return 0;
}

int beast2_media_path_absolute_to_workspace_relative(
    const char *workspace_root,
    const char *absolute_file_path,
    char *out_relative,
    size_t out_size
) {
    char workspace_canonical[BEAST2_MAX_PATH_LENGTH];
    char file_canonical[BEAST2_MAX_PATH_LENGTH];

    if (workspace_root == NULL || absolute_file_path == NULL || out_relative == NULL || out_size == 0) {
        return -1;
    }

    if (beast2_media_canonical_path(workspace_root, workspace_canonical, sizeof(workspace_canonical)) != 0) {
        return -1;
    }

    if (beast2_media_canonical_path(absolute_file_path, file_canonical, sizeof(file_canonical)) != 0) {
        return -1;
    }

    if (beast2_media_relative_under_workspace(workspace_canonical, file_canonical, out_relative, out_size) != 0) {
        return -1;
    }

    return 0;
}

int beast2_media_library_ensure_media_for_absolute_file(
    beast2_media_library_context *context,
    const char *absolute_file_path,
    long long *out_media_id,
    char *error_message,
    size_t error_message_size
) {
    char relative_path[BEAST2_MAX_PATH_LENGTH];
    char file_canonical[BEAST2_MAX_PATH_LENGTH];
    struct stat path_stat;
    sqlite3_stmt *statement = NULL;
    char timestamp[64];
    const char *slash = NULL;
    const char *basename_ptr = NULL;
    long long existing_id = 0;

    if (out_media_id != NULL) {
        *out_media_id = 0;
    }

    if (context == NULL || context->db == NULL || absolute_file_path == NULL || out_media_id == NULL) {
        beast2_media_set_error(error_message, error_message_size, "invalid arguments for ensure media");
        return -1;
    }

    if (beast2_media_canonical_path(absolute_file_path, file_canonical, sizeof(file_canonical)) != 0) {
        beast2_media_set_error(error_message, error_message_size, "failed to resolve media file path");
        return -1;
    }

    if (stat(file_canonical, &path_stat) != 0 || !S_ISREG(path_stat.st_mode)) {
        beast2_media_set_error(error_message, error_message_size, "media path is not a regular file");
        return -1;
    }

    if (beast2_media_relative_under_workspace(context->workspace_root, file_canonical, relative_path, sizeof(relative_path)) != 0) {
        beast2_media_set_error(error_message, error_message_size, "media file is outside workspace root");
        return -1;
    }

    existing_id = beast2_media_query_media_id_if_exists(context->db, relative_path);
    if (existing_id != 0) {
        *out_media_id = existing_id;
        return 0;
    }

    slash = strrchr(relative_path, '/');
    basename_ptr = slash != NULL ? slash + 1 : relative_path;

    beast2_media_current_timestamp(timestamp, sizeof(timestamp));

    if (
        beast2_media_prepare_statement(
            context->db,
            &statement,
            "INSERT INTO media (path, thumbnail_path, type, resolution, duration, size, creation_time, generator_id, checkpoint, seed, prompt_hash, prompt, backend, precision) "
            "VALUES (?, '', ?, '', 0, ?, ?, NULL, '', '', '', '', '', '') "
            "ON CONFLICT(path) DO UPDATE SET size = excluded.size, type = excluded.type;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_text(statement, 1, relative_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, beast2_media_guess_type_from_basename(basename_ptr), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(statement, 3, (sqlite3_int64) path_stat.st_size);
    sqlite3_bind_text(statement, 4, timestamp, -1, SQLITE_TRANSIENT);

    if (beast2_media_step_done(context->db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return -1;
    }

    sqlite3_finalize(statement);

    existing_id = beast2_media_query_media_id_if_exists(context->db, relative_path);
    if (existing_id == 0) {
        beast2_media_set_error(error_message, error_message_size, "failed to read media id after insert");
        return -1;
    }

    *out_media_id = existing_id;
    return 0;
}

int beast2_media_library_list_tags(
    beast2_media_library_context *context,
    char (*out_tags)[BEAST2_MEDIA_MAX_TAG_NAME],
    size_t max_tags,
    size_t *out_count,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    size_t n = 0;

    if (out_count != NULL) {
        *out_count = 0;
    }

    if (context == NULL || context->db == NULL || out_tags == NULL || out_count == NULL || max_tags == 0) {
        beast2_media_set_error(error_message, error_message_size, "invalid arguments for list tags");
        return -1;
    }

    if (
        beast2_media_prepare_statement(
            context->db,
            &statement,
            "SELECT tag_name FROM tags ORDER BY tag_name COLLATE NOCASE;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    while (sqlite3_step(statement) == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(statement, 0);

        if (text == NULL) {
            continue;
        }

        if (n >= max_tags) {
            break;
        }

        snprintf(out_tags[n], BEAST2_MEDIA_MAX_TAG_NAME, "%s", (const char *) text);
        n++;
    }

    sqlite3_finalize(statement);
    *out_count = n;
    return 0;
}

int beast2_media_library_add_tag_for_media_id(
    beast2_media_library_context *context,
    long long media_id,
    const char *tag_name,
    char *error_message,
    size_t error_message_size
) {
    if (context == NULL || context->db == NULL || tag_name == NULL || tag_name[0] == '\0') {
        beast2_media_set_error(error_message, error_message_size, "invalid arguments for add tag");
        return -1;
    }

    return beast2_media_link_tag_to_media(context->db, media_id, tag_name, error_message, error_message_size);
}

int beast2_media_library_list_relative_paths_for_tag(
    beast2_media_library_context *context,
    const char *tag_name,
    char (*out_paths)[BEAST2_MAX_PATH_LENGTH],
    size_t max_paths,
    size_t *out_count,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    size_t n = 0;

    if (out_count != NULL) {
        *out_count = 0;
    }

    if (context == NULL || context->db == NULL || tag_name == NULL || tag_name[0] == '\0' || out_paths == NULL || out_count == NULL || max_paths == 0) {
        beast2_media_set_error(error_message, error_message_size, "invalid arguments for list paths for tag");
        return -1;
    }

    if (
        beast2_media_prepare_statement(
            context->db,
            &statement,
            "SELECT m.path FROM media m "
            "INNER JOIN media_tags mt ON m.media_id = mt.media_id "
            "INNER JOIN tags t ON t.tag_id = mt.tag_id "
            "WHERE t.tag_name = ? "
            "ORDER BY m.path COLLATE NOCASE;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_text(statement, 1, tag_name, -1, SQLITE_TRANSIENT);

    while (sqlite3_step(statement) == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(statement, 0);

        if (text == NULL) {
            continue;
        }

        if (n >= max_paths) {
            break;
        }

        snprintf(out_paths[n], BEAST2_MAX_PATH_LENGTH, "%s", (const char *) text);
        n++;
    }

    sqlite3_finalize(statement);
    *out_count = n;
    return 0;
}

int beast2_media_library_init(
    beast2_media_library_context *context,
    const char *workspace_root,
    char *error_message,
    size_t error_message_size
) {
    char db_directory[BEAST2_MAX_PATH_LENGTH];
    static const char *schema_sql =
        "PRAGMA journal_mode=WAL;"
        "CREATE TABLE IF NOT EXISTS generators ("
        "  generator_id INTEGER PRIMARY KEY,"
        "  name TEXT NOT NULL UNIQUE,"
        "  description TEXT,"
        "  creation_date TEXT NOT NULL,"
        "  source_path TEXT,"
        "  engine TEXT,"
        "  checkpoint TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS media ("
        "  media_id INTEGER PRIMARY KEY,"
        "  path TEXT NOT NULL UNIQUE,"
        "  thumbnail_path TEXT,"
        "  type TEXT NOT NULL,"
        "  resolution TEXT,"
        "  duration REAL DEFAULT 0,"
        "  size INTEGER NOT NULL,"
        "  creation_time TEXT NOT NULL,"
        "  generator_id INTEGER,"
        "  checkpoint TEXT,"
        "  seed TEXT,"
        "  prompt_hash TEXT,"
        "  prompt TEXT,"
        "  backend TEXT,"
        "  precision TEXT,"
        "  FOREIGN KEY(generator_id) REFERENCES generators(generator_id)"
        ");"
        "CREATE TABLE IF NOT EXISTS tags ("
        "  tag_id INTEGER PRIMARY KEY,"
        "  tag_name TEXT NOT NULL UNIQUE,"
        "  tag_type TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS media_tags ("
        "  media_id INTEGER NOT NULL,"
        "  tag_id INTEGER NOT NULL,"
        "  confidence REAL NOT NULL DEFAULT 1.0,"
        "  PRIMARY KEY(media_id, tag_id),"
        "  FOREIGN KEY(media_id) REFERENCES media(media_id),"
        "  FOREIGN KEY(tag_id) REFERENCES tags(tag_id)"
        ");"
        "CREATE TABLE IF NOT EXISTS generator_history ("
        "  history_id INTEGER PRIMARY KEY,"
        "  generator_id INTEGER NOT NULL,"
        "  media_id INTEGER NOT NULL,"
        "  artifact_path TEXT NOT NULL,"
        "  created_time TEXT NOT NULL,"
        "  FOREIGN KEY(generator_id) REFERENCES generators(generator_id),"
        "  FOREIGN KEY(media_id) REFERENCES media(media_id)"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_media_generator_id ON media(generator_id);"
        "CREATE INDEX IF NOT EXISTS idx_media_type ON media(type);"
        "CREATE INDEX IF NOT EXISTS idx_media_creation_time ON media(creation_time);"
        "CREATE INDEX IF NOT EXISTS idx_tags_name ON tags(tag_name);"
        "CREATE INDEX IF NOT EXISTS idx_media_tags_tag_id ON media_tags(tag_id);";

    memset(context, 0, sizeof(*context));
    {
        char canonical_workspace[BEAST2_MAX_PATH_LENGTH];

        if (beast2_media_canonical_path(workspace_root, canonical_workspace, sizeof(canonical_workspace)) == 0) {
            snprintf(context->workspace_root, sizeof(context->workspace_root), "%s", canonical_workspace);
        } else {
            snprintf(context->workspace_root, sizeof(context->workspace_root), "%s", workspace_root);
        }
    }

    if (
        beast2_fs_join_path(
            context->db_path,
            sizeof(context->db_path),
            context->workspace_root,
            "db/beast2.sqlite"
        ) != 0
    ) {
        beast2_media_set_error(error_message, error_message_size, "failed to build media database path");
        return -1;
    }

    if (beast2_fs_parent_directory(context->db_path, db_directory, sizeof(db_directory)) != 0) {
        beast2_media_set_error(error_message, error_message_size, "failed to derive media database directory");
        return -1;
    }

    if (beast2_fs_mkdirs(db_directory, error_message, error_message_size) != 0) {
        return -1;
    }

    if (sqlite3_open(context->db_path, &context->db) != SQLITE_OK) {
        snprintf(error_message, error_message_size, "failed to open sqlite database: %s", sqlite3_errmsg(context->db));
        sqlite3_close(context->db);
        context->db = NULL;
        return -1;
    }

    if (beast2_media_exec_sql(context->db, schema_sql, error_message, error_message_size) != 0) {
        sqlite3_close(context->db);
        context->db = NULL;
        return -1;
    }

    return 0;
}

void beast2_media_library_shutdown(beast2_media_library_context *context) {
    if (context->db != NULL) {
        sqlite3_close(context->db);
        context->db = NULL;
    }
}

int beast2_media_library_record_output(
    beast2_media_library_context *context,
    const beast2_media_record *record,
    beast2_media_record_result *result,
    char *error_message,
    size_t error_message_size
) {
    struct stat path_stat;
    sqlite3_stmt *statement = NULL;
    char timestamp[64];
    long long generator_id = 0;

    memset(result, 0, sizeof(*result));

    if (context == NULL || context->db == NULL || record == NULL) {
        beast2_media_set_error(error_message, error_message_size, "media library context is invalid");
        return -1;
    }

    if (stat(record->output_path, &path_stat) != 0) {
        snprintf(error_message, error_message_size, "failed to stat output path: %s", record->output_path);
        return -1;
    }

    if (beast2_media_generate_thumbnail(context, record, result, error_message, error_message_size) != 0) {
        return -1;
    }

    generator_id = beast2_media_upsert_generator(context->db, record, error_message, error_message_size);
    if (generator_id == 0) {
        return -1;
    }

    beast2_media_current_timestamp(timestamp, sizeof(timestamp));

    if (
        beast2_media_prepare_statement(
            context->db,
            &statement,
            "INSERT INTO media (path, thumbnail_path, type, resolution, duration, size, creation_time, generator_id, checkpoint, seed, prompt_hash, prompt, backend, precision) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
            "ON CONFLICT(path) DO UPDATE SET "
            "thumbnail_path=excluded.thumbnail_path, "
            "type=excluded.type, "
            "resolution=excluded.resolution, "
            "duration=excluded.duration, "
            "size=excluded.size, "
            "creation_time=excluded.creation_time, "
            "generator_id=excluded.generator_id, "
            "checkpoint=excluded.checkpoint, "
            "seed=excluded.seed, "
            "prompt_hash=excluded.prompt_hash, "
            "prompt=excluded.prompt, "
            "backend=excluded.backend, "
            "precision=excluded.precision;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_text(statement, 1, record->output_relative_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, result->thumbnail_relative_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, record->output_kind, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, record->resolution, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(statement, 5, 0.0);
    sqlite3_bind_int64(statement, 6, (sqlite3_int64) path_stat.st_size);
    sqlite3_bind_text(statement, 7, timestamp, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(statement, 8, generator_id);
    sqlite3_bind_text(statement, 9, record->checkpoint, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 10, record->seed, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 11, record->prompt_hash, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 12, record->prompt, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 13, record->backend, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 14, record->precision, -1, SQLITE_TRANSIENT);

    if (beast2_media_step_done(context->db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return -1;
    }

    sqlite3_finalize(statement);
    result->generator_id = generator_id;
    result->media_id = beast2_media_lookup_media_id(
        context->db,
        record->output_relative_path,
        error_message,
        error_message_size
    );

    if (result->media_id == 0) {
        return -1;
    }

    if (
        beast2_media_insert_media_tags(
            context->db,
            result->media_id,
            record->tags_csv,
            error_message,
            error_message_size
        ) != 0 ||
        beast2_media_insert_generator_history(
            context->db,
            generator_id,
            result->media_id,
            record->generator_artifact_relative_path,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    return 0;
}
