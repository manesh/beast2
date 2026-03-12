#ifndef BEAST2_SCHEDULER_H
#define BEAST2_SCHEDULER_H

#include <stddef.h>

#include "beast2/runtime.h"

#define BEAST2_MAX_GPU_QUEUE_JOBS 512
#define BEAST2_MAX_RESIDENT_MODELS 64

typedef enum beast2_gpu_job_priority {
    BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW = 1,
    BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_GENERATION = 2,
    BEAST2_GPU_JOB_PRIORITY_BATCH_GENERATION = 3,
    BEAST2_GPU_JOB_PRIORITY_BACKGROUND = 4
} beast2_gpu_job_priority;

typedef enum beast2_gpu_job_state {
    BEAST2_GPU_JOB_STATE_QUEUED = 0,
    BEAST2_GPU_JOB_STATE_RUNNING = 1,
    BEAST2_GPU_JOB_STATE_COMPLETED = 2,
    BEAST2_GPU_JOB_STATE_FAILED = 3
} beast2_gpu_job_state;

typedef struct beast2_gpu_scheduler_config {
    size_t total_vram_mb;
    size_t model_cache_vram_mb;
    size_t generation_vram_mb;
    size_t preview_vram_mb;
    size_t buffer_vram_mb;
} beast2_gpu_scheduler_config;

typedef struct beast2_gpu_job_request {
    const char *job_key;
    const char *engine;
    const char *checkpoint;
    beast2_model_category category;
    beast2_gpu_job_priority priority;
    size_t estimated_model_vram_mb;
    size_t estimated_job_vram_mb;
    size_t estimated_ui_vram_mb;
    size_t estimated_runtime_ms;
} beast2_gpu_job_request;

typedef struct beast2_gpu_job_ticket {
    unsigned long long scheduler_job_id;
    beast2_gpu_job_priority priority;
    beast2_gpu_job_state state;
    int model_cache_hit;
    size_t estimated_model_vram_mb;
    size_t estimated_job_vram_mb;
    size_t estimated_ui_vram_mb;
    size_t queue_position;
    char job_key[64];
    char residency_key[512];
} beast2_gpu_job_ticket;

typedef struct beast2_gpu_scheduler_telemetry {
    size_t queue_length;
    size_t peak_queue_length;
    size_t jobs_enqueued;
    size_t jobs_started;
    size_t jobs_completed;
    size_t jobs_failed;
    size_t cache_hits;
    size_t cache_misses;
    size_t model_evictions;
    size_t reserved_model_vram_mb;
    size_t reserved_job_vram_mb;
    size_t reserved_ui_vram_mb;
    size_t available_vram_mb;
    size_t peak_reserved_vram_mb;
    size_t resident_model_count;
} beast2_gpu_scheduler_telemetry;

typedef struct beast2_gpu_resident_model {
    int occupied;
    char residency_key[512];
    char engine[128];
    char checkpoint[256];
    size_t model_vram_mb;
    size_t reference_count;
    size_t last_used_tick;
} beast2_gpu_resident_model;

typedef struct beast2_gpu_queue_entry {
    int occupied;
    unsigned long long scheduler_job_id;
    beast2_gpu_job_priority priority;
    beast2_gpu_job_state state;
    size_t enqueue_tick;
    size_t estimated_model_vram_mb;
    size_t estimated_job_vram_mb;
    size_t estimated_ui_vram_mb;
    size_t estimated_runtime_ms;
    char job_key[64];
    char residency_key[512];
    char engine[128];
    char checkpoint[256];
} beast2_gpu_queue_entry;

typedef struct beast2_gpu_scheduler_context {
    beast2_gpu_scheduler_config config;
    beast2_gpu_queue_entry queue[BEAST2_MAX_GPU_QUEUE_JOBS];
    beast2_gpu_resident_model resident_models[BEAST2_MAX_RESIDENT_MODELS];
    unsigned long long next_scheduler_job_id;
    unsigned long long active_scheduler_job_id;
    size_t active_queue_index;
    size_t tick;
    beast2_gpu_scheduler_telemetry telemetry;
} beast2_gpu_scheduler_context;

void beast2_gpu_scheduler_init(
    beast2_gpu_scheduler_context *context,
    const beast2_gpu_scheduler_config *config
);

void beast2_gpu_scheduler_shutdown(beast2_gpu_scheduler_context *context);

int beast2_gpu_scheduler_enqueue(
    beast2_gpu_scheduler_context *context,
    const beast2_gpu_job_request *request,
    beast2_gpu_job_ticket *ticket,
    char *error_message,
    size_t error_message_size
);

int beast2_gpu_scheduler_start_next(
    beast2_gpu_scheduler_context *context,
    beast2_gpu_job_ticket *ticket,
    char *error_message,
    size_t error_message_size
);

int beast2_gpu_scheduler_complete(
    beast2_gpu_scheduler_context *context,
    const beast2_gpu_job_ticket *ticket,
    char *error_message,
    size_t error_message_size
);

int beast2_gpu_scheduler_fail(
    beast2_gpu_scheduler_context *context,
    const beast2_gpu_job_ticket *ticket,
    char *error_message,
    size_t error_message_size
);

void beast2_gpu_scheduler_get_telemetry(
    const beast2_gpu_scheduler_context *context,
    beast2_gpu_scheduler_telemetry *telemetry
);

const char *beast2_gpu_job_priority_name(beast2_gpu_job_priority priority);

#endif
