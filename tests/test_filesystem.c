#include <string.h>

#include "beast2/config.h"
#include "beast2/filesystem.h"
#include "beast2/logger.h"
#include "test_support.h"

static int test_path_helpers(void) {
    char destination[BEAST2_MAX_PATH_LENGTH];

    BEAST2_TEST_ASSERT(beast2_fs_is_absolute("/tmp/example") == 1);
    BEAST2_TEST_ASSERT(beast2_fs_is_absolute("relative/path") == 0);

    BEAST2_TEST_ASSERT(
        beast2_fs_join_path(destination, sizeof(destination), "alpha", "beta") == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(destination, "alpha/beta");

    BEAST2_TEST_ASSERT(
        beast2_fs_parent_directory("alpha/beta/file.txt", destination, sizeof(destination)) == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(destination, "alpha/beta");

    return 0;
}

static int test_layout_and_scan(void) {
    const char *workspace_root = BEAST2_TEST_BINARY_DIR "/test-workspaces/filesystem";
    beast2_config config;
    beast2_logger logger;
    beast2_scan_result scan_result;
    char error_message[512];
    char path[BEAST2_MAX_PATH_LENGTH];
    char log_path[BEAST2_MAX_PATH_LENGTH];

    memset(&logger, 0, sizeof(logger));

    BEAST2_TEST_ASSERT(beast2_test_prepare_clean_directory(workspace_root) == 0);
    BEAST2_TEST_ASSERT(
        beast2_fs_ensure_layout(workspace_root, error_message, sizeof(error_message)) == 0
    );

    BEAST2_TEST_ASSERT(
        beast2_fs_join_path(path, sizeof(path), workspace_root, "models/diffusion") == 0
    );
    BEAST2_TEST_ASSERT(beast2_test_path_exists(path) == 1);

    BEAST2_TEST_ASSERT(
        beast2_fs_join_path(path, sizeof(path), workspace_root, "db/performance_logs") == 0
    );
    BEAST2_TEST_ASSERT(beast2_test_path_exists(path) == 1);

    BEAST2_TEST_ASSERT(
        beast2_fs_join_path(path, sizeof(path), workspace_root, "outputs/images/sample.txt") == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_test_write_text_file(path, "fixture output") == 0
    );

    BEAST2_TEST_ASSERT(
        beast2_fs_join_path(log_path, sizeof(log_path), workspace_root, "db/performance_logs/test.log") == 0
    );
    BEAST2_TEST_ASSERT(
        beast2_logger_init(&logger, log_path, 0, error_message, sizeof(error_message)) == 0
    );

    beast2_config_set_defaults(&config);
    config.scan_directory_count = 3;
    snprintf(config.scan_directories[0], sizeof(config.scan_directories[0]), "%s", "models");
    snprintf(config.scan_directories[1], sizeof(config.scan_directories[1]), "%s", "outputs");
    snprintf(config.scan_directories[2], sizeof(config.scan_directories[2]), "%s", "missing");

    BEAST2_TEST_ASSERT(
        beast2_fs_scan_directories(
            workspace_root,
            &config,
            &logger,
            &scan_result,
            error_message,
            sizeof(error_message)
        ) == 0
    );

    beast2_logger_close(&logger);

    BEAST2_TEST_ASSERT(scan_result.roots_scanned == 3);
    BEAST2_TEST_ASSERT(scan_result.directories_seen == 7);
    BEAST2_TEST_ASSERT(scan_result.files_seen == 1);
    BEAST2_TEST_ASSERT(scan_result.missing_roots == 1);

    BEAST2_TEST_ASSERT(beast2_test_remove_tree(workspace_root) == 0);

    return 0;
}

int main(void) {
    if (test_path_helpers() != 0) {
        return 1;
    }

    if (test_layout_and_scan() != 0) {
        return 1;
    }

    return 0;
}
