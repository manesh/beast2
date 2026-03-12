#include "beast2/latent_library.h"

#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "beast2/filesystem.h"

static void beast2_latent_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static uint64_t beast2_latent_hash_append(uint64_t hash, const char *value) {
    const unsigned char *cursor = (const unsigned char *) value;

    while (*cursor != '\0') {
        hash ^= (uint64_t) *cursor;
        hash *= 1099511628211ULL;
        cursor++;
    }

    return hash;
}

static uint64_t beast2_latent_hash_values(const char **values) {
    uint64_t hash = 1469598103934665603ULL;
    size_t index = 0;

    for (index = 0; values[index] != NULL; index++) {
        hash = beast2_latent_hash_append(hash, values[index]);
        hash = beast2_latent_hash_append(hash, "\n");
    }

    return hash;
}

static int beast2_latent_prepare_statement(
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

static int beast2_latent_step_done(
    sqlite3 *db,
    sqlite3_stmt *statement,
    char *error_message,
    size_t error_message_size
) {
    int code = sqlite3_step(statement);

    if (code != SQLITE_DONE) {
        snprintf(error_message, error_message_size, "sqlite step error: %s", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

static void beast2_latent_split_output_path(
    const char *output_relative_path,
    char *shard,
    size_t shard_size,
    char *stem,
    size_t stem_size
) {
    const char *slash = strrchr(output_relative_path, '/');
    const char *directory_shard = NULL;
    size_t shard_length = 0;
    size_t stem_length = 0;

    if (slash == NULL) {
        snprintf(shard, shard_size, "%s", "00");
        snprintf(stem, stem_size, "%s", "latent");
        return;
    }

    directory_shard = slash;
    while (directory_shard > output_relative_path && directory_shard[-1] != '/') {
        directory_shard--;
    }

    shard_length = (size_t) (slash - directory_shard);
    snprintf(shard, shard_size, "%.*s", (int) shard_length, directory_shard);
    stem_length = strcspn(slash + 1, ".");
    snprintf(stem, stem_size, "%.*s", (int) stem_length, slash + 1);
}

static int beast2_latent_write_file(
    beast2_tensor_memory_context *tensor_memory,
    const char *absolute_path,
    const char **hash_values,
    beast2_tensor_kind kind,
    size_t width,
    size_t height,
    size_t depth,
    size_t channels,
    char *error_message,
    size_t error_message_size
) {
    beast2_tensor_buffer buffer;
    beast2_tensor_desc desc;
    char parent_path[BEAST2_MAX_PATH_LENGTH];
    FILE *file = NULL;
    uint64_t state = 0;
    size_t byte_index = 0;

    memset(&buffer, 0, sizeof(buffer));
    memset(&desc, 0, sizeof(desc));

    desc.device = BEAST2_TENSOR_DEVICE_CPU;
    desc.dtype = BEAST2_TENSOR_DTYPE_FP16;
    desc.kind = kind;
    desc.width = width;
    desc.height = height;
    desc.depth = depth;
    desc.channels = channels;

    if (
        beast2_tensor_memory_acquire(
            tensor_memory,
            &desc,
            &buffer,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    state = beast2_latent_hash_values(hash_values);

    for (byte_index = 0; byte_index < buffer.size_bytes; byte_index++) {
        state = state * 6364136223846793005ULL + 1ULL;
        ((unsigned char *) buffer.data)[byte_index] = (unsigned char) ((state >> 24) & 0xff);
    }

    if (beast2_fs_parent_directory(absolute_path, parent_path, sizeof(parent_path)) != 0) {
        beast2_tensor_memory_release(tensor_memory, &buffer);
        beast2_latent_set_error(error_message, error_message_size, "failed to derive latent directory");
        return -1;
    }

    if (beast2_fs_mkdirs(parent_path, error_message, error_message_size) != 0) {
        beast2_tensor_memory_release(tensor_memory, &buffer);
        return -1;
    }

    file = fopen(absolute_path, "wb");
    if (file == NULL) {
        beast2_tensor_memory_release(tensor_memory, &buffer);
        snprintf(error_message, error_message_size, "failed to open latent file: %s", absolute_path);
        return -1;
    }

    if (fwrite(buffer.data, 1, buffer.size_bytes, file) != buffer.size_bytes) {
        fclose(file);
        beast2_tensor_memory_release(tensor_memory, &buffer);
        snprintf(error_message, error_message_size, "failed to write latent file: %s", absolute_path);
        return -1;
    }

    if (fclose(file) != 0) {
        beast2_tensor_memory_release(tensor_memory, &buffer);
        snprintf(error_message, error_message_size, "failed to close latent file: %s", absolute_path);
        return -1;
    }

    beast2_tensor_memory_release(tensor_memory, &buffer);
    return 0;
}

static int beast2_latent_insert_row(
    sqlite3 *db,
    long long media_id,
    const char *latent_type,
    const char *model,
    const char *shape,
    const char *file_path,
    const char *seed,
    size_t width,
    size_t height,
    size_t steps,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;

    if (
        beast2_latent_prepare_statement(
            db,
            &statement,
            "INSERT INTO latents (media_id, latent_type, model, shape, file_path, seed, width, height, steps) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) "
            "ON CONFLICT(file_path) DO UPDATE SET "
            "media_id=excluded.media_id, "
            "latent_type=excluded.latent_type, "
            "model=excluded.model, "
            "shape=excluded.shape, "
            "seed=excluded.seed, "
            "width=excluded.width, "
            "height=excluded.height, "
            "steps=excluded.steps;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_int64(statement, 1, media_id);
    sqlite3_bind_text(statement, 2, latent_type, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, model, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, shape, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, file_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 6, seed, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(statement, 7, (sqlite3_int64) width);
    sqlite3_bind_int64(statement, 8, (sqlite3_int64) height);
    sqlite3_bind_int64(statement, 9, (sqlite3_int64) steps);

    if (beast2_latent_step_done(db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return -1;
    }

    sqlite3_finalize(statement);
    return 0;
}

int beast2_latent_library_capture(
    beast2_media_library_context *media_library,
    beast2_tensor_memory_context *tensor_memory,
    const beast2_latent_capture_request *request,
    beast2_latent_capture_result *result,
    char *error_message,
    size_t error_message_size
) {
    char shard[16];
    char stem[BEAST2_MAX_PATH_LENGTH];
    char latent_relative_path[BEAST2_MAX_PATH_LENGTH];
    char latent_absolute_path[BEAST2_MAX_PATH_LENGTH];
    char latent_shape[128];
    char steps_buffer[32];
    char width_buffer[32];
    char height_buffer[32];
    char frames_buffer[32];
    const char *hash_values[10];
    size_t latent_width = 0;
    size_t latent_height = 0;
    size_t steps = 0;

    memset(result, 0, sizeof(*result));

    if (
        media_library == NULL ||
        media_library->db == NULL ||
        tensor_memory == NULL ||
        request == NULL
    ) {
        beast2_latent_set_error(error_message, error_message_size, "latent library context is invalid");
        return -1;
    }

    if (strcmp(request->output_kind, "image") != 0 && strcmp(request->output_kind, "video") != 0) {
        return 0;
    }

    beast2_latent_split_output_path(request->output_relative_path, shard, sizeof(shard), stem, sizeof(stem));
    latent_width = request->width > 7 ? request->width / 8 : 1;
    latent_height = request->height > 7 ? request->height / 8 : 1;
    steps = (size_t) strtoull(request->steps != NULL ? request->steps : "0", NULL, 10);

    snprintf(steps_buffer, sizeof(steps_buffer), "%zu", steps);
    snprintf(width_buffer, sizeof(width_buffer), "%zu", request->width);
    snprintf(height_buffer, sizeof(height_buffer), "%zu", request->height);
    snprintf(frames_buffer, sizeof(frames_buffer), "%zu", request->frame_count);

    if (strcmp(request->output_kind, "image") == 0) {
        if (
            snprintf(
                latent_relative_path,
                sizeof(latent_relative_path),
                "latents/image/%s/%s.lat",
                shard,
                stem
            ) >= (int) sizeof(latent_relative_path)
        ) {
            beast2_latent_set_error(error_message, error_message_size, "latent path exceeds supported length");
            return -1;
        }

        if (
            beast2_fs_join_path(
                latent_absolute_path,
                sizeof(latent_absolute_path),
                media_library->workspace_root,
                latent_relative_path
            ) != 0
        ) {
            beast2_latent_set_error(error_message, error_message_size, "failed to build latent path");
            return -1;
        }

        hash_values[0] = request->engine;
        hash_values[1] = request->checkpoint;
        hash_values[2] = request->seed;
        hash_values[3] = request->prompt_hash;
        hash_values[4] = width_buffer;
        hash_values[5] = height_buffer;
        hash_values[6] = steps_buffer;
        hash_values[7] = "image_latent";
        hash_values[8] = NULL;

        if (
            beast2_latent_write_file(
                tensor_memory,
                latent_absolute_path,
                hash_values,
                BEAST2_TENSOR_KIND_LATENT,
                latent_width,
                latent_height,
                1,
                4,
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }

        snprintf(latent_shape, sizeof(latent_shape), "4x%zux%zu", latent_height, latent_width);

        if (
            beast2_latent_insert_row(
                media_library->db,
                request->media_id,
                "image_latent",
                request->engine,
                latent_shape,
                latent_relative_path,
                request->seed,
                request->width,
                request->height,
                steps,
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }

        result->latent_count = 1;
        snprintf(result->first_latent_relative_path, sizeof(result->first_latent_relative_path), "%s", latent_relative_path);
        snprintf(result->first_latent_path, sizeof(result->first_latent_path), "%s", latent_absolute_path);
        return 0;
    }

    /* video latent */
    if (
        snprintf(
            latent_relative_path,
            sizeof(latent_relative_path),
            "latents/video/%s/%s.lat",
            shard,
            stem
        ) >= (int) sizeof(latent_relative_path)
    ) {
        beast2_latent_set_error(error_message, error_message_size, "video latent path exceeds supported length");
        return -1;
    }

    if (
        beast2_fs_join_path(
            latent_absolute_path,
            sizeof(latent_absolute_path),
            media_library->workspace_root,
            latent_relative_path
        ) != 0
    ) {
        beast2_latent_set_error(error_message, error_message_size, "failed to build video latent path");
        return -1;
    }

    hash_values[0] = request->engine;
    hash_values[1] = request->checkpoint;
    hash_values[2] = request->seed;
    hash_values[3] = request->prompt_hash;
    hash_values[4] = width_buffer;
    hash_values[5] = height_buffer;
    hash_values[6] = frames_buffer;
    hash_values[7] = "video_latent";
    hash_values[8] = NULL;

    if (
        beast2_latent_write_file(
            tensor_memory,
            latent_absolute_path,
            hash_values,
            BEAST2_TENSOR_KIND_VIDEO,
            latent_width,
            latent_height,
            request->frame_count > 0 ? request->frame_count : 1,
            4,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    snprintf(
        latent_shape,
        sizeof(latent_shape),
        "%zux4x%zux%zu",
        request->frame_count > 0 ? request->frame_count : 1,
        latent_height,
        latent_width
    );

    if (
        beast2_latent_insert_row(
            media_library->db,
            request->media_id,
            "video_latent",
            request->engine,
            latent_shape,
            latent_relative_path,
            request->seed,
            request->width,
            request->height,
            steps,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    result->latent_count++;
    snprintf(result->first_latent_relative_path, sizeof(result->first_latent_relative_path), "%s", latent_relative_path);
    snprintf(result->first_latent_path, sizeof(result->first_latent_path), "%s", latent_absolute_path);

    /* motion vector latent */
    if (
        snprintf(
            result->first_motion_relative_path,
            sizeof(result->first_motion_relative_path),
            "latents/motion/%s/%s.motion",
            shard,
            stem
        ) >= (int) sizeof(result->first_motion_relative_path)
    ) {
        beast2_latent_set_error(error_message, error_message_size, "motion latent path exceeds supported length");
        return -1;
    }

    if (
        beast2_fs_join_path(
            result->first_motion_path,
            sizeof(result->first_motion_path),
            media_library->workspace_root,
            result->first_motion_relative_path
        ) != 0
    ) {
        beast2_latent_set_error(error_message, error_message_size, "failed to build motion latent path");
        return -1;
    }

    hash_values[7] = "motion_vector";

    if (
        beast2_latent_write_file(
            tensor_memory,
            result->first_motion_path,
            hash_values,
            BEAST2_TENSOR_KIND_VIDEO,
            latent_width,
            latent_height,
            request->frame_count > 0 ? request->frame_count : 1,
            2,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    snprintf(
        latent_shape,
        sizeof(latent_shape),
        "%zux2x%zux%zu",
        request->frame_count > 0 ? request->frame_count : 1,
        latent_height,
        latent_width
    );

    if (
        beast2_latent_insert_row(
            media_library->db,
            request->media_id,
            "motion_vector",
            request->engine,
            latent_shape,
            result->first_motion_relative_path,
            request->seed,
            request->width,
            request->height,
            steps,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    result->latent_count++;
    return 0;
}
