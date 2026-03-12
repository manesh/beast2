#include "beast2/latent_explorer.h"

#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "beast2/filesystem.h"

typedef struct beast2_latent_record {
    long long latent_id;
    char latent_type[32];
    char model[128];
    char shape[128];
    char file_path[BEAST2_MAX_PATH_LENGTH];
    size_t width;
    size_t height;
    size_t frame_count;
    size_t channels;
    size_t bytes;
} beast2_latent_record;

static void beast2_latent_explorer_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static uint64_t beast2_latent_explorer_hash_append(uint64_t hash, const char *value) {
    const unsigned char *cursor = (const unsigned char *) value;

    while (*cursor != '\0') {
        hash ^= (uint64_t) *cursor;
        hash *= 1099511628211ULL;
        cursor++;
    }

    return hash;
}

static void beast2_latent_explorer_hash_hex(char *destination, size_t destination_size, const char **values) {
    uint64_t hash = 1469598103934665603ULL;
    size_t index = 0;

    for (index = 0; values[index] != NULL; index++) {
        hash = beast2_latent_explorer_hash_append(hash, values[index]);
        hash = beast2_latent_explorer_hash_append(hash, "\n");
    }

    snprintf(destination, destination_size, "%016llx", (unsigned long long) hash);
}

static int beast2_latent_explorer_prepare_statement(
    sqlite3 *db,
    sqlite3_stmt **statement,
    const char *sql,
    char *error_message,
    size_t error_message_size
) {
    if (sqlite3_prepare_v2(db, sql, -1, statement, NULL) != SQLITE_OK) {
        snprintf(error_message, error_message_size, "sqlite prepare error: %s", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

static void beast2_latent_explorer_parse_shape(
    beast2_latent_record *record
) {
    char buffer[128];
    char *part = NULL;
    char *cursor = NULL;
    size_t values[4];
    size_t count = 0;

    memset(values, 0, sizeof(values));
    snprintf(buffer, sizeof(buffer), "%s", record->shape);
    cursor = buffer;

    while ((part = strtok(cursor, "x")) != NULL && count < 4) {
        values[count++] = (size_t) strtoull(part, NULL, 10);
        cursor = NULL;
    }

    if (strcmp(record->latent_type, "image_latent") == 0 && count >= 3) {
        record->channels = values[0];
        record->frame_count = 1;
    } else if (count >= 4) {
        record->frame_count = values[0];
        record->channels = values[1];
    } else {
        record->frame_count = 1;
        record->channels = 1;
    }
}

static int beast2_latent_explorer_load_record(
    beast2_media_library_context *media_library,
    long long latent_id,
    beast2_latent_record *record,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    char absolute_path[BEAST2_MAX_PATH_LENGTH];
    struct stat path_stat;

    memset(record, 0, sizeof(*record));

    if (
        beast2_latent_explorer_prepare_statement(
            media_library->db,
            &statement,
            "SELECT latent_id, latent_type, model, shape, file_path, width, height "
            "FROM latents WHERE latent_id = ?;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_int64(statement, 1, latent_id);

    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        snprintf(error_message, error_message_size, "latent id not found: %lld", latent_id);
        return -1;
    }

    record->latent_id = sqlite3_column_int64(statement, 0);
    snprintf(record->latent_type, sizeof(record->latent_type), "%s", (const char *) sqlite3_column_text(statement, 1));
    snprintf(record->model, sizeof(record->model), "%s", (const char *) sqlite3_column_text(statement, 2));
    snprintf(record->shape, sizeof(record->shape), "%s", (const char *) sqlite3_column_text(statement, 3));
    snprintf(record->file_path, sizeof(record->file_path), "%s", (const char *) sqlite3_column_text(statement, 4));
    record->width = (size_t) sqlite3_column_int64(statement, 5);
    record->height = (size_t) sqlite3_column_int64(statement, 6);
    sqlite3_finalize(statement);

    beast2_latent_explorer_parse_shape(record);

    if (
        beast2_fs_join_path(
            absolute_path,
            sizeof(absolute_path),
            media_library->workspace_root,
            record->file_path
        ) != 0
    ) {
        beast2_latent_explorer_set_error(error_message, error_message_size, "failed to build latent path");
        return -1;
    }

    if (stat(absolute_path, &path_stat) != 0) {
        snprintf(error_message, error_message_size, "failed to stat latent file: %s", absolute_path);
        return -1;
    }

    record->bytes = (size_t) path_stat.st_size;
    return 0;
}

static int beast2_latent_explorer_read_file(
    beast2_media_library_context *media_library,
    const beast2_latent_record *record,
    beast2_tensor_memory_context *tensor_memory,
    beast2_tensor_buffer *buffer,
    char *error_message,
    size_t error_message_size
) {
    char absolute_path[BEAST2_MAX_PATH_LENGTH];
    FILE *file = NULL;

    if (
        beast2_fs_join_path(
            absolute_path,
            sizeof(absolute_path),
            media_library->workspace_root,
            record->file_path
        ) != 0
    ) {
        beast2_latent_explorer_set_error(error_message, error_message_size, "failed to build latent absolute path");
        return -1;
    }

    if (
        beast2_tensor_memory_acquire(
            tensor_memory,
            &(beast2_tensor_desc) {
                BEAST2_TENSOR_DEVICE_CPU,
                BEAST2_TENSOR_DTYPE_FP16,
                BEAST2_TENSOR_KIND_LATENT,
                1,
                1,
                1,
                1,
                record->bytes
            },
            buffer,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    file = fopen(absolute_path, "rb");
    if (file == NULL) {
        beast2_tensor_memory_release(tensor_memory, buffer);
        snprintf(error_message, error_message_size, "failed to open latent file: %s", absolute_path);
        return -1;
    }

    if (fread(buffer->data, 1, record->bytes, file) != record->bytes) {
        fclose(file);
        beast2_tensor_memory_release(tensor_memory, buffer);
        snprintf(error_message, error_message_size, "failed to read latent file: %s", absolute_path);
        return -1;
    }

    fclose(file);
    return 0;
}

static int beast2_latent_explorer_write_preview_image(
    const beast2_tensor_buffer *buffer,
    size_t width,
    size_t height,
    const char *path,
    char *error_message,
    size_t error_message_size
) {
    char parent_path[BEAST2_MAX_PATH_LENGTH];
    FILE *file = NULL;
    size_t x = 0;
    size_t y = 0;
    size_t offset = 0;

    if (beast2_fs_parent_directory(path, parent_path, sizeof(parent_path)) != 0) {
        beast2_latent_explorer_set_error(error_message, error_message_size, "failed to derive preview directory");
        return -1;
    }

    if (beast2_fs_mkdirs(parent_path, error_message, error_message_size) != 0) {
        return -1;
    }

    file = fopen(path, "w");
    if (file == NULL) {
        snprintf(error_message, error_message_size, "failed to open preview image: %s", path);
        return -1;
    }

    fprintf(file, "P3\n# beast2_latent_preview\n%zu %zu\n255\n", width, height);

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            const unsigned char *bytes = (const unsigned char *) buffer->data;
            unsigned int red = bytes[offset % buffer->size_bytes];
            unsigned int green = bytes[(offset + 1) % buffer->size_bytes];
            unsigned int blue = bytes[(offset + 2) % buffer->size_bytes];

            fprintf(file, "%u %u %u\n", red, green, blue);
            offset += 3;
        }
    }

    fclose(file);
    return 0;
}

static int beast2_latent_explorer_write_preview_video(
    const beast2_tensor_buffer *buffer,
    size_t width,
    size_t height,
    size_t frame_count,
    const char *path,
    char *error_message,
    size_t error_message_size
) {
    char parent_path[BEAST2_MAX_PATH_LENGTH];
    char command[8192];
    const unsigned char *bytes = (const unsigned char *) buffer->data;
    size_t fps = frame_count > 0 ? frame_count : 4;
    unsigned int base_red = bytes[0 % buffer->size_bytes];
    unsigned int base_green = bytes[1 % buffer->size_bytes];
    unsigned int base_blue = bytes[2 % buffer->size_bytes];
    unsigned int overlay_red = bytes[3 % buffer->size_bytes];
    unsigned int overlay_green = bytes[4 % buffer->size_bytes];
    unsigned int overlay_blue = bytes[5 % buffer->size_bytes];

    if (beast2_fs_parent_directory(path, parent_path, sizeof(parent_path)) != 0) {
        beast2_latent_explorer_set_error(error_message, error_message_size, "failed to derive preview directory");
        return -1;
    }

    if (beast2_fs_mkdirs(parent_path, error_message, error_message_size) != 0) {
        return -1;
    }

    if (
        snprintf(
            command,
            sizeof(command),
            "ffmpeg -hide_banner -loglevel error -y "
            "-f lavfi -i \"color=c=#%02x%02x%02x:size=%zux%zu:rate=%zu\" "
            "-t %d "
            "-vf \"drawbox=x=t*3:y=t*2:w=%zu:h=%zu:color=#%02x%02x%02x@0.85:t=fill,format=yuv420p\" "
            "-an -c:v libvpx-vp9 -crf 38 -b:v 0 \"%s\"",
            base_red,
            base_green,
            base_blue,
            width,
            height,
            fps,
            1,
            width / 4 > 0 ? width / 4 : 8,
            height / 4 > 0 ? height / 4 : 8,
            overlay_red,
            overlay_green,
            overlay_blue,
            path
        ) >= (int) sizeof(command)
    ) {
        beast2_latent_explorer_set_error(error_message, error_message_size, "video preview command exceeded supported length");
        return -1;
    }

    if (system(command) != 0) {
        snprintf(error_message, error_message_size, "ffmpeg failed to generate latent preview video: %s", path);
        return -1;
    }

    return 0;
}

int beast2_latent_explorer_preview(
    beast2_media_library_context *media_library,
    beast2_gpu_scheduler_context *scheduler,
    beast2_tensor_memory_context *tensor_memory,
    const beast2_latent_explorer_request *request,
    beast2_latent_explorer_result *result,
    char *error_message,
    size_t error_message_size
) {
    beast2_latent_record records[4];
    beast2_tensor_buffer input_buffers[4];
    beast2_tensor_buffer blended_buffer;
    beast2_gpu_job_request scheduler_request;
    beast2_gpu_job_ticket ticket;
    char x_buffer[64];
    char y_buffer[64];
    const char *hash_values[8];
    size_t input_index = 0;
    size_t byte_index = 0;

    memset(records, 0, sizeof(records));
    memset(input_buffers, 0, sizeof(input_buffers));
    memset(&blended_buffer, 0, sizeof(blended_buffer));
    memset(&scheduler_request, 0, sizeof(scheduler_request));
    memset(&ticket, 0, sizeof(ticket));
    memset(result, 0, sizeof(*result));

    if (
        media_library == NULL ||
        scheduler == NULL ||
        tensor_memory == NULL ||
        request == NULL
    ) {
        beast2_latent_explorer_set_error(error_message, error_message_size, "latent explorer context is invalid");
        return -1;
    }

    if (request->x < 0.0 || request->x > 1.0 || request->y < 0.0 || request->y > 1.0) {
        beast2_latent_explorer_set_error(error_message, error_message_size, "latent explorer coordinates must be between 0 and 1");
        return -1;
    }

    for (input_index = 0; input_index < 4; input_index++) {
        if (
            beast2_latent_explorer_load_record(
                media_library,
                request->latent_ids[input_index],
                &records[input_index],
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }
    }

    for (input_index = 1; input_index < 4; input_index++) {
        if (
            strcmp(records[input_index].latent_type, records[0].latent_type) != 0 ||
            strcmp(records[input_index].model, records[0].model) != 0 ||
            records[input_index].bytes != records[0].bytes
        ) {
            beast2_latent_explorer_set_error(error_message, error_message_size, "latent explorer requires matching latent types, models, and sizes");
            return -1;
        }
    }

    snprintf(x_buffer, sizeof(x_buffer), "%.4f", request->x);
    snprintf(y_buffer, sizeof(y_buffer), "%.4f", request->y);
    hash_values[0] = records[0].model;
    hash_values[1] = records[0].file_path;
    hash_values[2] = records[1].file_path;
    hash_values[3] = records[2].file_path;
    hash_values[4] = records[3].file_path;
    hash_values[5] = x_buffer;
    hash_values[6] = y_buffer;
    hash_values[7] = NULL;
    beast2_latent_explorer_hash_hex(result->cache_key, sizeof(result->cache_key), hash_values);

    snprintf(result->latent_type, sizeof(result->latent_type), "%s", records[0].latent_type);
    snprintf(result->model, sizeof(result->model), "%s", records[0].model);
    result->width = records[0].width;
    result->height = records[0].height;
    result->frame_count = records[0].frame_count;

    if (strcmp(records[0].latent_type, "image_latent") == 0) {
        snprintf(result->preview_path, sizeof(result->preview_path), "%s/cache/latent-previews/image/%s.ppm", media_library->workspace_root, result->cache_key);
    } else {
        snprintf(result->preview_path, sizeof(result->preview_path), "%s/cache/latent-previews/video/%s.webm", media_library->workspace_root, result->cache_key);
    }

    {
        struct stat path_stat;

        if (stat(result->preview_path, &path_stat) == 0) {
            result->cache_hit = 1;
        result->blended_bytes = records[0].bytes;
            return 0;
        }
    }

    scheduler_request.job_key = result->cache_key;
    scheduler_request.engine = records[0].model;
    scheduler_request.checkpoint = records[0].model;
    scheduler_request.category =
        strcmp(records[0].latent_type, "image_latent") == 0
            ? BEAST2_MODEL_CATEGORY_DIFFUSION
            : BEAST2_MODEL_CATEGORY_VIDEO;
    scheduler_request.priority = BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW;
    scheduler_request.estimated_model_vram_mb = 512;
    scheduler_request.estimated_job_vram_mb = 128;
    scheduler_request.estimated_ui_vram_mb = 256;
    scheduler_request.estimated_runtime_ms = 75;

    if (
        beast2_gpu_scheduler_enqueue(
            scheduler,
            &scheduler_request,
            &ticket,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    if (
        beast2_gpu_scheduler_start_next(
            scheduler,
            &ticket,
            error_message,
            error_message_size
        ) <= 0
    ) {
        return -1;
    }

    result->scheduler_cache_hit = ticket.model_cache_hit;

    for (input_index = 0; input_index < 4; input_index++) {
        if (
            beast2_latent_explorer_read_file(
                media_library,
                &records[input_index],
                tensor_memory,
                &input_buffers[input_index],
                error_message,
                error_message_size
            ) != 0
        ) {
            beast2_gpu_scheduler_fail(scheduler, &ticket, error_message, error_message_size);
            return -1;
        }
    }

    if (
        beast2_tensor_memory_acquire(
            tensor_memory,
            &(beast2_tensor_desc) {
                BEAST2_TENSOR_DEVICE_CPU,
                BEAST2_TENSOR_DTYPE_FP16,
                BEAST2_TENSOR_KIND_LATENT,
                1,
                1,
                1,
                1,
                records[0].bytes
            },
            &blended_buffer,
            error_message,
            error_message_size
        ) != 0
    ) {
        beast2_gpu_scheduler_fail(scheduler, &ticket, error_message, error_message_size);
        return -1;
    }

    for (byte_index = 0; byte_index < records[0].bytes; byte_index++) {
        const double a = ((unsigned char *) input_buffers[0].data)[byte_index];
        const double b = ((unsigned char *) input_buffers[1].data)[byte_index];
        const double c = ((unsigned char *) input_buffers[2].data)[byte_index];
        const double d = ((unsigned char *) input_buffers[3].data)[byte_index];
        const double blended =
            ((1.0 - request->x) * (1.0 - request->y) * a) +
            (request->x * (1.0 - request->y) * b) +
            ((1.0 - request->x) * request->y * c) +
            (request->x * request->y * d);

        ((unsigned char *) blended_buffer.data)[byte_index] = (unsigned char) blended;
    }

    result->blended_bytes = records[0].bytes;

    if (strcmp(records[0].latent_type, "image_latent") == 0) {
        if (
            beast2_latent_explorer_write_preview_image(
                &blended_buffer,
                records[0].width > 0 ? records[0].width : 64,
                records[0].height > 0 ? records[0].height : 64,
                result->preview_path,
                error_message,
                error_message_size
            ) != 0
        ) {
            beast2_gpu_scheduler_fail(scheduler, &ticket, error_message, error_message_size);
            return -1;
        }
    } else {
        if (
            beast2_latent_explorer_write_preview_video(
                &blended_buffer,
                records[0].width > 0 ? records[0].width : 64,
                records[0].height > 0 ? records[0].height : 64,
                records[0].frame_count > 0 ? records[0].frame_count : 4,
                result->preview_path,
                error_message,
                error_message_size
            ) != 0
        ) {
            beast2_gpu_scheduler_fail(scheduler, &ticket, error_message, error_message_size);
            return -1;
        }
    }

    for (input_index = 0; input_index < 4; input_index++) {
        beast2_tensor_memory_release(tensor_memory, &input_buffers[input_index]);
    }
    beast2_tensor_memory_release(tensor_memory, &blended_buffer);
    beast2_gpu_scheduler_complete(scheduler, &ticket, error_message, error_message_size);

    return 0;
}
