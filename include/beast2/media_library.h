#ifndef BEAST2_MEDIA_LIBRARY_H
#define BEAST2_MEDIA_LIBRARY_H

#include <stddef.h>

#include "beast2/config.h"

typedef struct sqlite3 sqlite3;

typedef struct beast2_media_library_context {
    sqlite3 *db;
    char db_path[BEAST2_MAX_PATH_LENGTH];
    char workspace_root[BEAST2_MAX_PATH_LENGTH];
} beast2_media_library_context;

typedef struct beast2_media_record {
    const char *generator_name;
    const char *generator_source_path;
    const char *generator_artifact_relative_path;
    const char *generator_artifact_path;
    const char *output_relative_path;
    const char *output_path;
    const char *output_kind;
    const char *engine;
    const char *checkpoint;
    const char *backend;
    const char *precision;
    const char *seed;
    const char *resolution;
    const char *prompt;
    const char *prompt_hash;
    const char *tags_csv;
} beast2_media_record;

typedef struct beast2_media_record_result {
    long long generator_id;
    long long media_id;
    char thumbnail_relative_path[BEAST2_MAX_PATH_LENGTH];
    char thumbnail_path[BEAST2_MAX_PATH_LENGTH];
} beast2_media_record_result;

int beast2_media_library_init(
    beast2_media_library_context *context,
    const char *workspace_root,
    char *error_message,
    size_t error_message_size
);

void beast2_media_library_shutdown(beast2_media_library_context *context);

int beast2_media_library_record_output(
    beast2_media_library_context *context,
    const beast2_media_record *record,
    beast2_media_record_result *result,
    char *error_message,
    size_t error_message_size
);

#define BEAST2_MEDIA_MAX_TAG_NAME 128

/*
 * Map an absolute file path into a workspace-relative path (forward slashes).
 * Both paths are canonicalized first. Returns 0 on success, -1 if the file is not under the workspace.
 */
int beast2_media_path_absolute_to_workspace_relative(
    const char *workspace_root,
    const char *absolute_file_path,
    char *out_relative,
    size_t out_size
);

/*
 * Insert or update a media row for an existing file under the workspace (e.g. tagging from the UI).
 */
int beast2_media_library_ensure_media_for_absolute_file(
    beast2_media_library_context *context,
    const char *absolute_file_path,
    long long *out_media_id,
    char *error_message,
    size_t error_message_size
);

int beast2_media_library_list_tags(
    beast2_media_library_context *context,
    char (*out_tags)[BEAST2_MEDIA_MAX_TAG_NAME],
    size_t max_tags,
    size_t *out_count,
    char *error_message,
    size_t error_message_size
);

int beast2_media_library_add_tag_for_media_id(
    beast2_media_library_context *context,
    long long media_id,
    const char *tag_name,
    char *error_message,
    size_t error_message_size
);

/*
 * Returns workspace-relative paths stored in the database (caller joins with workspace for absolute paths).
 */
int beast2_media_library_list_relative_paths_for_tag(
    beast2_media_library_context *context,
    const char *tag_name,
    char (*out_paths)[BEAST2_MAX_PATH_LENGTH],
    size_t max_paths,
    size_t *out_count,
    char *error_message,
    size_t error_message_size
);

#endif
