#include <string.h>

#include "beast2/scheduler.h"
#include "test_support.h"

static beast2_gpu_scheduler_config beast2_test_scheduler_config(void) {
    beast2_gpu_scheduler_config config;

    memset(&config, 0, sizeof(config));
    config.total_vram_mb = 12288;
    config.model_cache_vram_mb = 4096;
    config.generation_vram_mb = 6144;
    config.preview_vram_mb = 1024;
    config.buffer_vram_mb = 1024;
    return config;
}

static int test_priority_ordering(void) {
    beast2_gpu_scheduler_context context;
    beast2_gpu_scheduler_config config = beast2_test_scheduler_config();
    beast2_gpu_job_request batch_request;
    beast2_gpu_job_request preview_request;
    beast2_gpu_job_request background_request;
    beast2_gpu_job_ticket ticket;
    beast2_gpu_scheduler_telemetry telemetry;
    char error_message[512];

    memset(&batch_request, 0, sizeof(batch_request));
    memset(&preview_request, 0, sizeof(preview_request));
    memset(&background_request, 0, sizeof(background_request));
    memset(&ticket, 0, sizeof(ticket));
    memset(&telemetry, 0, sizeof(telemetry));

    beast2_gpu_scheduler_init(&context, &config);

    batch_request.job_key = "batch-job";
    batch_request.engine = "sdxl";
    batch_request.checkpoint = "sdxl#sched001";
    batch_request.category = BEAST2_MODEL_CATEGORY_DIFFUSION;
    batch_request.priority = BEAST2_GPU_JOB_PRIORITY_BATCH_GENERATION;
    batch_request.estimated_model_vram_mb = 3000;
    batch_request.estimated_job_vram_mb = 1500;

    preview_request.job_key = "preview-job";
    preview_request.engine = "sdxl";
    preview_request.checkpoint = "sdxl#sched001";
    preview_request.category = BEAST2_MODEL_CATEGORY_DIFFUSION;
    preview_request.priority = BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW;
    preview_request.estimated_model_vram_mb = 3000;
    preview_request.estimated_ui_vram_mb = 512;

    background_request.job_key = "background-job";
    background_request.engine = "qwen";
    background_request.checkpoint = "qwen#sched001";
    background_request.category = BEAST2_MODEL_CATEGORY_LLM;
    background_request.priority = BEAST2_GPU_JOB_PRIORITY_BACKGROUND;
    background_request.estimated_model_vram_mb = 2000;
    background_request.estimated_job_vram_mb = 512;

    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_enqueue(&context, &batch_request, &ticket, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_enqueue(&context, &background_request, &ticket, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_enqueue(&context, &preview_request, &ticket, error_message, sizeof(error_message)) == 0
    );

    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_start_next(&context, &ticket, error_message, sizeof(error_message)) == 1
    );
    BEAST2_TEST_ASSERT_STRING_EQ(ticket.job_key, "preview-job");
    BEAST2_TEST_ASSERT(ticket.priority == BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW);
    BEAST2_TEST_ASSERT(beast2_gpu_scheduler_complete(&context, &ticket, error_message, sizeof(error_message)) == 0);

    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_start_next(&context, &ticket, error_message, sizeof(error_message)) == 1
    );
    BEAST2_TEST_ASSERT_STRING_EQ(ticket.job_key, "batch-job");
    BEAST2_TEST_ASSERT(beast2_gpu_scheduler_complete(&context, &ticket, error_message, sizeof(error_message)) == 0);

    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_start_next(&context, &ticket, error_message, sizeof(error_message)) == 1
    );
    BEAST2_TEST_ASSERT_STRING_EQ(ticket.job_key, "background-job");
    BEAST2_TEST_ASSERT(beast2_gpu_scheduler_complete(&context, &ticket, error_message, sizeof(error_message)) == 0);

    beast2_gpu_scheduler_get_telemetry(&context, &telemetry);
    BEAST2_TEST_ASSERT(telemetry.peak_queue_length == 3);
    BEAST2_TEST_ASSERT(telemetry.jobs_completed == 3);
    BEAST2_TEST_ASSERT(telemetry.queue_length == 0);

    beast2_gpu_scheduler_shutdown(&context);
    return 0;
}

static int test_residency_cache_and_eviction(void) {
    beast2_gpu_scheduler_context context;
    beast2_gpu_scheduler_config config = beast2_test_scheduler_config();
    beast2_gpu_job_request first_request;
    beast2_gpu_job_request second_request;
    beast2_gpu_job_request third_request;
    beast2_gpu_job_ticket ticket;
    beast2_gpu_scheduler_telemetry telemetry;
    char error_message[512];

    memset(&first_request, 0, sizeof(first_request));
    memset(&second_request, 0, sizeof(second_request));
    memset(&third_request, 0, sizeof(third_request));
    memset(&ticket, 0, sizeof(ticket));
    memset(&telemetry, 0, sizeof(telemetry));

    beast2_gpu_scheduler_init(&context, &config);

    first_request.job_key = "cache-a1";
    first_request.engine = "sdxl";
    first_request.checkpoint = "sdxl#cache001";
    first_request.category = BEAST2_MODEL_CATEGORY_DIFFUSION;
    first_request.priority = BEAST2_GPU_JOB_PRIORITY_BATCH_GENERATION;
    first_request.estimated_model_vram_mb = 3000;
    first_request.estimated_job_vram_mb = 512;

    second_request = first_request;
    second_request.job_key = "cache-a2";

    third_request.job_key = "cache-b1";
    third_request.engine = "wan22";
    third_request.checkpoint = "wan22#cache002";
    third_request.category = BEAST2_MODEL_CATEGORY_VIDEO;
    third_request.priority = BEAST2_GPU_JOB_PRIORITY_BATCH_GENERATION;
    third_request.estimated_model_vram_mb = 3000;
    third_request.estimated_job_vram_mb = 512;

    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_enqueue(&context, &first_request, &ticket, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_start_next(&context, &ticket, error_message, sizeof(error_message)) == 1
    );
    BEAST2_TEST_ASSERT(ticket.model_cache_hit == 0);
    BEAST2_TEST_ASSERT(beast2_gpu_scheduler_complete(&context, &ticket, error_message, sizeof(error_message)) == 0);

    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_enqueue(&context, &second_request, &ticket, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_start_next(&context, &ticket, error_message, sizeof(error_message)) == 1
    );
    BEAST2_TEST_ASSERT(ticket.model_cache_hit == 1);
    BEAST2_TEST_ASSERT(beast2_gpu_scheduler_complete(&context, &ticket, error_message, sizeof(error_message)) == 0);

    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_enqueue(&context, &third_request, &ticket, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_gpu_scheduler_start_next(&context, &ticket, error_message, sizeof(error_message)) == 1
    );
    BEAST2_TEST_ASSERT(ticket.model_cache_hit == 0);
    BEAST2_TEST_ASSERT(beast2_gpu_scheduler_complete(&context, &ticket, error_message, sizeof(error_message)) == 0);

    beast2_gpu_scheduler_get_telemetry(&context, &telemetry);
    BEAST2_TEST_ASSERT(telemetry.cache_hits == 1);
    BEAST2_TEST_ASSERT(telemetry.cache_misses == 2);
    BEAST2_TEST_ASSERT(telemetry.model_evictions == 1);
    BEAST2_TEST_ASSERT(telemetry.resident_model_count == 1);

    beast2_gpu_scheduler_shutdown(&context);
    return 0;
}

int main(void) {
    if (test_priority_ordering() != 0) {
        return 1;
    }

    if (test_residency_cache_and_eviction() != 0) {
        return 1;
    }

    return 0;
}
