#include <string.h>

#include "beast2/config.h"
#include "beast2/executor.h"
#include "beast2/filesystem.h"
#include "beast2/logger.h"
#include "beast2/runtime.h"
#include "test_support.h"

static int test_execute_generator_creates_outputs(void) {
    const char *workspace_root = BEAST2_TEST_BINARY_DIR "/test-workspaces/execution";
    const char *generator_path = BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/executable_generator.b2";
    beast2_config config;
    beast2_logger logger;
    beast2_model_runtime_context runtime_context;
    beast2_execution_summary summary;
    char error_message[512];
    char log_path[BEAST2_MAX_PATH_LENGTH];
    char output_contents[8192];
    char artifact_contents[8192];

    memset(&logger, 0, sizeof(logger));
    memset(&summary, 0, sizeof(summary));
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
        beast2_execute_generator(
            &config,
            &logger,
            &runtime_context,
            generator_path,
            &summary,
            error_message,
            sizeof(error_message)
        ) == 0
    );

    beast2_logger_close(&logger);
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
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_output_path) == 1);
    BEAST2_TEST_ASSERT(beast2_test_path_exists(summary.first_generator_artifact_path) == 1);

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

    BEAST2_TEST_ASSERT(beast2_test_remove_tree(workspace_root) == 0);

    return 0;
}

int main(void) {
    return test_execute_generator_creates_outputs();
}
