#include "beast2/tensor_memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void beast2_tensor_memory_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static void beast2_tensor_memory_refresh_telemetry(beast2_tensor_memory_context *context) {
    size_t index = 0;
    size_t bytes_reserved_cpu = 0;
    size_t bytes_reserved_gpu = 0;
    size_t bytes_in_use_cpu = 0;
    size_t bytes_in_use_gpu = 0;
    size_t active_cpu_buffers = 0;
    size_t active_gpu_buffers = 0;

    for (index = 0; index < BEAST2_MAX_TENSOR_POOL_SLOTS; index++) {
        const beast2_tensor_pool_slot *slot = &context->slots[index];

        if (!slot->occupied) {
            continue;
        }

        if (slot->desc.device == BEAST2_TENSOR_DEVICE_GPU) {
            bytes_reserved_gpu += slot->capacity_bytes;
        } else {
            bytes_reserved_cpu += slot->capacity_bytes;
        }

        if (slot->in_use) {
            if (slot->desc.device == BEAST2_TENSOR_DEVICE_GPU) {
                bytes_in_use_gpu += slot->size_bytes;
                active_gpu_buffers++;
            } else {
                bytes_in_use_cpu += slot->size_bytes;
                active_cpu_buffers++;
            }
        }
    }

    context->telemetry.bytes_reserved_cpu = bytes_reserved_cpu;
    context->telemetry.bytes_reserved_gpu = bytes_reserved_gpu;
    context->telemetry.bytes_in_use_cpu = bytes_in_use_cpu;
    context->telemetry.bytes_in_use_gpu = bytes_in_use_gpu;
    context->telemetry.active_cpu_buffers = active_cpu_buffers;
    context->telemetry.active_gpu_buffers = active_gpu_buffers;

    if (bytes_reserved_cpu > context->telemetry.peak_reserved_cpu) {
        context->telemetry.peak_reserved_cpu = bytes_reserved_cpu;
    }

    if (bytes_reserved_gpu > context->telemetry.peak_reserved_gpu) {
        context->telemetry.peak_reserved_gpu = bytes_reserved_gpu;
    }

    if (bytes_in_use_cpu > context->telemetry.peak_in_use_cpu) {
        context->telemetry.peak_in_use_cpu = bytes_in_use_cpu;
    }

    if (bytes_in_use_gpu > context->telemetry.peak_in_use_gpu) {
        context->telemetry.peak_in_use_gpu = bytes_in_use_gpu;
    }
}

static int beast2_tensor_memory_slot_matches(
    const beast2_tensor_pool_slot *slot,
    const beast2_tensor_desc *desc,
    size_t required_bytes
) {
    if (!slot->occupied || slot->in_use) {
        return 0;
    }

    return
        slot->desc.device == desc->device &&
        slot->desc.dtype == desc->dtype &&
        slot->desc.kind == desc->kind &&
        slot->capacity_bytes >= required_bytes;
}

static beast2_tensor_pool_slot *beast2_tensor_memory_choose_reuse_slot(
    beast2_tensor_memory_context *context,
    const beast2_tensor_desc *desc,
    size_t required_bytes
) {
    size_t index = 0;
    beast2_tensor_pool_slot *best = NULL;

    for (index = 0; index < BEAST2_MAX_TENSOR_POOL_SLOTS; index++) {
        beast2_tensor_pool_slot *candidate = &context->slots[index];

        if (!beast2_tensor_memory_slot_matches(candidate, desc, required_bytes)) {
            continue;
        }

        if (best == NULL || candidate->capacity_bytes < best->capacity_bytes) {
            best = candidate;
        }
    }

    return best;
}

static beast2_tensor_pool_slot *beast2_tensor_memory_choose_empty_slot(
    beast2_tensor_memory_context *context
) {
    size_t index = 0;

    for (index = 0; index < BEAST2_MAX_TENSOR_POOL_SLOTS; index++) {
        if (!context->slots[index].occupied) {
            return &context->slots[index];
        }
    }

    return NULL;
}

static beast2_tensor_pool_slot *beast2_tensor_memory_choose_replacement_slot(
    beast2_tensor_memory_context *context
) {
    size_t index = 0;
    beast2_tensor_pool_slot *best = NULL;

    for (index = 0; index < BEAST2_MAX_TENSOR_POOL_SLOTS; index++) {
        beast2_tensor_pool_slot *candidate = &context->slots[index];

        if (!candidate->occupied || candidate->in_use) {
            continue;
        }

        if (best == NULL || candidate->last_used_tick < best->last_used_tick) {
            best = candidate;
        }
    }

    return best;
}

size_t beast2_tensor_dtype_size(beast2_tensor_dtype dtype) {
    switch (dtype) {
        case BEAST2_TENSOR_DTYPE_FP32:
            return 4;
        case BEAST2_TENSOR_DTYPE_FP16:
            return 2;
        case BEAST2_TENSOR_DTYPE_UINT8:
        case BEAST2_TENSOR_DTYPE_INT8:
        default:
            return 1;
    }
}

size_t beast2_tensor_desc_bytes(const beast2_tensor_desc *desc) {
    size_t width = 0;
    size_t height = 0;
    size_t depth = 0;
    size_t channels = 0;

    if (desc == NULL) {
        return 0;
    }

    if (desc->bytes > 0) {
        return desc->bytes;
    }

    width = desc->width > 0 ? desc->width : 1;
    height = desc->height > 0 ? desc->height : 1;
    depth = desc->depth > 0 ? desc->depth : 1;
    channels = desc->channels > 0 ? desc->channels : 1;

    return width * height * depth * channels * beast2_tensor_dtype_size(desc->dtype);
}

void beast2_tensor_memory_init(beast2_tensor_memory_context *context) {
    memset(context, 0, sizeof(*context));
}

void beast2_tensor_memory_shutdown(beast2_tensor_memory_context *context) {
    size_t index = 0;

    for (index = 0; index < BEAST2_MAX_TENSOR_POOL_SLOTS; index++) {
        free(context->slots[index].data);
        context->slots[index].data = NULL;
    }

    memset(context, 0, sizeof(*context));
}

int beast2_tensor_memory_acquire(
    beast2_tensor_memory_context *context,
    const beast2_tensor_desc *desc,
    beast2_tensor_buffer *buffer,
    char *error_message,
    size_t error_message_size
) {
    size_t required_bytes = 0;
    beast2_tensor_pool_slot *slot = NULL;

    if (context == NULL || desc == NULL || buffer == NULL) {
        beast2_tensor_memory_set_error(error_message, error_message_size, "tensor acquire request is invalid");
        return -1;
    }

    required_bytes = beast2_tensor_desc_bytes(desc);

    if (required_bytes == 0) {
        beast2_tensor_memory_set_error(error_message, error_message_size, "tensor acquire request resolved to zero bytes");
        return -1;
    }

    slot = beast2_tensor_memory_choose_reuse_slot(context, desc, required_bytes);

    memset(buffer, 0, sizeof(*buffer));

    if (slot != NULL) {
        slot->in_use = 1;
        slot->size_bytes = required_bytes;
        slot->last_used_tick = ++context->tick;
        slot->reuse_count++;
        slot->desc = *desc;

        buffer->is_valid = 1;
        buffer->reused = 1;
        buffer->slot_index = (size_t) (slot - context->slots);
        buffer->desc = *desc;
        buffer->data = slot->data;
        buffer->capacity_bytes = slot->capacity_bytes;
        buffer->size_bytes = required_bytes;

        context->telemetry.pool_hits++;
        if (desc->device == BEAST2_TENSOR_DEVICE_GPU) {
            context->telemetry.gpu_pool_hits++;
            context->telemetry.bytes_reused_gpu += required_bytes;
        } else {
            context->telemetry.cpu_pool_hits++;
            context->telemetry.bytes_reused_cpu += required_bytes;
        }

        beast2_tensor_memory_refresh_telemetry(context);
        return 0;
    }

    slot = beast2_tensor_memory_choose_empty_slot(context);
    if (slot == NULL) {
        slot = beast2_tensor_memory_choose_replacement_slot(context);
    }

    if (slot == NULL) {
        beast2_tensor_memory_set_error(error_message, error_message_size, "tensor memory pool is exhausted");
        return -1;
    }

    if (slot->occupied) {
        free(slot->data);
        memset(slot, 0, sizeof(*slot));
    }

    slot->data = malloc(required_bytes);
    if (slot->data == NULL) {
        beast2_tensor_memory_set_error(error_message, error_message_size, "out of memory while allocating tensor buffer");
        return -1;
    }

    slot->occupied = 1;
    slot->in_use = 1;
    slot->desc = *desc;
    slot->capacity_bytes = required_bytes;
    slot->size_bytes = required_bytes;
    slot->last_used_tick = ++context->tick;

    buffer->is_valid = 1;
    buffer->reused = 0;
    buffer->slot_index = (size_t) (slot - context->slots);
    buffer->desc = *desc;
    buffer->data = slot->data;
    buffer->capacity_bytes = required_bytes;
    buffer->size_bytes = required_bytes;

    context->telemetry.pool_misses++;
    if (desc->device == BEAST2_TENSOR_DEVICE_GPU) {
        context->telemetry.gpu_pool_misses++;
    } else {
        context->telemetry.cpu_pool_misses++;
    }

    beast2_tensor_memory_refresh_telemetry(context);
    return 0;
}

void beast2_tensor_memory_release(
    beast2_tensor_memory_context *context,
    const beast2_tensor_buffer *buffer
) {
    beast2_tensor_pool_slot *slot = NULL;

    if (context == NULL || buffer == NULL || !buffer->is_valid || buffer->slot_index >= BEAST2_MAX_TENSOR_POOL_SLOTS) {
        return;
    }

    slot = &context->slots[buffer->slot_index];

    if (!slot->occupied) {
        return;
    }

    slot->in_use = 0;
    slot->last_used_tick = ++context->tick;
    beast2_tensor_memory_refresh_telemetry(context);
}

void beast2_tensor_memory_get_telemetry(
    const beast2_tensor_memory_context *context,
    beast2_tensor_memory_telemetry *telemetry
) {
    *telemetry = context->telemetry;
}

const char *beast2_tensor_device_name(beast2_tensor_device device) {
    switch (device) {
        case BEAST2_TENSOR_DEVICE_GPU:
            return "gpu";
        case BEAST2_TENSOR_DEVICE_CPU:
        default:
            return "cpu";
    }
}

const char *beast2_tensor_dtype_name(beast2_tensor_dtype dtype) {
    switch (dtype) {
        case BEAST2_TENSOR_DTYPE_FP32:
            return "fp32";
        case BEAST2_TENSOR_DTYPE_FP16:
            return "fp16";
        case BEAST2_TENSOR_DTYPE_INT8:
            return "int8";
        case BEAST2_TENSOR_DTYPE_UINT8:
        default:
            return "uint8";
    }
}

const char *beast2_tensor_kind_name(beast2_tensor_kind kind) {
    switch (kind) {
        case BEAST2_TENSOR_KIND_LATENT:
            return "latent";
        case BEAST2_TENSOR_KIND_IMAGE:
            return "image";
        case BEAST2_TENSOR_KIND_VIDEO:
            return "video";
        case BEAST2_TENSOR_KIND_EMBEDDING:
            return "embedding";
        case BEAST2_TENSOR_KIND_TEXT:
            return "text";
        case BEAST2_TENSOR_KIND_UNKNOWN:
        default:
            return "unknown";
    }
}
