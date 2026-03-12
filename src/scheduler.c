#include "beast2/scheduler.h"

#include <stdio.h>
#include <string.h>

static void beast2_scheduler_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static size_t beast2_scheduler_total_reserved_vram(const beast2_gpu_scheduler_context *context) {
    return
        context->telemetry.reserved_model_vram_mb +
        context->telemetry.reserved_job_vram_mb +
        context->telemetry.reserved_ui_vram_mb +
        context->config.buffer_vram_mb;
}

static void beast2_scheduler_refresh_telemetry(beast2_gpu_scheduler_context *context) {
    size_t index = 0;
    size_t resident_count = 0;
    size_t total_reserved = 0;

    for (index = 0; index < BEAST2_MAX_RESIDENT_MODELS; index++) {
        if (context->resident_models[index].occupied) {
            resident_count++;
        }
    }

    context->telemetry.resident_model_count = resident_count;
    total_reserved = beast2_scheduler_total_reserved_vram(context);

    if (total_reserved > context->telemetry.peak_reserved_vram_mb) {
        context->telemetry.peak_reserved_vram_mb = total_reserved;
    }

    if (context->config.total_vram_mb > total_reserved) {
        context->telemetry.available_vram_mb = context->config.total_vram_mb - total_reserved;
    } else {
        context->telemetry.available_vram_mb = 0;
    }
}

static void beast2_scheduler_build_residency_key(
    char *destination,
    size_t destination_size,
    const char *engine,
    const char *checkpoint
) {
    snprintf(
        destination,
        destination_size,
        "%s|%s",
        engine != NULL ? engine : "(engine)",
        checkpoint != NULL ? checkpoint : "(checkpoint)"
    );
}

static beast2_gpu_resident_model *beast2_scheduler_find_resident_model(
    beast2_gpu_scheduler_context *context,
    const char *residency_key
) {
    size_t index = 0;

    for (index = 0; index < BEAST2_MAX_RESIDENT_MODELS; index++) {
        if (
            context->resident_models[index].occupied &&
            strcmp(context->resident_models[index].residency_key, residency_key) == 0
        ) {
            return &context->resident_models[index];
        }
    }

    return NULL;
}

static beast2_gpu_resident_model *beast2_scheduler_choose_resident_slot(
    beast2_gpu_scheduler_context *context
) {
    size_t index = 0;
    beast2_gpu_resident_model *best = NULL;

    for (index = 0; index < BEAST2_MAX_RESIDENT_MODELS; index++) {
        if (!context->resident_models[index].occupied) {
            return &context->resident_models[index];
        }
    }

    for (index = 0; index < BEAST2_MAX_RESIDENT_MODELS; index++) {
        beast2_gpu_resident_model *candidate = &context->resident_models[index];

        if (candidate->reference_count != 0) {
            continue;
        }

        if (best == NULL || candidate->last_used_tick < best->last_used_tick) {
            best = candidate;
        }
    }

    return best;
}

static beast2_gpu_resident_model *beast2_scheduler_choose_eviction_target(
    beast2_gpu_scheduler_context *context
) {
    size_t index = 0;
    beast2_gpu_resident_model *best = NULL;

    for (index = 0; index < BEAST2_MAX_RESIDENT_MODELS; index++) {
        beast2_gpu_resident_model *candidate = &context->resident_models[index];

        if (!candidate->occupied || candidate->reference_count != 0) {
            continue;
        }

        if (best == NULL || candidate->last_used_tick < best->last_used_tick) {
            best = candidate;
        }
    }

    return best;
}

static int beast2_scheduler_ensure_model_residency(
    beast2_gpu_scheduler_context *context,
    beast2_gpu_queue_entry *entry,
    int *cache_hit,
    char *error_message,
    size_t error_message_size
) {
    beast2_gpu_resident_model *resident = NULL;

    resident = beast2_scheduler_find_resident_model(context, entry->residency_key);

    if (resident != NULL) {
        resident->reference_count++;
        resident->last_used_tick = ++context->tick;
        context->telemetry.cache_hits++;
        *cache_hit = 1;
        beast2_scheduler_refresh_telemetry(context);
        return 0;
    }

    while (
        context->telemetry.reserved_model_vram_mb + entry->estimated_model_vram_mb >
        context->config.model_cache_vram_mb
    ) {
        beast2_gpu_resident_model *eviction_target = beast2_scheduler_choose_eviction_target(context);

        if (eviction_target == NULL || eviction_target->reference_count != 0) {
            beast2_scheduler_set_error(
                error_message,
                error_message_size,
                "scheduler could not free enough model-cache VRAM"
            );
            return -1;
        }

        context->telemetry.reserved_model_vram_mb -= eviction_target->model_vram_mb;
        context->telemetry.model_evictions++;
        memset(eviction_target, 0, sizeof(*eviction_target));
    }

    resident = beast2_scheduler_choose_resident_slot(context);

    if (resident == NULL) {
        beast2_scheduler_set_error(error_message, error_message_size, "scheduler model residency table is full");
        return -1;
    }

    memset(resident, 0, sizeof(*resident));
    resident->occupied = 1;
    resident->model_vram_mb = entry->estimated_model_vram_mb;
    resident->reference_count = 1;
    resident->last_used_tick = ++context->tick;
    snprintf(resident->residency_key, sizeof(resident->residency_key), "%s", entry->residency_key);
    snprintf(resident->engine, sizeof(resident->engine), "%s", entry->engine);
    snprintf(resident->checkpoint, sizeof(resident->checkpoint), "%s", entry->checkpoint);
    context->telemetry.cache_misses++;
    context->telemetry.reserved_model_vram_mb += entry->estimated_model_vram_mb;
    *cache_hit = 0;
    beast2_scheduler_refresh_telemetry(context);
    return 0;
}

static int beast2_scheduler_entry_can_run(
    const beast2_gpu_scheduler_context *context,
    const beast2_gpu_queue_entry *entry
) {
    size_t projected_model_vram = context->telemetry.reserved_model_vram_mb;
    size_t projected_job_vram = context->telemetry.reserved_job_vram_mb;
    size_t projected_ui_vram = context->telemetry.reserved_ui_vram_mb;
    size_t total_reserved = 0;
    size_t total_available = 0;
    int has_resident_model = 0;
    const beast2_gpu_resident_model *resident = NULL;

    resident = beast2_scheduler_find_resident_model((beast2_gpu_scheduler_context *) context, entry->residency_key);
    has_resident_model = resident != NULL;

    if (!has_resident_model) {
        if (entry->estimated_model_vram_mb > context->config.model_cache_vram_mb) {
            return 0;
        }

        if (projected_model_vram + entry->estimated_model_vram_mb > context->config.model_cache_vram_mb) {
            /* still potentially runnable after eviction */
        } else {
            projected_model_vram += entry->estimated_model_vram_mb;
        }
    }

    if (entry->priority == BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW) {
        if (entry->estimated_ui_vram_mb > context->config.preview_vram_mb) {
            return 0;
        }

        if (projected_ui_vram + entry->estimated_ui_vram_mb > context->config.preview_vram_mb) {
            return 0;
        }

        projected_ui_vram += entry->estimated_ui_vram_mb;
    } else {
        if (entry->estimated_job_vram_mb > context->config.generation_vram_mb) {
            return 0;
        }

        if (projected_job_vram + entry->estimated_job_vram_mb > context->config.generation_vram_mb) {
            return 0;
        }

        projected_job_vram += entry->estimated_job_vram_mb;
    }

    total_reserved =
        projected_model_vram +
        projected_job_vram +
        projected_ui_vram +
        context->config.buffer_vram_mb;
    total_available = context->config.total_vram_mb;

    return total_reserved <= total_available;
}

static beast2_gpu_queue_entry *beast2_scheduler_pick_next_entry(
    beast2_gpu_scheduler_context *context
) {
    size_t index = 0;
    beast2_gpu_queue_entry *best = NULL;

    for (index = 0; index < BEAST2_MAX_GPU_QUEUE_JOBS; index++) {
        beast2_gpu_queue_entry *candidate = &context->queue[index];

        if (!candidate->occupied || candidate->state != BEAST2_GPU_JOB_STATE_QUEUED) {
            continue;
        }

        if (!beast2_scheduler_entry_can_run(context, candidate)) {
            continue;
        }

        if (
            best == NULL ||
            candidate->priority < best->priority ||
            (
                candidate->priority == best->priority &&
                candidate->enqueue_tick < best->enqueue_tick
            )
        ) {
            best = candidate;
        }
    }

    return best;
}

void beast2_gpu_scheduler_init(
    beast2_gpu_scheduler_context *context,
    const beast2_gpu_scheduler_config *config
) {
    memset(context, 0, sizeof(*context));

    context->config = *config;
    context->next_scheduler_job_id = 1;
    context->active_queue_index = BEAST2_MAX_GPU_QUEUE_JOBS;
    beast2_scheduler_refresh_telemetry(context);
}

void beast2_gpu_scheduler_shutdown(beast2_gpu_scheduler_context *context) {
    memset(context, 0, sizeof(*context));
}

int beast2_gpu_scheduler_enqueue(
    beast2_gpu_scheduler_context *context,
    const beast2_gpu_job_request *request,
    beast2_gpu_job_ticket *ticket,
    char *error_message,
    size_t error_message_size
) {
    size_t index = 0;
    beast2_gpu_queue_entry *entry = NULL;

    if (request == NULL || ticket == NULL) {
        beast2_scheduler_set_error(error_message, error_message_size, "scheduler enqueue request is invalid");
        return -1;
    }

    for (index = 0; index < BEAST2_MAX_GPU_QUEUE_JOBS; index++) {
        if (!context->queue[index].occupied) {
            entry = &context->queue[index];
            break;
        }
    }

    if (entry == NULL) {
        beast2_scheduler_set_error(error_message, error_message_size, "scheduler queue is full");
        return -1;
    }

    memset(entry, 0, sizeof(*entry));
    entry->occupied = 1;
    entry->scheduler_job_id = context->next_scheduler_job_id++;
    entry->priority = request->priority;
    entry->state = BEAST2_GPU_JOB_STATE_QUEUED;
    entry->enqueue_tick = ++context->tick;
    entry->estimated_model_vram_mb = request->estimated_model_vram_mb;
    entry->estimated_job_vram_mb = request->estimated_job_vram_mb;
    entry->estimated_ui_vram_mb = request->estimated_ui_vram_mb;
    entry->estimated_runtime_ms = request->estimated_runtime_ms;
    snprintf(entry->job_key, sizeof(entry->job_key), "%s", request->job_key != NULL ? request->job_key : "");
    snprintf(entry->engine, sizeof(entry->engine), "%s", request->engine != NULL ? request->engine : "");
    snprintf(entry->checkpoint, sizeof(entry->checkpoint), "%s", request->checkpoint != NULL ? request->checkpoint : "");
    beast2_scheduler_build_residency_key(
        entry->residency_key,
        sizeof(entry->residency_key),
        request->engine,
        request->checkpoint
    );

    context->telemetry.jobs_enqueued++;
    context->telemetry.queue_length++;
    if (context->telemetry.queue_length > context->telemetry.peak_queue_length) {
        context->telemetry.peak_queue_length = context->telemetry.queue_length;
    }
    beast2_scheduler_refresh_telemetry(context);

    memset(ticket, 0, sizeof(*ticket));
    ticket->scheduler_job_id = entry->scheduler_job_id;
    ticket->priority = entry->priority;
    ticket->state = entry->state;
    ticket->estimated_model_vram_mb = entry->estimated_model_vram_mb;
    ticket->estimated_job_vram_mb = entry->estimated_job_vram_mb;
    ticket->estimated_ui_vram_mb = entry->estimated_ui_vram_mb;
    ticket->queue_position = context->telemetry.queue_length;
    snprintf(ticket->job_key, sizeof(ticket->job_key), "%s", entry->job_key);
    snprintf(ticket->residency_key, sizeof(ticket->residency_key), "%s", entry->residency_key);

    return 0;
}

int beast2_gpu_scheduler_start_next(
    beast2_gpu_scheduler_context *context,
    beast2_gpu_job_ticket *ticket,
    char *error_message,
    size_t error_message_size
) {
    beast2_gpu_queue_entry *entry = NULL;
    int cache_hit = 0;

    if (ticket == NULL) {
        beast2_scheduler_set_error(error_message, error_message_size, "scheduler start request is invalid");
        return -1;
    }

    if (context->active_scheduler_job_id != 0) {
        beast2_scheduler_set_error(error_message, error_message_size, "scheduler already has an active GPU job");
        return -1;
    }

    entry = beast2_scheduler_pick_next_entry(context);

    if (entry == NULL) {
        return 0;
    }

    if (
        beast2_scheduler_ensure_model_residency(
            context,
            entry,
            &cache_hit,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    if (entry->priority == BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW) {
        context->telemetry.reserved_ui_vram_mb += entry->estimated_ui_vram_mb;
    } else {
        context->telemetry.reserved_job_vram_mb += entry->estimated_job_vram_mb;
    }

    entry->state = BEAST2_GPU_JOB_STATE_RUNNING;
    context->active_scheduler_job_id = entry->scheduler_job_id;
    context->active_queue_index = (size_t) (entry - context->queue);
    context->telemetry.queue_length--;
    context->telemetry.jobs_started++;
    beast2_scheduler_refresh_telemetry(context);

    memset(ticket, 0, sizeof(*ticket));
    ticket->scheduler_job_id = entry->scheduler_job_id;
    ticket->priority = entry->priority;
    ticket->state = entry->state;
    ticket->model_cache_hit = cache_hit;
    ticket->estimated_model_vram_mb = entry->estimated_model_vram_mb;
    ticket->estimated_job_vram_mb = entry->estimated_job_vram_mb;
    ticket->estimated_ui_vram_mb = entry->estimated_ui_vram_mb;
    snprintf(ticket->job_key, sizeof(ticket->job_key), "%s", entry->job_key);
    snprintf(ticket->residency_key, sizeof(ticket->residency_key), "%s", entry->residency_key);

    return 1;
}

static int beast2_scheduler_finish_active(
    beast2_gpu_scheduler_context *context,
    const beast2_gpu_job_ticket *ticket,
    beast2_gpu_job_state end_state,
    char *error_message,
    size_t error_message_size
) {
    beast2_gpu_queue_entry *entry = NULL;
    beast2_gpu_resident_model *resident = NULL;

    if (
        ticket == NULL ||
        context->active_scheduler_job_id == 0 ||
        ticket->scheduler_job_id != context->active_scheduler_job_id ||
        context->active_queue_index >= BEAST2_MAX_GPU_QUEUE_JOBS
    ) {
        beast2_scheduler_set_error(error_message, error_message_size, "scheduler active job completion request is invalid");
        return -1;
    }

    entry = &context->queue[context->active_queue_index];

    if (!entry->occupied || entry->scheduler_job_id != ticket->scheduler_job_id) {
        beast2_scheduler_set_error(error_message, error_message_size, "scheduler active queue entry was not found");
        return -1;
    }

    if (entry->priority == BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW) {
        if (context->telemetry.reserved_ui_vram_mb >= entry->estimated_ui_vram_mb) {
            context->telemetry.reserved_ui_vram_mb -= entry->estimated_ui_vram_mb;
        }
    } else {
        if (context->telemetry.reserved_job_vram_mb >= entry->estimated_job_vram_mb) {
            context->telemetry.reserved_job_vram_mb -= entry->estimated_job_vram_mb;
        }
    }

    resident = beast2_scheduler_find_resident_model(context, entry->residency_key);
    if (resident != NULL && resident->reference_count > 0) {
        resident->reference_count--;
        resident->last_used_tick = ++context->tick;
    }

    entry->state = end_state;
    entry->occupied = 0;
    memset(entry, 0, sizeof(*entry));

    context->active_scheduler_job_id = 0;
    context->active_queue_index = BEAST2_MAX_GPU_QUEUE_JOBS;

    if (end_state == BEAST2_GPU_JOB_STATE_COMPLETED) {
        context->telemetry.jobs_completed++;
    } else if (end_state == BEAST2_GPU_JOB_STATE_FAILED) {
        context->telemetry.jobs_failed++;
    }

    beast2_scheduler_refresh_telemetry(context);
    return 0;
}

int beast2_gpu_scheduler_complete(
    beast2_gpu_scheduler_context *context,
    const beast2_gpu_job_ticket *ticket,
    char *error_message,
    size_t error_message_size
) {
    return beast2_scheduler_finish_active(
        context,
        ticket,
        BEAST2_GPU_JOB_STATE_COMPLETED,
        error_message,
        error_message_size
    );
}

int beast2_gpu_scheduler_fail(
    beast2_gpu_scheduler_context *context,
    const beast2_gpu_job_ticket *ticket,
    char *error_message,
    size_t error_message_size
) {
    return beast2_scheduler_finish_active(
        context,
        ticket,
        BEAST2_GPU_JOB_STATE_FAILED,
        error_message,
        error_message_size
    );
}

void beast2_gpu_scheduler_get_telemetry(
    const beast2_gpu_scheduler_context *context,
    beast2_gpu_scheduler_telemetry *telemetry
) {
    *telemetry = context->telemetry;
}

const char *beast2_gpu_job_priority_name(beast2_gpu_job_priority priority) {
    switch (priority) {
        case BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW:
            return "interactive_preview";
        case BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_GENERATION:
            return "interactive_generation";
        case BEAST2_GPU_JOB_PRIORITY_BATCH_GENERATION:
            return "batch_generation";
        case BEAST2_GPU_JOB_PRIORITY_BACKGROUND:
        default:
            return "background";
    }
}
