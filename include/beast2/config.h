#ifndef BEAST2_CONFIG_H
#define BEAST2_CONFIG_H

#include <stddef.h>

#define BEAST2_MAX_PATH_LENGTH 4096
#define BEAST2_MAX_SCAN_DIRECTORIES 16

typedef struct beast2_config {
    char workspace_root[BEAST2_MAX_PATH_LENGTH];
    char log_file[BEAST2_MAX_PATH_LENGTH];
    int log_to_stderr;
    int create_missing_directories;
    size_t scheduler_total_vram_mb;
    size_t scheduler_model_cache_vram_mb;
    size_t scheduler_generation_vram_mb;
    size_t scheduler_preview_vram_mb;
    size_t scheduler_buffer_vram_mb;
    size_t scan_directory_count;
    char scan_directories[BEAST2_MAX_SCAN_DIRECTORIES][BEAST2_MAX_PATH_LENGTH];
} beast2_config;

void beast2_config_set_defaults(beast2_config *config);

int beast2_config_load(
    beast2_config *config,
    const char *path,
    char *error_message,
    size_t error_message_size
);

#endif
