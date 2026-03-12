#include <string.h>

#include <sqlite3.h>

#include "beast2/filesystem.h"
#include "beast2/executor.h"
#include "beast2/latent_explorer.h"
#include "beast2/logger.h"
#include "beast2/media_library.h"
#include "beast2/scheduler.h"
#include "beast2/runtime.h"
#include "test_support.h"

static int beast2_test_collect_first_four_image_latents(sqlite3 *db, long long latent_ids[4]) {
    sqlite3_stmt *statement = NULL;
    int index = 0;

    if (
        sqlite3_prepare_v2(
            db,
            "SELECT latent_id FROM latents WHERE latent_type = 'image_latent' ORDER BY latent_id LIMIT 4;",
            -1,
            &statement,
            NULL
        ) != SQLITE_OK
    ) {
        return 1;
    }

    while (sqlite3_step(statement) == SQLITE_ROW && index < 4) {
        latent_ids[index++] = sqlite3_column_int64(statement, 0);
    }

    sqlite3_finalize(statement);
    return index == 4 ? 0 : 1;
}

static int test_latent_explorer_preview_and_cache(void) {
    const char *workspace_root = BEAST2_TEST_BINARY_DIR "/test-workspaces/explorer";
    const char *generator_path = BEAST2_TEST_SOURCE_DIR "/examples/sdxl_character_concept.b2";
    beast2_config config;
    beast2_logger logger;
    beast2_media_library_context media_library;
    beast2_gpu_scheduler_context scheduler;
    beast2_gpu_scheduler_config scheduler_config;
    beast2_model_runtime_context runtime_context;
    beast2_execution_summary execution_summary;
    beast2_latent_explorer_request request;
    beast2_latent_explorer_result result;
    beast2_latent_explorer_result cached_result;
    long long latent_ids[4] = {0, 0, 0, 0};
    char error_message[512];
    char log_path[BEAST2_MAX_PATH_LENGTH];
    char preview_contents[4096];
    sqlite3 *db = NULL;

    memset(&logger, 0, sizeof(logger));
    memset(&media_library, 0, sizeof(media_library));
    memset(&scheduler_config, 0, sizeof(scheduler_config));
    memset(&execution_summary, 0, sizeof(execution_summary));
    memset(&request, 0, sizeof(request));
    memset(&result, 0, sizeof(result));
    memset(&cached_result, 0, sizeof(cached_result));

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
        beast2_fs_join_path(log_path, sizeof(log_path), workspace_root, "db/performance_logs/explorer.log") == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_logger_init(&logger, log_path, 0, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_media_library_init(&media_library, workspace_root, error_message, sizeof(error_message)) == 0
    );

    BEAST2_TEST_ASSERT(
        beast2_execute_generator(
            &config,
            &logger,
            &media_library,
            &scheduler,
            &runtime_context,
            generator_path,
            &execution_summary,
            error_message,
            sizeof(error_message)
        ) == 0
    );

    BEAST2_TEST_ASSERT(sqlite3_open(execution_summary.database_path, &db) == SQLITE_OK);
    BEAST2_TEST_ASSERT(beast2_test_collect_first_four_image_latents(db, latent_ids) == 0);
    sqlite3_close(db);

    memcpy(request.latent_ids, latent_ids, sizeof(latent_ids));
    request.x = 0.5;
    request.y = 0.5;
    request.preview_mode = "decode-only";

    BEAST2_TEST_ASSERT(
        beast2_latent_explorer_preview(
            &media_library,
            &scheduler,
            &runtime_context.tensor_memory,
            &request,
            &result,
            error_message,
            sizeof(error_message)
        ) == 0
    );
    BEAST2_TEST_ASSERT(result.cache_hit == 0);
    BEAST2_TEST_ASSERT_STRING_EQ(result.latent_type, "image_latent");
    BEAST2_TEST_ASSERT(result.width > 0);
    BEAST2_TEST_ASSERT(result.height > 0);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(result.preview_path) == 1);
    BEAST2_TEST_ASSERT(
        beast2_test_read_text_file(result.preview_path, preview_contents, sizeof(preview_contents)) == 0
    );
    BEAST2_TEST_ASSERT(strstr(preview_contents, "P3\n") == preview_contents);
    BEAST2_TEST_ASSERT(strstr(preview_contents, "# beast2_latent_preview") != NULL);

    BEAST2_TEST_ASSERT(
        beast2_latent_explorer_preview(
            &media_library,
            &scheduler,
            &runtime_context.tensor_memory,
            &request,
            &cached_result,
            error_message,
            sizeof(error_message)
        ) == 0
    );
    BEAST2_TEST_ASSERT(cached_result.cache_hit == 1);
    BEAST2_TEST_ASSERT_STRING_EQ(cached_result.preview_path, result.preview_path);

    beast2_logger_close(&logger);
    beast2_media_library_shutdown(&media_library);
    beast2_gpu_scheduler_shutdown(&scheduler);
    beast2_model_runtime_shutdown(&runtime_context);
    BEAST2_TEST_ASSERT(beast2_test_remove_tree(workspace_root) == 0);

    return 0;
}

int main(void) {
    return test_latent_explorer_preview_and_cache();
}
