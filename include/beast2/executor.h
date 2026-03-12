#ifndef BEAST2_EXECUTOR_H
#define BEAST2_EXECUTOR_H

#include <stddef.h>

#include "beast2/config.h"
#include "beast2/logger.h"
#include "beast2/media_library.h"
#include "beast2/scheduler.h"
#include "beast2/runtime.h"

typedef struct beast2_execution_summary {
    char generator_name[256];
    char engine[128];
    char checkpoint[256];
    char seed[64];
    char backend[64];
    char precision[32];
    char model_category[32];
    char output_kind[32];
    size_t total_jobs;
    size_t completed_jobs;
    size_t failed_jobs;
    size_t cache_hits;
    size_t cache_misses;
    size_t scheduler_peak_queue_length;
    size_t scheduler_model_evictions;
    size_t scheduler_peak_reserved_vram_mb;
    size_t tensor_pool_hits;
    size_t tensor_pool_misses;
    size_t tensor_peak_reserved_cpu;
    size_t tensor_peak_reserved_gpu;
    size_t tensor_bytes_reused_cpu;
    size_t tensor_bytes_reused_gpu;
    char database_path[BEAST2_MAX_PATH_LENGTH];
    char first_output_path[BEAST2_MAX_PATH_LENGTH];
    char first_thumbnail_path[BEAST2_MAX_PATH_LENGTH];
    char first_generator_artifact_path[BEAST2_MAX_PATH_LENGTH];
} beast2_execution_summary;

int beast2_execute_generator(
    const beast2_config *config,
    beast2_logger *logger,
    beast2_media_library_context *media_library,
    beast2_gpu_scheduler_context *scheduler,
    beast2_model_runtime_context *runtime_context,
    const char *generator_path,
    beast2_execution_summary *summary,
    char *error_message,
    size_t error_message_size
);

#endif
