#ifndef BEAST2_RUNTIME_H
#define BEAST2_RUNTIME_H

#include <stddef.h>

#include "beast2/tensor_memory.h"

#define BEAST2_MAX_RUNTIME_OUTPUT_SIZE 65536
#define BEAST2_MAX_RUNTIME_MODELS 16

typedef enum beast2_model_category {
    BEAST2_MODEL_CATEGORY_UNKNOWN = 0,
    BEAST2_MODEL_CATEGORY_DIFFUSION = 1,
    BEAST2_MODEL_CATEGORY_VIDEO = 2,
    BEAST2_MODEL_CATEGORY_LLM = 3
} beast2_model_category;

typedef enum beast2_runtime_backend {
    BEAST2_RUNTIME_BACKEND_DEFAULT = 0,
    BEAST2_RUNTIME_BACKEND_ONNX = 1,
    BEAST2_RUNTIME_BACKEND_TENSORRT = 2,
    BEAST2_RUNTIME_BACKEND_LLAMA_CPP = 3,
    BEAST2_RUNTIME_BACKEND_PYTHON = 4
} beast2_runtime_backend;

typedef enum beast2_precision_mode {
    BEAST2_PRECISION_DEFAULT = 0,
    BEAST2_PRECISION_FP32 = 1,
    BEAST2_PRECISION_FP16 = 2,
    BEAST2_PRECISION_INT8 = 3
} beast2_precision_mode;

typedef enum beast2_output_kind {
    BEAST2_OUTPUT_KIND_UNKNOWN = 0,
    BEAST2_OUTPUT_KIND_IMAGE = 1,
    BEAST2_OUTPUT_KIND_VIDEO_MANIFEST = 2,
    BEAST2_OUTPUT_KIND_TEXT = 3
} beast2_output_kind;

typedef struct beast2_model_request {
    const char *engine;
    const char *checkpoint;
    const char *prompt;
    const char *seed;
    const char *steps;
    const char *resolution;
    const char *backend;
    const char *precision;
    const char *runtime;
    const char *model_type;
} beast2_model_request;

typedef struct beast2_model_handle {
    int is_valid;
    int cache_hit;
    size_t slot_index;
    beast2_model_category category;
    beast2_runtime_backend backend;
    beast2_precision_mode precision;
    char engine[128];
    char checkpoint[256];
    char model_id[256];
} beast2_model_handle;

typedef struct beast2_model_result {
    beast2_output_kind output_kind;
    char file_extension[16];
    char mime_type[64];
    char content[BEAST2_MAX_RUNTIME_OUTPUT_SIZE];
    size_t content_length;
    size_t inference_steps;
    size_t load_count;
    size_t infer_count;
    size_t cache_hits;
    size_t tensor_pool_hits;
    size_t tensor_pool_misses;
    size_t tensor_bytes_reused_cpu;
    size_t tensor_bytes_reused_gpu;
    size_t tensor_peak_reserved_cpu;
    size_t tensor_peak_reserved_gpu;
    size_t tensor_peak_in_use_cpu;
    size_t tensor_peak_in_use_gpu;
} beast2_model_result;

typedef struct beast2_loaded_model {
    int occupied;
    char cache_key[512];
    char engine[128];
    char checkpoint[256];
    beast2_model_category category;
    beast2_runtime_backend backend;
    beast2_precision_mode precision;
    size_t reference_count;
    size_t load_count;
    size_t infer_count;
    size_t last_used_tick;
} beast2_loaded_model;

typedef struct beast2_model_runtime_context {
    beast2_loaded_model loaded_models[BEAST2_MAX_RUNTIME_MODELS];
    beast2_tensor_memory_context tensor_memory;
    size_t tick;
    size_t cache_hits;
    size_t cache_misses;
} beast2_model_runtime_context;

void beast2_model_runtime_init(beast2_model_runtime_context *context);
void beast2_model_runtime_shutdown(beast2_model_runtime_context *context);

int beast2_model_load(
    beast2_model_runtime_context *context,
    const beast2_model_request *request,
    beast2_model_handle *handle,
    char *error_message,
    size_t error_message_size
);

int beast2_model_infer(
    beast2_model_runtime_context *context,
    const beast2_model_handle *handle,
    const beast2_model_request *request,
    beast2_model_result *result,
    char *error_message,
    size_t error_message_size
);

void beast2_model_unload(
    beast2_model_runtime_context *context,
    const beast2_model_handle *handle
);

beast2_model_category beast2_model_request_category(const beast2_model_request *request);
beast2_runtime_backend beast2_model_request_backend(const beast2_model_request *request);
beast2_precision_mode beast2_model_request_precision(const beast2_model_request *request);
void beast2_model_runtime_get_tensor_telemetry(
    const beast2_model_runtime_context *context,
    beast2_tensor_memory_telemetry *telemetry
);

const char *beast2_model_category_name(beast2_model_category category);
const char *beast2_runtime_backend_name(beast2_runtime_backend backend);
const char *beast2_precision_mode_name(beast2_precision_mode precision);
const char *beast2_output_kind_name(beast2_output_kind kind);

#endif
