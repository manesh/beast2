#ifndef BEAST2_TENSOR_MEMORY_H
#define BEAST2_TENSOR_MEMORY_H

#include <stddef.h>

#define BEAST2_MAX_TENSOR_POOL_SLOTS 64

typedef enum beast2_tensor_device {
    BEAST2_TENSOR_DEVICE_CPU = 1,
    BEAST2_TENSOR_DEVICE_GPU = 2
} beast2_tensor_device;

typedef enum beast2_tensor_dtype {
    BEAST2_TENSOR_DTYPE_UINT8 = 1,
    BEAST2_TENSOR_DTYPE_FP32 = 2,
    BEAST2_TENSOR_DTYPE_FP16 = 3,
    BEAST2_TENSOR_DTYPE_INT8 = 4
} beast2_tensor_dtype;

typedef enum beast2_tensor_kind {
    BEAST2_TENSOR_KIND_UNKNOWN = 0,
    BEAST2_TENSOR_KIND_LATENT = 1,
    BEAST2_TENSOR_KIND_IMAGE = 2,
    BEAST2_TENSOR_KIND_VIDEO = 3,
    BEAST2_TENSOR_KIND_EMBEDDING = 4,
    BEAST2_TENSOR_KIND_TEXT = 5
} beast2_tensor_kind;

typedef struct beast2_tensor_desc {
    beast2_tensor_device device;
    beast2_tensor_dtype dtype;
    beast2_tensor_kind kind;
    size_t width;
    size_t height;
    size_t depth;
    size_t channels;
    size_t bytes;
} beast2_tensor_desc;

typedef struct beast2_tensor_buffer {
    int is_valid;
    int reused;
    size_t slot_index;
    beast2_tensor_desc desc;
    void *data;
    size_t capacity_bytes;
    size_t size_bytes;
} beast2_tensor_buffer;

typedef struct beast2_tensor_pool_slot {
    int occupied;
    int in_use;
    beast2_tensor_desc desc;
    void *data;
    size_t capacity_bytes;
    size_t size_bytes;
    size_t reuse_count;
    size_t last_used_tick;
} beast2_tensor_pool_slot;

typedef struct beast2_tensor_memory_telemetry {
    size_t pool_hits;
    size_t pool_misses;
    size_t cpu_pool_hits;
    size_t gpu_pool_hits;
    size_t cpu_pool_misses;
    size_t gpu_pool_misses;
    size_t bytes_reused_cpu;
    size_t bytes_reused_gpu;
    size_t bytes_reserved_cpu;
    size_t bytes_reserved_gpu;
    size_t bytes_in_use_cpu;
    size_t bytes_in_use_gpu;
    size_t peak_reserved_cpu;
    size_t peak_reserved_gpu;
    size_t peak_in_use_cpu;
    size_t peak_in_use_gpu;
    size_t active_cpu_buffers;
    size_t active_gpu_buffers;
} beast2_tensor_memory_telemetry;

typedef struct beast2_tensor_memory_context {
    beast2_tensor_pool_slot slots[BEAST2_MAX_TENSOR_POOL_SLOTS];
    size_t tick;
    beast2_tensor_memory_telemetry telemetry;
} beast2_tensor_memory_context;

void beast2_tensor_memory_init(beast2_tensor_memory_context *context);
void beast2_tensor_memory_shutdown(beast2_tensor_memory_context *context);

int beast2_tensor_memory_acquire(
    beast2_tensor_memory_context *context,
    const beast2_tensor_desc *desc,
    beast2_tensor_buffer *buffer,
    char *error_message,
    size_t error_message_size
);

void beast2_tensor_memory_release(
    beast2_tensor_memory_context *context,
    const beast2_tensor_buffer *buffer
);

void beast2_tensor_memory_get_telemetry(
    const beast2_tensor_memory_context *context,
    beast2_tensor_memory_telemetry *telemetry
);

size_t beast2_tensor_desc_bytes(const beast2_tensor_desc *desc);
size_t beast2_tensor_dtype_size(beast2_tensor_dtype dtype);

const char *beast2_tensor_device_name(beast2_tensor_device device);
const char *beast2_tensor_dtype_name(beast2_tensor_dtype dtype);
const char *beast2_tensor_kind_name(beast2_tensor_kind kind);

#endif
