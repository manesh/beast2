#include <string.h>

#include <sqlite3.h>

#include "beast2/executor.h"
#include "beast2/filesystem.h"
#include "beast2/logger.h"
#include "beast2/media_library.h"
#include "beast2/runtime.h"
#include "beast2/scheduler.h"
#include "test_support.h"

static int beast2_test_run_generator_with_context(
    const beast2_config *config,
    beast2_logger *logger,
    beast2_media_library_context *media_library,
    beast2_gpu_scheduler_context *scheduler,
    beast2_model_runtime_context *runtime_context,
    const char *generator_path,
    beast2_execution_summary *summary
) {
    char error_message[512];

    memset(error_message, 0, sizeof(error_message));
    return beast2_execute_generator(
        config,
        logger,
        media_library,
        scheduler,
        runtime_context,
        generator_path,
        summary,
        error_message,
        sizeof(error_message)
    );
}

static int beast2_test_contains(const char *path, const char *needle) {
    char contents[32768];

    if (beast2_test_read_text_file(path, contents, sizeof(contents)) != 0) {
        return 1;
    }

    return strstr(contents, needle) == NULL ? 1 : 0;
}

static int beast2_test_llm_workflows(void) {
    const char *workspace_root = BEAST2_TEST_BINARY_DIR "/test-workspaces/llm";
    beast2_config config;
    beast2_logger logger;
    beast2_media_library_context media_library;
    beast2_gpu_scheduler_context scheduler;
    beast2_gpu_scheduler_config scheduler_config;
    beast2_model_runtime_context runtime_context;
    beast2_execution_summary summary;
    char error_message[512];
    char log_path[BEAST2_MAX_PATH_LENGTH];
    sqlite3 *db = NULL;

    memset(&logger, 0, sizeof(logger));
    memset(&media_library, 0, sizeof(media_library));
    memset(&scheduler_config, 0, sizeof(scheduler_config));
    memset(&summary, 0, sizeof(summary));

    scheduler_config.total_vram_mb = 24576;
    scheduler_config.model_cache_vram_mb = 8192;
    scheduler_config.generation_vram_mb = 12288;
    scheduler_config.preview_vram_mb = 2048;
    scheduler_config.buffer_vram_mb = 2048;

    beast2_gpu_scheduler_init(&scheduler, &scheduler_config);
    beast2_model_runtime_init(&runtime_context);
    beast2_config_set_defaults(&config);
    snprintf(config.workspace_root, sizeof(config.workspace_root), "%s", workspace_root);
    config.log_to_stderr = 0;

    BEAST2_TEST_ASSERT(beast2_test_prepare_clean_directory(workspace_root) == 0);
    BEAST2_TEST_ASSERT(
        beast2_fs_ensure_layout(workspace_root, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_fs_join_path(log_path, sizeof(log_path), workspace_root, "db/performance_logs/llm.log") == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_logger_init(&logger, log_path, 0, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_media_library_init(&media_library, workspace_root, error_message, sizeof(error_message)) == 0
    );

    /* Seed the media database for database-query workflows. */
    BEAST2_TEST_ASSERT(
        beast2_test_run_generator_with_context(
            &config,
            &logger,
            &media_library,
            &scheduler,
            &runtime_context,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/executable_generator.b2",
            &summary
        ) == 0
    );

    memset(&summary, 0, sizeof(summary));
    BEAST2_TEST_ASSERT(
        beast2_test_run_generator_with_context(
            &config,
            &logger,
            &media_library,
            &scheduler,
            &runtime_context,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/llm_prompt_mutator.b2",
            &summary
        ) == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(summary.model_category, "llm");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.output_kind, "text");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.llm_task, "prompt_mutation");
    BEAST2_TEST_ASSERT(beast2_test_contains(summary.first_output_path, "BEAST2_PHASE10_PROMPT_MUTATION") == 0);
    BEAST2_TEST_ASSERT(beast2_test_contains(summary.first_output_path, "variant_1:") == 0);

    memset(&summary, 0, sizeof(summary));
    BEAST2_TEST_ASSERT(
        beast2_test_run_generator_with_context(
            &config,
            &logger,
            &media_library,
            &scheduler,
            &runtime_context,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/llm_generator_editor.b2",
            &summary
        ) == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(summary.llm_task, "generator_edit");
    BEAST2_TEST_ASSERT(summary.generated_generator_path[0] != '\0');
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.generated_generator_path) == 1);
    BEAST2_TEST_ASSERT(beast2_test_contains(summary.generated_generator_path, "# beast2 llm-generated generator candidate") == 0);
    BEAST2_TEST_ASSERT(beast2_test_contains(summary.generated_generator_path, "sdxl_character_concept_demo") == 0);

    memset(&summary, 0, sizeof(summary));
    BEAST2_TEST_ASSERT(
        beast2_test_run_generator_with_context(
            &config,
            &logger,
            &media_library,
            &scheduler,
            &runtime_context,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/llm_tag_generator.b2",
            &summary
        ) == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(summary.llm_task, "tag_generation");
    BEAST2_TEST_ASSERT(beast2_test_contains(summary.first_output_path, "BEAST2_PHASE10_TAG_GENERATION") == 0);
    BEAST2_TEST_ASSERT(beast2_test_contains(summary.first_output_path, "cinematic") == 0);

    memset(&summary, 0, sizeof(summary));
    BEAST2_TEST_ASSERT(
        beast2_test_run_generator_with_context(
            &config,
            &logger,
            &media_library,
            &scheduler,
            &runtime_context,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/llm_database_query.b2",
            &summary
        ) == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(summary.llm_task, "database_query");
    BEAST2_TEST_ASSERT(beast2_test_contains(summary.first_output_path, "BEAST2_PHASE10_DATABASE_QUERY") == 0);
    BEAST2_TEST_ASSERT(beast2_test_contains(summary.first_output_path, "query_results:") == 0);
    BEAST2_TEST_ASSERT(beast2_test_contains(summary.first_output_path, "row_1:") == 0);

    BEAST2_TEST_ASSERT(sqlite3_open(summary.database_path, &db) == SQLITE_OK);
    {
        sqlite3_stmt *statement = NULL;
        int tag_count = 0;
        BEAST2_TEST_ASSERT(
            sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM tags WHERE tag_name = 'cinematic';", -1, &statement, NULL) == SQLITE_OK
        );
        BEAST2_TEST_ASSERT(sqlite3_step(statement) == SQLITE_ROW);
        tag_count = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);
        BEAST2_TEST_ASSERT(tag_count >= 1);
    }
    sqlite3_close(db);

    beast2_logger_close(&logger);
    beast2_media_library_shutdown(&media_library);
    beast2_gpu_scheduler_shutdown(&scheduler);
    beast2_model_runtime_shutdown(&runtime_context);
    BEAST2_TEST_ASSERT(beast2_test_remove_tree(workspace_root) == 0);

    return 0;
}

int main(void) {
    return beast2_test_llm_workflows();
}
