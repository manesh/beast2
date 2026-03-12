#ifndef BEAST2_LATENT_LIBRARY_H
#define BEAST2_LATENT_LIBRARY_H

#include <stddef.h>

#include "beast2/media_library.h"
#include "beast2/tensor_memory.h"

typedef struct beast2_latent_capture_request {
    long long media_id;
    const char *output_kind;
    const char *output_relative_path;
    const char *engine;
    const char *checkpoint;
    const char *seed;
    const char *steps;
    size_t width;
    size_t height;
    size_t frame_count;
    const char *prompt_hash;
} beast2_latent_capture_request;

typedef struct beast2_latent_capture_result {
    size_t latent_count;
    char first_latent_relative_path[BEAST2_MAX_PATH_LENGTH];
    char first_latent_path[BEAST2_MAX_PATH_LENGTH];
    char first_motion_relative_path[BEAST2_MAX_PATH_LENGTH];
    char first_motion_path[BEAST2_MAX_PATH_LENGTH];
} beast2_latent_capture_result;

int beast2_latent_library_capture(
    beast2_media_library_context *media_library,
    beast2_tensor_memory_context *tensor_memory,
    const beast2_latent_capture_request *request,
    beast2_latent_capture_result *result,
    char *error_message,
    size_t error_message_size
);

#endif
