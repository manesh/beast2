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

#endif
