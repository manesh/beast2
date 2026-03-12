#include "beast2/runtime.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void beast2_runtime_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static int beast2_runtime_string_equals_ignore_case(const char *left, const char *right) {
    size_t index = 0;

    if (left == NULL || right == NULL) {
        return 0;
    }

    while (left[index] != '\0' && right[index] != '\0') {
        if (tolower((unsigned char) left[index]) != tolower((unsigned char) right[index])) {
            return 0;
        }

        index++;
    }

    return left[index] == '\0' && right[index] == '\0';
}

static int beast2_runtime_contains_ignore_case(const char *haystack, const char *needle) {
    size_t start = 0;
    size_t needle_length = 0;

    if (haystack == NULL || needle == NULL) {
        return 0;
    }

    needle_length = strlen(needle);

    if (needle_length == 0) {
        return 1;
    }

    for (start = 0; haystack[start] != '\0'; start++) {
        size_t index = 0;

        while (
            haystack[start + index] != '\0' &&
            needle[index] != '\0' &&
            tolower((unsigned char) haystack[start + index]) == tolower((unsigned char) needle[index])
        ) {
            index++;
        }

        if (index == needle_length) {
            return 1;
        }
    }

    return 0;
}

static uint64_t beast2_runtime_hash_append(uint64_t hash, const char *value) {
    const unsigned char *cursor = (const unsigned char *) value;

    while (*cursor != '\0') {
        hash ^= (uint64_t) *cursor;
        hash *= 1099511628211ULL;
        cursor++;
    }

    return hash;
}

static uint64_t beast2_runtime_hash_values(const char **values) {
    uint64_t hash = 1469598103934665603ULL;
    size_t index = 0;

    for (index = 0; values[index] != NULL; index++) {
        hash = beast2_runtime_hash_append(hash, values[index]);
        hash = beast2_runtime_hash_append(hash, "\n");
    }

    return hash;
}

static void beast2_runtime_hash_hex(char *destination, size_t destination_size, const char **values) {
    uint64_t hash = beast2_runtime_hash_values(values);

    snprintf(destination, destination_size, "%016llx", (unsigned long long) hash);
}

static beast2_model_category beast2_runtime_parse_model_category(
    const beast2_model_request *request
) {
    if (request->model_type != NULL && *request->model_type != '\0') {
        if (beast2_runtime_string_equals_ignore_case(request->model_type, "diffusion")) {
            return BEAST2_MODEL_CATEGORY_DIFFUSION;
        }

        if (beast2_runtime_string_equals_ignore_case(request->model_type, "video")) {
            return BEAST2_MODEL_CATEGORY_VIDEO;
        }

        if (beast2_runtime_string_equals_ignore_case(request->model_type, "llm")) {
            return BEAST2_MODEL_CATEGORY_LLM;
        }
    }

    if (request->engine != NULL) {
        if (
            beast2_runtime_contains_ignore_case(request->engine, "sdxl") ||
            beast2_runtime_contains_ignore_case(request->engine, "stable_diffusion") ||
            beast2_runtime_string_equals_ignore_case(request->engine, "sd")
        ) {
            return BEAST2_MODEL_CATEGORY_DIFFUSION;
        }

        if (
            beast2_runtime_contains_ignore_case(request->engine, "wan") ||
            beast2_runtime_contains_ignore_case(request->engine, "ltx") ||
            beast2_runtime_contains_ignore_case(request->engine, "video")
        ) {
            return BEAST2_MODEL_CATEGORY_VIDEO;
        }

        if (
            beast2_runtime_contains_ignore_case(request->engine, "llama") ||
            beast2_runtime_contains_ignore_case(request->engine, "qwen") ||
            beast2_runtime_contains_ignore_case(request->engine, "mistral")
        ) {
            return BEAST2_MODEL_CATEGORY_LLM;
        }
    }

    return BEAST2_MODEL_CATEGORY_DIFFUSION;
}

static beast2_runtime_backend beast2_runtime_parse_backend(
    const beast2_model_request *request,
    beast2_model_category category
) {
    const char *backend = request->backend;

    if (request->runtime != NULL && *request->runtime != '\0') {
        if (
            beast2_runtime_string_equals_ignore_case(request->runtime, "b2_python") ||
            beast2_runtime_string_equals_ignore_case(request->runtime, "python")
        ) {
            return BEAST2_RUNTIME_BACKEND_PYTHON;
        }
    }

    if (backend != NULL && *backend != '\0') {
        if (beast2_runtime_string_equals_ignore_case(backend, "onnx")) {
            return BEAST2_RUNTIME_BACKEND_ONNX;
        }

        if (beast2_runtime_string_equals_ignore_case(backend, "tensorrt")) {
            return BEAST2_RUNTIME_BACKEND_TENSORRT;
        }

        if (
            beast2_runtime_string_equals_ignore_case(backend, "llama.cpp") ||
            beast2_runtime_string_equals_ignore_case(backend, "llama_cpp")
        ) {
            return BEAST2_RUNTIME_BACKEND_LLAMA_CPP;
        }

        if (beast2_runtime_string_equals_ignore_case(backend, "python")) {
            return BEAST2_RUNTIME_BACKEND_PYTHON;
        }
    }

    switch (category) {
        case BEAST2_MODEL_CATEGORY_LLM:
            return BEAST2_RUNTIME_BACKEND_LLAMA_CPP;
        case BEAST2_MODEL_CATEGORY_VIDEO:
        case BEAST2_MODEL_CATEGORY_DIFFUSION:
        case BEAST2_MODEL_CATEGORY_UNKNOWN:
        default:
            return BEAST2_RUNTIME_BACKEND_ONNX;
    }
}

static beast2_precision_mode beast2_runtime_parse_precision(const beast2_model_request *request) {
    if (request->precision == NULL || *request->precision == '\0') {
        return BEAST2_PRECISION_FP32;
    }

    if (beast2_runtime_string_equals_ignore_case(request->precision, "fp32")) {
        return BEAST2_PRECISION_FP32;
    }

    if (beast2_runtime_string_equals_ignore_case(request->precision, "fp16")) {
        return BEAST2_PRECISION_FP16;
    }

    if (beast2_runtime_string_equals_ignore_case(request->precision, "int8")) {
        return BEAST2_PRECISION_INT8;
    }

    return BEAST2_PRECISION_FP32;
}

static size_t beast2_runtime_parse_size_value(const char *value, size_t fallback) {
    unsigned long parsed = 0;
    char *end = NULL;

    if (value == NULL || *value == '\0') {
        return fallback;
    }

    parsed = strtoul(value, &end, 10);

    if (end == value || *end != '\0' || parsed == 0) {
        return fallback;
    }

    return (size_t) parsed;
}

static void beast2_runtime_parse_resolution(
    const char *resolution,
    size_t *width,
    size_t *height
) {
    const char *separator = NULL;
    char width_buffer[32];
    char height_buffer[32];
    size_t width_length = 0;
    size_t height_length = 0;

    *width = 32;
    *height = 32;

    if (resolution == NULL || *resolution == '\0') {
        return;
    }

    separator = strchr(resolution, 'x');

    if (separator == NULL) {
        separator = strchr(resolution, 'X');
    }

    if (separator == NULL) {
        return;
    }

    width_length = (size_t) (separator - resolution);
    height_length = strlen(separator + 1);

    if (
        width_length == 0 ||
        height_length == 0 ||
        width_length >= sizeof(width_buffer) ||
        height_length >= sizeof(height_buffer)
    ) {
        return;
    }

    memcpy(width_buffer, resolution, width_length);
    width_buffer[width_length] = '\0';
    memcpy(height_buffer, separator + 1, height_length);
    height_buffer[height_length] = '\0';

    *width = beast2_runtime_parse_size_value(width_buffer, 32);
    *height = beast2_runtime_parse_size_value(height_buffer, 32);
}

static beast2_loaded_model *beast2_runtime_choose_slot(beast2_model_runtime_context *context) {
    size_t index = 0;
    beast2_loaded_model *best = &context->loaded_models[0];

    for (index = 0; index < BEAST2_MAX_RUNTIME_MODELS; index++) {
        if (!context->loaded_models[index].occupied) {
            return &context->loaded_models[index];
        }
    }

    for (index = 0; index < BEAST2_MAX_RUNTIME_MODELS; index++) {
        beast2_loaded_model *candidate = &context->loaded_models[index];

        if (candidate->reference_count == 0 && candidate->last_used_tick < best->last_used_tick) {
            best = candidate;
        }
    }

    return best;
}

static int beast2_runtime_format_image(
    const beast2_model_handle *handle,
    const beast2_model_request *request,
    beast2_model_result *result,
    char *error_message,
    size_t error_message_size
) {
    char steps_buffer[64];
    char resolution_buffer[64];
    const char *hash_values[10];
    uint64_t pixel_state = 0;
    size_t width = 32;
    size_t height = 32;
    size_t requested_width = 32;
    size_t requested_height = 32;
    size_t x = 0;
    size_t y = 0;
    size_t length = 0;
    size_t steps = beast2_runtime_parse_size_value(request->steps, 1);

    beast2_runtime_parse_resolution(request->resolution, &requested_width, &requested_height);
    width = requested_width;
    height = requested_height;

    if (width * height > 4096) {
        width = 64;
        height = 64;
    }

    snprintf(steps_buffer, sizeof(steps_buffer), "%zu", steps);
    snprintf(resolution_buffer, sizeof(resolution_buffer), "%zux%zu", requested_width, requested_height);

    hash_values[0] = handle->engine;
    hash_values[1] = handle->checkpoint;
    hash_values[2] = request->prompt != NULL ? request->prompt : "";
    hash_values[3] = request->seed != NULL ? request->seed : "0";
    hash_values[4] = steps_buffer;
    hash_values[5] = resolution_buffer;
    hash_values[6] = beast2_runtime_backend_name(handle->backend);
    hash_values[7] = beast2_precision_mode_name(handle->precision);
    hash_values[8] = beast2_model_category_name(handle->category);
    hash_values[9] = NULL;

    pixel_state = beast2_runtime_hash_values(hash_values);

    length += (size_t) snprintf(
        result->content + length,
        sizeof(result->content) - length,
        "P3\n"
        "# beast2_phase3_image\n"
        "# engine %s\n"
        "# checkpoint %s\n"
        "# backend %s\n"
        "# precision %s\n"
        "# seed %s\n"
        "# requested_resolution %zux%zu\n"
        "# prompt %s\n"
        "%zu %zu\n"
        "255\n",
        handle->engine,
        handle->checkpoint,
        beast2_runtime_backend_name(handle->backend),
        beast2_precision_mode_name(handle->precision),
        request->seed != NULL ? request->seed : "0",
        requested_width,
        requested_height,
        request->prompt != NULL ? request->prompt : "",
        width,
        height
    );

    if (length >= sizeof(result->content)) {
        beast2_runtime_set_error(error_message, error_message_size, "image output exceeded supported size");
        return -1;
    }

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            unsigned int red = 0;
            unsigned int green = 0;
            unsigned int blue = 0;
            int written = 0;

            pixel_state = pixel_state * 6364136223846793005ULL + 1ULL;
            red = (unsigned int) ((pixel_state >> 16) & 0xff);
            green = (unsigned int) ((pixel_state >> 24) & 0xff);
            blue = (unsigned int) ((pixel_state >> 32) & 0xff);

            written = snprintf(
                result->content + length,
                sizeof(result->content) - length,
                "%u %u %u\n",
                red,
                green,
                blue
            );

            if (written < 0 || length + (size_t) written >= sizeof(result->content)) {
                beast2_runtime_set_error(error_message, error_message_size, "image output exceeded supported size");
                return -1;
            }

            length += (size_t) written;
        }
    }

    result->output_kind = BEAST2_OUTPUT_KIND_IMAGE;
    snprintf(result->file_extension, sizeof(result->file_extension), "%s", "ppm");
    snprintf(result->mime_type, sizeof(result->mime_type), "%s", "image/x-portable-pixmap");
    result->content_length = length;
    result->inference_steps = steps;
    return 0;
}

static int beast2_runtime_format_video_manifest(
    const beast2_model_handle *handle,
    const beast2_model_request *request,
    beast2_model_result *result,
    char *error_message,
    size_t error_message_size
) {
    size_t steps = beast2_runtime_parse_size_value(request->steps, 1);
    size_t frame_index = 0;
    size_t frame_count = 4;
    size_t length = 0;
    char hash_buffer[17];

    length += (size_t) snprintf(
        result->content + length,
        sizeof(result->content) - length,
        "BEAST2_PHASE3_VIDEO\n"
        "engine: %s\n"
        "checkpoint: %s\n"
        "backend: %s\n"
        "precision: %s\n"
        "seed: %s\n"
        "steps: %zu\n"
        "resolution: %s\n"
        "prompt: %s\n",
        handle->engine,
        handle->checkpoint,
        beast2_runtime_backend_name(handle->backend),
        beast2_precision_mode_name(handle->precision),
        request->seed != NULL ? request->seed : "0",
        steps,
        request->resolution != NULL ? request->resolution : "unknown",
        request->prompt != NULL ? request->prompt : ""
    );

    if (length >= sizeof(result->content)) {
        beast2_runtime_set_error(error_message, error_message_size, "video manifest exceeded supported size");
        return -1;
    }

    for (frame_index = 0; frame_index < frame_count; frame_index++) {
        char frame_buffer[64];
        const char *hash_values[4];

        snprintf(frame_buffer, sizeof(frame_buffer), "%zu", frame_index + 1);
        hash_values[0] = handle->checkpoint;
        hash_values[1] = request->prompt != NULL ? request->prompt : "";
        hash_values[2] = frame_buffer;
        hash_values[3] = NULL;
        beast2_runtime_hash_hex(hash_buffer, sizeof(hash_buffer), hash_values);

        length += (size_t) snprintf(
            result->content + length,
            sizeof(result->content) - length,
            "frame_%zu: %s\n",
            frame_index + 1,
            hash_buffer
        );

        if (length >= sizeof(result->content)) {
            beast2_runtime_set_error(error_message, error_message_size, "video manifest exceeded supported size");
            return -1;
        }
    }

    result->output_kind = BEAST2_OUTPUT_KIND_VIDEO_MANIFEST;
    snprintf(result->file_extension, sizeof(result->file_extension), "%s", "txt");
    snprintf(result->mime_type, sizeof(result->mime_type), "%s", "text/plain");
    result->content_length = length;
    result->inference_steps = steps;
    return 0;
}

static int beast2_runtime_format_text_output(
    const beast2_model_handle *handle,
    const beast2_model_request *request,
    beast2_model_result *result,
    char *error_message,
    size_t error_message_size
) {
    char hash_buffer[17];
    const char *hash_values[5];
    int written = 0;

    hash_values[0] = handle->engine;
    hash_values[1] = handle->checkpoint;
    hash_values[2] = request->prompt != NULL ? request->prompt : "";
    hash_values[3] = request->seed != NULL ? request->seed : "0";
    hash_values[4] = NULL;
    beast2_runtime_hash_hex(hash_buffer, sizeof(hash_buffer), hash_values);

    written = snprintf(
        result->content,
        sizeof(result->content),
        "BEAST2_PHASE3_LLM_OUTPUT\n"
        "engine: %s\n"
        "checkpoint: %s\n"
        "backend: %s\n"
        "precision: %s\n"
        "seed: %s\n"
        "prompt_hash: %s\n"
        "response: %s :: deterministic local language response %s\n",
        handle->engine,
        handle->checkpoint,
        beast2_runtime_backend_name(handle->backend),
        beast2_precision_mode_name(handle->precision),
        request->seed != NULL ? request->seed : "0",
        hash_buffer,
        request->prompt != NULL ? request->prompt : "",
        hash_buffer
    );

    if (written < 0 || (size_t) written >= sizeof(result->content)) {
        beast2_runtime_set_error(error_message, error_message_size, "text output exceeded supported size");
        return -1;
    }

    result->output_kind = BEAST2_OUTPUT_KIND_TEXT;
    snprintf(result->file_extension, sizeof(result->file_extension), "%s", "txt");
    snprintf(result->mime_type, sizeof(result->mime_type), "%s", "text/plain");
    result->content_length = (size_t) written;
    result->inference_steps = beast2_runtime_parse_size_value(request->steps, 1);
    return 0;
}

void beast2_model_runtime_init(beast2_model_runtime_context *context) {
    memset(context, 0, sizeof(*context));
}

void beast2_model_runtime_shutdown(beast2_model_runtime_context *context) {
    memset(context, 0, sizeof(*context));
}

int beast2_model_load(
    beast2_model_runtime_context *context,
    const beast2_model_request *request,
    beast2_model_handle *handle,
    char *error_message,
    size_t error_message_size
) {
    beast2_model_category category = BEAST2_MODEL_CATEGORY_UNKNOWN;
    beast2_runtime_backend backend = BEAST2_RUNTIME_BACKEND_DEFAULT;
    beast2_precision_mode precision = BEAST2_PRECISION_DEFAULT;
    char cache_key[512];
    size_t index = 0;
    beast2_loaded_model *slot = NULL;

    if (request == NULL || handle == NULL) {
        beast2_runtime_set_error(error_message, error_message_size, "runtime load request is invalid");
        return -1;
    }

    memset(handle, 0, sizeof(*handle));
    category = beast2_runtime_parse_model_category(request);
    backend = beast2_runtime_parse_backend(request, category);
    precision = beast2_runtime_parse_precision(request);

    if (request->engine == NULL || *request->engine == '\0') {
        beast2_runtime_set_error(error_message, error_message_size, "workflow must specify b2_engine for runtime execution");
        return -1;
    }

    if (
        snprintf(
            cache_key,
            sizeof(cache_key),
            "%s|%s|%s|%s",
            request->engine,
            request->checkpoint != NULL && *request->checkpoint != '\0' ? request->checkpoint : "(default)",
            beast2_runtime_backend_name(backend),
            beast2_precision_mode_name(precision)
        ) >= (int) sizeof(cache_key)
    ) {
        beast2_runtime_set_error(error_message, error_message_size, "runtime cache key exceeds supported length");
        return -1;
    }

    context->tick++;

    for (index = 0; index < BEAST2_MAX_RUNTIME_MODELS; index++) {
        if (
            context->loaded_models[index].occupied &&
            strcmp(context->loaded_models[index].cache_key, cache_key) == 0
        ) {
            slot = &context->loaded_models[index];
            slot->reference_count++;
            slot->last_used_tick = context->tick;
            context->cache_hits++;
            handle->cache_hit = 1;
            break;
        }
    }

    if (slot == NULL) {
        slot = beast2_runtime_choose_slot(context);
        memset(slot, 0, sizeof(*slot));
        slot->occupied = 1;
        slot->reference_count = 1;
        slot->load_count = 1;
        slot->last_used_tick = context->tick;
        slot->category = category;
        slot->backend = backend;
        slot->precision = precision;
        snprintf(slot->cache_key, sizeof(slot->cache_key), "%s", cache_key);
        snprintf(slot->engine, sizeof(slot->engine), "%s", request->engine);
        snprintf(
            slot->checkpoint,
            sizeof(slot->checkpoint),
            "%s",
            request->checkpoint != NULL && *request->checkpoint != '\0' ? request->checkpoint : "(default)"
        );
        context->cache_misses++;
    }

    handle->is_valid = 1;
    handle->slot_index = (size_t) (slot - context->loaded_models);
    handle->category = slot->category;
    handle->backend = slot->backend;
    handle->precision = slot->precision;
    snprintf(handle->engine, sizeof(handle->engine), "%s", slot->engine);
    snprintf(handle->checkpoint, sizeof(handle->checkpoint), "%s", slot->checkpoint);
    snprintf(
        handle->model_id,
        sizeof(handle->model_id),
        "%s/%s/%s",
        slot->engine,
        beast2_runtime_backend_name(slot->backend),
        slot->checkpoint
    );

    return 0;
}

int beast2_model_infer(
    beast2_model_runtime_context *context,
    const beast2_model_handle *handle,
    const beast2_model_request *request,
    beast2_model_result *result,
    char *error_message,
    size_t error_message_size
) {
    beast2_loaded_model *slot = NULL;

    if (context == NULL || handle == NULL || request == NULL || result == NULL || !handle->is_valid) {
        beast2_runtime_set_error(error_message, error_message_size, "runtime infer request is invalid");
        return -1;
    }

    if (handle->slot_index >= BEAST2_MAX_RUNTIME_MODELS) {
        beast2_runtime_set_error(error_message, error_message_size, "runtime handle slot is out of range");
        return -1;
    }

    slot = &context->loaded_models[handle->slot_index];

    if (!slot->occupied) {
        beast2_runtime_set_error(error_message, error_message_size, "runtime handle references an unloaded model");
        return -1;
    }

    memset(result, 0, sizeof(*result));
    slot->infer_count++;
    slot->last_used_tick = ++context->tick;
    result->load_count = slot->load_count;
    result->infer_count = slot->infer_count;
    result->cache_hits = context->cache_hits;

    switch (handle->category) {
        case BEAST2_MODEL_CATEGORY_DIFFUSION:
            return beast2_runtime_format_image(handle, request, result, error_message, error_message_size);
        case BEAST2_MODEL_CATEGORY_VIDEO:
            return beast2_runtime_format_video_manifest(handle, request, result, error_message, error_message_size);
        case BEAST2_MODEL_CATEGORY_LLM:
            return beast2_runtime_format_text_output(handle, request, result, error_message, error_message_size);
        case BEAST2_MODEL_CATEGORY_UNKNOWN:
        default:
            beast2_runtime_set_error(error_message, error_message_size, "runtime does not support the requested model category");
            return -1;
    }
}

void beast2_model_unload(
    beast2_model_runtime_context *context,
    const beast2_model_handle *handle
) {
    beast2_loaded_model *slot = NULL;

    if (context == NULL || handle == NULL || !handle->is_valid || handle->slot_index >= BEAST2_MAX_RUNTIME_MODELS) {
        return;
    }

    slot = &context->loaded_models[handle->slot_index];

    if (slot->occupied && slot->reference_count > 0) {
        slot->reference_count--;
        slot->last_used_tick = ++context->tick;
    }
}

beast2_model_category beast2_model_request_category(const beast2_model_request *request) {
    return beast2_runtime_parse_model_category(request);
}

beast2_runtime_backend beast2_model_request_backend(const beast2_model_request *request) {
    return beast2_runtime_parse_backend(request, beast2_runtime_parse_model_category(request));
}

beast2_precision_mode beast2_model_request_precision(const beast2_model_request *request) {
    return beast2_runtime_parse_precision(request);
}

const char *beast2_model_category_name(beast2_model_category category) {
    switch (category) {
        case BEAST2_MODEL_CATEGORY_DIFFUSION:
            return "diffusion";
        case BEAST2_MODEL_CATEGORY_VIDEO:
            return "video";
        case BEAST2_MODEL_CATEGORY_LLM:
            return "llm";
        case BEAST2_MODEL_CATEGORY_UNKNOWN:
        default:
            return "unknown";
    }
}

const char *beast2_runtime_backend_name(beast2_runtime_backend backend) {
    switch (backend) {
        case BEAST2_RUNTIME_BACKEND_ONNX:
            return "onnx";
        case BEAST2_RUNTIME_BACKEND_TENSORRT:
            return "tensorrt";
        case BEAST2_RUNTIME_BACKEND_LLAMA_CPP:
            return "llama_cpp";
        case BEAST2_RUNTIME_BACKEND_PYTHON:
            return "python";
        case BEAST2_RUNTIME_BACKEND_DEFAULT:
        default:
            return "default";
    }
}

const char *beast2_precision_mode_name(beast2_precision_mode precision) {
    switch (precision) {
        case BEAST2_PRECISION_FP32:
            return "fp32";
        case BEAST2_PRECISION_FP16:
            return "fp16";
        case BEAST2_PRECISION_INT8:
            return "int8";
        case BEAST2_PRECISION_DEFAULT:
        default:
            return "default";
    }
}

const char *beast2_output_kind_name(beast2_output_kind kind) {
    switch (kind) {
        case BEAST2_OUTPUT_KIND_IMAGE:
            return "image";
        case BEAST2_OUTPUT_KIND_VIDEO_MANIFEST:
            return "video_manifest";
        case BEAST2_OUTPUT_KIND_TEXT:
            return "text";
        case BEAST2_OUTPUT_KIND_UNKNOWN:
        default:
            return "unknown";
    }
}
