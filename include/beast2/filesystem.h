#ifndef BEAST2_FILESYSTEM_H
#define BEAST2_FILESYSTEM_H

#include <stddef.h>

#include "beast2/config.h"
#include "beast2/logger.h"

typedef struct beast2_scan_result {
    size_t roots_scanned;
    size_t directories_seen;
    size_t files_seen;
    size_t missing_roots;
} beast2_scan_result;

int beast2_fs_is_absolute(const char *path);

int beast2_fs_join_path(
    char *destination,
    size_t destination_size,
    const char *left,
    const char *right
);

int beast2_fs_parent_directory(
    const char *path,
    char *destination,
    size_t destination_size
);

int beast2_fs_mkdirs(
    const char *path,
    char *error_message,
    size_t error_message_size
);

int beast2_fs_ensure_layout(
    const char *workspace_root,
    char *error_message,
    size_t error_message_size
);

int beast2_fs_scan_directories(
    const char *workspace_root,
    const beast2_config *config,
    beast2_logger *logger,
    beast2_scan_result *result,
    char *error_message,
    size_t error_message_size
);

#endif
