#ifndef BEAST2_LATENT_EXPLORER_H
#define BEAST2_LATENT_EXPLORER_H

#include <stddef.h>

#include "beast2/media_library.h"
#include "beast2/scheduler.h"
#include "beast2/tensor_memory.h"

typedef struct beast2_latent_explorer_request {
    long long latent_ids[4];
    double x;
    double y;
    const char *preview_mode;
} beast2_latent_explorer_request;

typedef struct beast2_latent_explorer_result {
    char latent_type[32];
    char model[128];
    char preview_path[BEAST2_MAX_PATH_LENGTH];
    char cache_key[32];
    size_t width;
    size_t height;
    size_t frame_count;
    size_t blended_bytes;
    int cache_hit;
    int scheduler_cache_hit;
} beast2_latent_explorer_result;

int beast2_latent_explorer_preview(
    beast2_media_library_context *media_library,
    beast2_gpu_scheduler_context *scheduler,
    beast2_tensor_memory_context *tensor_memory,
    const beast2_latent_explorer_request *request,
    beast2_latent_explorer_result *result,
    char *error_message,
    size_t error_message_size
);

#endif
