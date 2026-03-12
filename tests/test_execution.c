#include <string.h>

#include <sqlite3.h>
#include <sys/stat.h>

#include "beast2/config.h"
#include "beast2/executor.h"
#include "beast2/filesystem.h"
#include "beast2/logger.h"
#include "beast2/scheduler.h"
#include "beast2/runtime.h"
#include "test_support.h"

static int beast2_test_query_count(sqlite3 *db, const char *sql, int expected_count) {
    sqlite3_stmt *statement = NULL;
    int count = -1;

    if (sqlite3_prepare_v2(db, sql, -1, &statement, NULL) != SQLITE_OK) {
        return 1;
    }

    if (sqlite3_step(statement) == SQLITE_ROW) {
        count = sqlite3_column_int(statement, 0);
    }

    sqlite3_finalize(statement);
    return count == expected_count ? 0 : 1;
}

static int test_execute_generator_creates_outputs(void) {
    const char *workspace_root = BEAST2_TEST_BINARY_DIR "/test-workspaces/execution";
    const char *generator_path = BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/executable_generator.b2";
    beast2_config config;
    beast2_logger logger;
    beast2_media_library_context media_library;
    beast2_gpu_scheduler_context scheduler;
    beast2_gpu_scheduler_config scheduler_config;
    beast2_model_runtime_context runtime_context;
    beast2_execution_summary summary;
    char error_message[512];
    char log_path[BEAST2_MAX_PATH_LENGTH];
    char output_contents[8192];
    char artifact_contents[8192];
    char thumbnail_contents[4096];
    struct stat latent_stat;
    sqlite3 *db = NULL;

    memset(&logger, 0, sizeof(logger));
    memset(&media_library, 0, sizeof(media_library));
    memset(&summary, 0, sizeof(summary));
    memset(&scheduler_config, 0, sizeof(scheduler_config));
    scheduler_config.total_vram_mb = 24576;
    scheduler_config.model_cache_vram_mb = 8192;
    scheduler_config.generation_vram_mb = 12288;
    scheduler_config.preview_vram_mb = 2048;
    scheduler_config.buffer_vram_mb = 2048;
    beast2_gpu_scheduler_init(&scheduler, &scheduler_config);
    beast2_model_runtime_init(&runtime_context);

    BEAST2_TEST_ASSERT(beast2_test_prepare_clean_directory(workspace_root) == 0);
    BEAST2_TEST_ASSERT(
        beast2_fs_ensure_layout(workspace_root, error_message, sizeof(error_message)) == 0
    );
    beast2_config_set_defaults(&config);
    snprintf(config.workspace_root, sizeof(config.workspace_root), "%s", workspace_root);
    config.log_to_stderr = 0;

    BEAST2_TEST_ASSERT(
        beast2_fs_join_path(log_path, sizeof(log_path), workspace_root, "db/performance_logs/executor.log") == 0
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
            &summary,
            error_message,
            sizeof(error_message)
        ) == 0
    );

    beast2_logger_close(&logger);
    beast2_media_library_shutdown(&media_library);
    beast2_gpu_scheduler_shutdown(&scheduler);
    beast2_model_runtime_shutdown(&runtime_context);

    BEAST2_TEST_ASSERT_STRING_EQ(summary.generator_name, "phase3_sdxl_demo");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.engine, "sdxl");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.checkpoint, "sdxl#abc123");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.backend, "onnx");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.precision, "fp32");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.model_category, "diffusion");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.output_kind, "image");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.seed, "9001");
    BEAST2_TEST_ASSERT(summary.total_jobs == 2);
    BEAST2_TEST_ASSERT(summary.completed_jobs == 2);
    BEAST2_TEST_ASSERT(summary.failed_jobs == 0);
    BEAST2_TEST_ASSERT(summary.cache_hits == 1);
    BEAST2_TEST_ASSERT(summary.cache_misses == 1);
    BEAST2_TEST_ASSERT(summary.scheduler_peak_queue_length == 2);
    BEAST2_TEST_ASSERT(summary.scheduler_model_evictions == 0);
    BEAST2_TEST_ASSERT(summary.scheduler_peak_reserved_vram_mb > 0);
    BEAST2_TEST_ASSERT(summary.tensor_pool_hits > 0);
    BEAST2_TEST_ASSERT(summary.tensor_pool_misses > 0);
    BEAST2_TEST_ASSERT(summary.tensor_peak_reserved_cpu > 0);
    BEAST2_TEST_ASSERT(summary.tensor_peak_reserved_gpu > 0);
    BEAST2_TEST_ASSERT(summary.tensor_bytes_reused_cpu > 0);
    BEAST2_TEST_ASSERT(summary.tensor_bytes_reused_gpu > 0);
    BEAST2_TEST_ASSERT(summary.latent_records_created == 2);
    BEAST2_TEST_ASSERT(strstr(summary.database_path, "db/beast2.sqlite") != NULL);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_output_path) == 1);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_thumbnail_path) == 1);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_generator_artifact_path) == 1);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_latent_path) == 1);
    BEAST2_TEST_ASSERT(stat(summary.first_latent_path, &latent_stat) == 0);
    BEAST2_TEST_ASSERT(latent_stat.st_size > 0);

    BEAST2_TEST_ASSERT(
        beast2_test_read_text_file(summary.first_output_path, output_contents, sizeof(output_contents)) == 0
    );
    BEAST2_TEST_ASSERT(strstr(output_contents, "P3\n") == output_contents);
    BEAST2_TEST_ASSERT(strstr(output_contents, "# engine sdxl") != NULL);
    BEAST2_TEST_ASSERT(strstr(output_contents, "# checkpoint sdxl#abc123") != NULL);
    BEAST2_TEST_ASSERT(strstr(output_contents, "# seed 9001") != NULL);
    BEAST2_TEST_ASSERT(strstr(output_contents, "# prompt Subject: test subject alpha") != NULL);

    BEAST2_TEST_ASSERT(
        beast2_test_read_text_file(summary.first_generator_artifact_path, artifact_contents, sizeof(artifact_contents)) == 0
    );
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_checkpoint sdxl#abc123") != NULL);
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_backend onnx") != NULL);
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_precision fp32") != NULL);
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_model_type diffusion") != NULL);
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_output_kind image") != NULL);
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_seed 9001") != NULL);
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_output_path outputs/images/") != NULL);
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_snippet Subject: test subject alpha") != NULL);

    BEAST2_TEST_ASSERT(
        beast2_test_read_text_file(summary.first_thumbnail_path, thumbnail_contents, sizeof(thumbnail_contents)) == 0
    );
    BEAST2_TEST_ASSERT(strstr(thumbnail_contents, "P3\n") == thumbnail_contents);
    BEAST2_TEST_ASSERT(strstr(thumbnail_contents, "# beast2_thumbnail") != NULL);

    BEAST2_TEST_ASSERT(sqlite3_open(summary.database_path, &db) == SQLITE_OK);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM generators;", 1) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM media;", 2) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM tags;", 2) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM media_tags;", 4) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM latents;", 2) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM latents WHERE latent_type = 'image_latent';", 2) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM generator_history;", 2) == 0);
    sqlite3_close(db);

    BEAST2_TEST_ASSERT(beast2_test_remove_tree(workspace_root) == 0);

    return 0;
}

static int test_execute_video_generator_creates_webm_outputs(void) {
    const char *workspace_root = BEAST2_TEST_BINARY_DIR "/test-workspaces/execution-video";
    const char *generator_path = BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/video_generator.b2";
    beast2_config config;
    beast2_logger logger;
    beast2_media_library_context media_library;
    beast2_gpu_scheduler_context scheduler;
    beast2_gpu_scheduler_config scheduler_config;
    beast2_model_runtime_context runtime_context;
    beast2_execution_summary summary;
    char error_message[512];
    char log_path[BEAST2_MAX_PATH_LENGTH];
    char artifact_contents[8192];
    struct stat output_stat;
    struct stat preview_stat;
    struct stat latent_stat;
    struct stat motion_stat;
    sqlite3 *db = NULL;

    memset(&logger, 0, sizeof(logger));
    memset(&media_library, 0, sizeof(media_library));
    memset(&summary, 0, sizeof(summary));
    memset(&scheduler_config, 0, sizeof(scheduler_config));
    scheduler_config.total_vram_mb = 24576;
    scheduler_config.model_cache_vram_mb = 8192;
    scheduler_config.generation_vram_mb = 12288;
    scheduler_config.preview_vram_mb = 2048;
    scheduler_config.buffer_vram_mb = 2048;
    beast2_gpu_scheduler_init(&scheduler, &scheduler_config);
    beast2_model_runtime_init(&runtime_context);

    BEAST2_TEST_ASSERT(beast2_test_prepare_clean_directory(workspace_root) == 0);
    BEAST2_TEST_ASSERT(
        beast2_fs_ensure_layout(workspace_root, error_message, sizeof(error_message)) == 0
    );
    beast2_config_set_defaults(&config);
    snprintf(config.workspace_root, sizeof(config.workspace_root), "%s", workspace_root);
    config.log_to_stderr = 0;

    BEAST2_TEST_ASSERT(
        beast2_fs_join_path(log_path, sizeof(log_path), workspace_root, "db/performance_logs/video-executor.log") == 0
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
            &summary,
            error_message,
            sizeof(error_message)
        ) == 0
    );

    beast2_logger_close(&logger);
    beast2_media_library_shutdown(&media_library);
    beast2_gpu_scheduler_shutdown(&scheduler);
    beast2_model_runtime_shutdown(&runtime_context);

    BEAST2_TEST_ASSERT_STRING_EQ(summary.generator_name, "phase7_video_demo");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.engine, "wan22");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.checkpoint, "wan22#video123");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.model_category, "video");
    BEAST2_TEST_ASSERT_STRING_EQ(summary.output_kind, "video");
    BEAST2_TEST_ASSERT(summary.completed_jobs == 2);
    BEAST2_TEST_ASSERT(summary.failed_jobs == 0);
    BEAST2_TEST_ASSERT(summary.latent_records_created == 4);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_output_path) == 1);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_thumbnail_path) == 1);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_latent_path) == 1);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_motion_latent_path) == 1);
    BEAST2_TEST_ASSERT(stat(summary.first_output_path, &output_stat) == 0);
    BEAST2_TEST_ASSERT(stat(summary.first_thumbnail_path, &preview_stat) == 0);
    BEAST2_TEST_ASSERT(stat(summary.first_latent_path, &latent_stat) == 0);
    BEAST2_TEST_ASSERT(stat(summary.first_motion_latent_path, &motion_stat) == 0);
    BEAST2_TEST_ASSERT(output_stat.st_size > 0);
    BEAST2_TEST_ASSERT(preview_stat.st_size > 0);
    BEAST2_TEST_ASSERT(latent_stat.st_size > 0);
    BEAST2_TEST_ASSERT(motion_stat.st_size > 0);

    BEAST2_TEST_ASSERT(
        beast2_test_read_text_file(summary.first_generator_artifact_path, artifact_contents, sizeof(artifact_contents)) == 0
    );
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_model_type video") != NULL);
    BEAST2_TEST_ASSERT(strstr(artifact_contents, "b2_output_kind video") != NULL);

    BEAST2_TEST_ASSERT(sqlite3_open(summary.database_path, &db) == SQLITE_OK);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM media WHERE type = 'video';", 2) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM latents;", 4) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM latents WHERE latent_type = 'video_latent';", 2) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM latents WHERE latent_type = 'motion_vector';", 2) == 0);
    BEAST2_TEST_ASSERT(beast2_test_query_count(db, "SELECT COUNT(*) FROM generator_history;", 2) == 0);
    sqlite3_close(db);

    BEAST2_TEST_ASSERT(beast2_test_remove_tree(workspace_root) == 0);

    return 0;
}

int main(void) {
    if (test_execute_generator_creates_outputs() != 0) {
        return 1;
    }

    if (test_execute_video_generator_creates_webm_outputs() != 0) {
        return 1;
    }

    return 0;
}
