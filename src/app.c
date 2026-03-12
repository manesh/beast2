#include "beast2/app.h"

#include <stdio.h>
#include <string.h>

#include "beast2/config.h"
#include "beast2/filesystem.h"
#include "beast2/logger.h"
#include "beast2/parser.h"

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
    beast2_config config;
    beast2_logger logger;
    beast2_scan_result scan_result;
    char error_message[512];
    char log_path[BEAST2_MAX_PATH_LENGTH];
    char log_directory[BEAST2_MAX_PATH_LENGTH];

    memset(&logger, 0, sizeof(logger));
    memset(error_message, 0, sizeof(error_message));

    if (beast2_config_load(&config, config_path, error_message, sizeof(error_message)) != 0) {
        fprintf(stderr, "beast2: failed to load configuration: %s\n", error_message);
        return 1;
    }

    if (config.create_missing_directories) {
        if (
            beast2_fs_ensure_layout(
                config.workspace_root,
                error_message,
                sizeof(error_message)
            ) != 0
        ) {
            fprintf(stderr, "beast2: failed to prepare workspace: %s\n", error_message);
            return 1;
        }
    }

    if (beast2_fs_is_absolute(config.log_file)) {
        snprintf(log_path, sizeof(log_path), "%s", config.log_file);
    } else if (
        beast2_fs_join_path(
            log_path,
            sizeof(log_path),
            config.workspace_root,
            config.log_file
        ) != 0
    ) {
        fprintf(stderr, "beast2: failed to build log path\n");
        return 1;
    }

    if (beast2_fs_parent_directory(log_path, log_directory, sizeof(log_directory)) != 0) {
        fprintf(stderr, "beast2: failed to derive log directory\n");
        return 1;
    }

    if (config.create_missing_directories) {
        if (
            beast2_fs_mkdirs(
                log_directory,
                error_message,
                sizeof(error_message)
            ) != 0
        ) {
            fprintf(stderr, "beast2: failed to prepare log directory: %s\n", error_message);
            return 1;
        }
    }

    if (
        beast2_logger_init(
            &logger,
            log_path,
            config.log_to_stderr,
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        fprintf(stderr, "beast2: %s\n", error_message);
        return 1;
    }

    beast2_logger_log(&logger, BEAST2_LOG_LEVEL_INFO, "Beast2 phase zero startup");
    beast2_logger_log(&logger, BEAST2_LOG_LEVEL_INFO, "loaded config from %s", config_path);
    beast2_logger_log(&logger, BEAST2_LOG_LEVEL_INFO, "workspace root: %s", config.workspace_root);
    beast2_logger_log(&logger, BEAST2_LOG_LEVEL_INFO, "log file: %s", log_path);
    beast2_logger_log(
        &logger,
        BEAST2_LOG_LEVEL_INFO,
        "create missing directories: %s",
        config.create_missing_directories ? "true" : "false"
    );

    if (
        beast2_fs_scan_directories(
            config.workspace_root,
            &config,
            &logger,
            &scan_result,
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        beast2_logger_log(&logger, BEAST2_LOG_LEVEL_ERROR, "directory scan failed: %s", error_message);
        beast2_logger_close(&logger);
        return 1;
    }

    beast2_logger_log(
        &logger,
        BEAST2_LOG_LEVEL_INFO,
        "scan complete: roots=%zu directories=%zu files=%zu missing_roots=%zu",
        scan_result.roots_scanned,
        scan_result.directories_seen,
        scan_result.files_seen,
        scan_result.missing_roots
    );

    printf("Beast2 phase zero boot complete.\n");
    printf("Config: %s\n", config_path);
    printf("Workspace: %s\n", config.workspace_root);
    printf("Log file: %s\n", log_path);
    printf(
        "Scan summary: roots=%zu directories=%zu files=%zu missing_roots=%zu\n",
        scan_result.roots_scanned,
        scan_result.directories_seen,
        scan_result.files_seen,
        scan_result.missing_roots
    );

    beast2_logger_close(&logger);
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
