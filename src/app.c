#include "beast2/app.h"

#include <stdio.h>
#include <string.h>

#include "beast2/config.h"
#include "beast2/executor.h"
#include "beast2/filesystem.h"
#include "beast2/latent_explorer.h"
#include "beast2/logger.h"
#include "beast2/media_library.h"
#include "beast2/parser.h"
#include "beast2/scheduler.h"
#include "beast2/runtime.h"

typedef struct beast2_runtime_context {
    beast2_config config;
    beast2_logger logger;
    beast2_media_library_context media_library;
    beast2_gpu_scheduler_context scheduler;
    beast2_model_runtime_context model_runtime;
    char log_path[BEAST2_MAX_PATH_LENGTH];
} beast2_runtime_context;

static int beast2_prepare_runtime(
    const char *config_path,
    beast2_runtime_context *context,
    char *error_message,
    size_t error_message_size
) {
    char log_directory[BEAST2_MAX_PATH_LENGTH];

    memset(context, 0, sizeof(*context));

    if (
        beast2_config_load(
            &context->config,
            config_path,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    if (context->config.create_missing_directories) {
        if (
            beast2_fs_ensure_layout(
                context->config.workspace_root,
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }
    }

    if (beast2_fs_is_absolute(context->config.log_file)) {
        snprintf(context->log_path, sizeof(context->log_path), "%s", context->config.log_file);
    } else if (
        beast2_fs_join_path(
            context->log_path,
            sizeof(context->log_path),
            context->config.workspace_root,
            context->config.log_file
        ) != 0
    ) {
        snprintf(error_message, error_message_size, "%s", "failed to build log path");
        return -1;
    }

    if (
        beast2_fs_parent_directory(
            context->log_path,
            log_directory,
            sizeof(log_directory)
        ) != 0
    ) {
        snprintf(error_message, error_message_size, "%s", "failed to derive log directory");
        return -1;
    }

    if (context->config.create_missing_directories) {
        if (
            beast2_fs_mkdirs(
                log_directory,
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }
    }

    if (
        beast2_logger_init(
            &context->logger,
            context->log_path,
            context->config.log_to_stderr,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    if (
        beast2_media_library_init(
            &context->media_library,
            context->config.workspace_root,
            error_message,
            error_message_size
        ) != 0
    ) {
        beast2_logger_close(&context->logger);
        return -1;
    }

    {
        beast2_gpu_scheduler_config scheduler_config;

        memset(&scheduler_config, 0, sizeof(scheduler_config));
        scheduler_config.total_vram_mb = context->config.scheduler_total_vram_mb;
        scheduler_config.model_cache_vram_mb = context->config.scheduler_model_cache_vram_mb;
        scheduler_config.generation_vram_mb = context->config.scheduler_generation_vram_mb;
        scheduler_config.preview_vram_mb = context->config.scheduler_preview_vram_mb;
        scheduler_config.buffer_vram_mb = context->config.scheduler_buffer_vram_mb;
        beast2_gpu_scheduler_init(&context->scheduler, &scheduler_config);
    }

    beast2_model_runtime_init(&context->model_runtime);
    return 0;
}

static void beast2_cleanup_runtime(beast2_runtime_context *context) {
    beast2_model_runtime_shutdown(&context->model_runtime);
    beast2_gpu_scheduler_shutdown(&context->scheduler);
    beast2_media_library_shutdown(&context->media_library);
    beast2_logger_close(&context->logger);
}

static void beast2_print_metadata_summary(const beast2_generator_document *document) {
    size_t section_index = 0;
    size_t instruction_index = 0;

    if (document->metadata_section_count == 0) {
        printf("Metadata sections: none\n");
        return;
    }

    printf("Metadata sections:\n");

    for (section_index = 0; section_index < document->metadata_section_count; section_index++) {
        const beast2_metadata_section *section = &document->metadata_sections[section_index];

        printf("  - $%s (%zu instruction%s)\n",
               section->name,
               section->instruction_count,
               section->instruction_count == 1 ? "" : "s");

        for (instruction_index = 0; instruction_index < section->instruction_count; instruction_index++) {
            const beast2_instruction *instruction = &section->instructions[instruction_index];

            if (instruction->value[0] == '\0') {
                printf("      %s\n", instruction->name);
            } else {
                printf("      %s %s\n", instruction->name, instruction->value);
            }
        }
    }
}

static void beast2_print_prompt_summary(const beast2_generator_document *document) {
    size_t block_index = 0;
    size_t section_index = 0;

    if (document->prompt_block_count == 0) {
        printf("Prompt blocks: none\n");
        return;
    }

    printf("Prompt blocks:\n");

    for (block_index = 0; block_index < document->prompt_block_count; block_index++) {
        const beast2_prompt_block *block = &document->prompt_blocks[block_index];
        size_t snippet_total = 0;

        for (section_index = 0; section_index < block->section_count; section_index++) {
            snippet_total += block->sections[section_index].snippet_count;
        }

        printf("  - $%s: sections=%zu snippets=%zu variants=%zu\n",
               block->name,
               block->section_count,
               snippet_total,
               beast2_prompt_block_variant_count(block));
    }
}

static int beast2_print_prompt_variants(
    const beast2_prompt_block *block,
    int print_all_prompts
) {
    char prompt[8192];
    char error_message[512];
    size_t variant_count = beast2_prompt_block_variant_count(block);
    size_t limit = variant_count;
    size_t variant_index = 0;

    if (variant_count == 0) {
        printf("Resolved prompts: none\n");
        return 0;
    }

    if (!print_all_prompts && limit > 16) {
        limit = 16;
    }

    printf("Resolved prompts for $%s (%zu total, showing %zu):\n",
           block->name,
           variant_count,
           limit);

    for (variant_index = 0; variant_index < limit; variant_index++) {
        if (
            beast2_prompt_block_render_variant(
                block,
                variant_index,
                prompt,
                sizeof(prompt),
                error_message,
                sizeof(error_message)
            ) != 0
        ) {
            fprintf(stderr, "beast2: failed to render prompt: %s\n", error_message);
            return 1;
        }

        printf("  [%zu] %s\n", variant_index + 1, prompt);
    }

    if (limit < variant_count) {
        printf("  ... %zu additional prompt variants hidden; use --all-prompts to print them\n",
               variant_count - limit);
    }

    return 0;
}

int beast2_run(const char *config_path) {
    beast2_runtime_context runtime;
    beast2_scan_result scan_result;
    char error_message[512];

    memset(error_message, 0, sizeof(error_message));

    if (beast2_prepare_runtime(config_path, &runtime, error_message, sizeof(error_message)) != 0) {
        fprintf(stderr, "beast2: failed to prepare runtime: %s\n", error_message);
        return 1;
    }

    beast2_logger_log(&runtime.logger, BEAST2_LOG_LEVEL_INFO, "Beast2 phase zero startup");
    beast2_logger_log(&runtime.logger, BEAST2_LOG_LEVEL_INFO, "loaded config from %s", config_path);
    beast2_logger_log(&runtime.logger, BEAST2_LOG_LEVEL_INFO, "workspace root: %s", runtime.config.workspace_root);
    beast2_logger_log(&runtime.logger, BEAST2_LOG_LEVEL_INFO, "log file: %s", runtime.log_path);
    beast2_logger_log(
        &runtime.logger,
        BEAST2_LOG_LEVEL_INFO,
        "create missing directories: %s",
        runtime.config.create_missing_directories ? "true" : "false"
    );

    if (
        beast2_fs_scan_directories(
            runtime.config.workspace_root,
            &runtime.config,
            &runtime.logger,
            &scan_result,
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        beast2_logger_log(&runtime.logger, BEAST2_LOG_LEVEL_ERROR, "directory scan failed: %s", error_message);
        beast2_cleanup_runtime(&runtime);
        return 1;
    }

    beast2_logger_log(
        &runtime.logger,
        BEAST2_LOG_LEVEL_INFO,
        "scan complete: roots=%zu directories=%zu files=%zu missing_roots=%zu",
        scan_result.roots_scanned,
        scan_result.directories_seen,
        scan_result.files_seen,
        scan_result.missing_roots
    );

    printf("Beast2 phase zero boot complete.\n");
    printf("Config: %s\n", config_path);
    printf("Workspace: %s\n", runtime.config.workspace_root);
    printf("Log file: %s\n", runtime.log_path);
    printf(
        "Scan summary: roots=%zu directories=%zu files=%zu missing_roots=%zu\n",
        scan_result.roots_scanned,
        scan_result.directories_seen,
        scan_result.files_seen,
        scan_result.missing_roots
    );

    beast2_cleanup_runtime(&runtime);
    return 0;
}

int beast2_run_generator(const char *generator_path, int print_all_prompts) {
    beast2_generator_document document;
    const beast2_prompt_block *primary_block = NULL;
    char error_message[512];
    size_t warning_index = 0;

    memset(error_message, 0, sizeof(error_message));

    if (
        beast2_generator_parse_file(
            &document,
            generator_path,
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        fprintf(stderr, "beast2: failed to parse generator: %s\n", error_message);
        return 1;
    }

    printf("Beast2 phase one parser\n");
    printf("Source: %s\n", generator_path);

    if (document.generator_name != NULL) {
        printf("Generator: %s\n", document.generator_name);
    }

    beast2_print_prompt_summary(&document);
    beast2_print_metadata_summary(&document);

    if (document.warning_count > 0) {
        printf("Warnings:\n");

        for (warning_index = 0; warning_index < document.warning_count; warning_index++) {
            printf("  - %s\n", document.warnings[warning_index]);
        }
    }

    primary_block = beast2_generator_primary_prompt_block(&document);

    if (primary_block == NULL) {
        printf("No prompt blocks found in generator.\n");
        beast2_generator_document_free(&document);
        return 0;
    }

    if (beast2_print_prompt_variants(primary_block, print_all_prompts) != 0) {
        beast2_generator_document_free(&document);
        return 1;
    }

    beast2_generator_document_free(&document);
    return 0;
}

int beast2_run_generator_execution(const char *config_path, const char *generator_path) {
    beast2_runtime_context runtime;
    beast2_execution_summary summary;
    char error_message[512];

    memset(error_message, 0, sizeof(error_message));
    memset(&summary, 0, sizeof(summary));

    if (beast2_prepare_runtime(config_path, &runtime, error_message, sizeof(error_message)) != 0) {
        fprintf(stderr, "beast2: failed to prepare runtime: %s\n", error_message);
        return 1;
    }

    if (
        beast2_execute_generator(
            &runtime.config,
            &runtime.logger,
            &runtime.media_library,
            &runtime.scheduler,
            &runtime.model_runtime,
            generator_path,
            &summary,
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        beast2_logger_log(&runtime.logger, BEAST2_LOG_LEVEL_ERROR, "phase eleven execution failed: %s", error_message);
        beast2_cleanup_runtime(&runtime);
        fprintf(stderr, "beast2: failed to execute generator: %s\n", error_message);
        return 1;
    }

    printf("Beast2 phase eleven execution complete.\n");
    printf("Generator: %s\n", summary.generator_name);
    printf("Engine: %s\n", summary.engine);
    printf("Checkpoint: %s\n", summary.checkpoint);
    printf("Backend: %s\n", summary.backend);
    printf("Precision: %s\n", summary.precision);
    printf("Model type: %s\n", summary.model_category);
    printf("Output kind: %s\n", summary.output_kind);
    if (summary.llm_task[0] != '\0') {
        printf("LLM task: %s\n", summary.llm_task);
    }
    printf("Seed: %s\n", summary.seed);
    printf(
        "Job summary: total=%zu completed=%zu failed=%zu cache_hits=%zu cache_misses=%zu "
        "queue_peak=%zu model_evictions=%zu peak_reserved_vram_mb=%zu latent_records=%zu "
        "tensor_pool_hits=%zu tensor_pool_misses=%zu tensor_peak_reserved_cpu=%zu tensor_peak_reserved_gpu=%zu\n",
        summary.total_jobs,
        summary.completed_jobs,
        summary.failed_jobs,
        summary.cache_hits,
        summary.cache_misses,
        summary.scheduler_peak_queue_length,
        summary.scheduler_model_evictions,
        summary.scheduler_peak_reserved_vram_mb,
        summary.latent_records_created,
        summary.tensor_pool_hits,
        summary.tensor_pool_misses,
        summary.tensor_peak_reserved_cpu,
        summary.tensor_peak_reserved_gpu
    );
    printf("Database: %s\n", summary.database_path);
    printf("First output: %s\n", summary.first_output_path);
    printf("First thumbnail: %s\n", summary.first_thumbnail_path);
    printf("First generator artifact: %s\n", summary.first_generator_artifact_path);
    printf("First latent: %s\n", summary.first_latent_path);
    if (summary.generated_generator_path[0] != '\0') {
        printf("Generated generator: %s\n", summary.generated_generator_path);
    }
    if (summary.first_motion_latent_path[0] != '\0') {
        printf("First motion latent: %s\n", summary.first_motion_latent_path);
    }

    beast2_cleanup_runtime(&runtime);
    return 0;
}

int beast2_run_latent_explorer(
    const char *config_path,
    const long long latent_ids[4],
    double x,
    double y,
    const char *preview_mode
) {
    beast2_runtime_context runtime;
    beast2_latent_explorer_request request;
    beast2_latent_explorer_result result;
    char error_message[512];

    memset(error_message, 0, sizeof(error_message));
    memset(&request, 0, sizeof(request));
    memset(&result, 0, sizeof(result));

    if (beast2_prepare_runtime(config_path, &runtime, error_message, sizeof(error_message)) != 0) {
        fprintf(stderr, "beast2: failed to prepare runtime: %s\n", error_message);
        return 1;
    }

    memcpy(request.latent_ids, latent_ids, sizeof(request.latent_ids));
    request.x = x;
    request.y = y;
    request.preview_mode = preview_mode;

    if (
        beast2_latent_explorer_preview(
            &runtime.media_library,
            &runtime.scheduler,
            &runtime.model_runtime.tensor_memory,
            &request,
            &result,
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        beast2_cleanup_runtime(&runtime);
        fprintf(stderr, "beast2: failed to explore latents: %s\n", error_message);
        return 1;
    }

    printf("Beast2 phase nine latent explorer preview complete.\n");
    printf("Model: %s\n", result.model);
    printf("Latent type: %s\n", result.latent_type);
    printf("Preview mode: %s\n", preview_mode);
    printf("Preview path: %s\n", result.preview_path);
    printf("Cache key: %s\n", result.cache_key);
    printf("Cache hit: %s\n", result.cache_hit ? "true" : "false");
    printf("Scheduler cache hit: %s\n", result.scheduler_cache_hit ? "true" : "false");
    printf("Dimensions: %zux%zu\n", result.width, result.height);
    printf("Frame count: %zu\n", result.frame_count);
    printf("Blended bytes: %zu\n", result.blended_bytes);

    beast2_cleanup_runtime(&runtime);
    return 0;
}
