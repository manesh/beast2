#include <string.h>

#include "beast2/config.h"
#include "test_support.h"

static int test_defaults(void) {
    beast2_config config;

    beast2_config_set_defaults(&config);

    BEAST2_TEST_ASSERT_STRING_EQ(config.workspace_root, "./runtime/beast2");
    BEAST2_TEST_ASSERT_STRING_EQ(config.log_file, "db/performance_logs/beast2.log");
    BEAST2_TEST_ASSERT(config.log_to_stderr == 1);
    BEAST2_TEST_ASSERT(config.create_missing_directories == 1);
    BEAST2_TEST_ASSERT(config.scan_directory_count == 7);
    BEAST2_TEST_ASSERT_STRING_EQ(config.scan_directories[0], "models");
    BEAST2_TEST_ASSERT_STRING_EQ(config.scan_directories[6], "cache");

    return 0;
}

static int test_valid_config_load(void) {
    beast2_config config;
    char error_message[512];

    BEAST2_TEST_ASSERT(
        beast2_config_load(
            &config,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/config/valid.conf",
            error_message,
            sizeof(error_message)
        ) == 0
    );

    BEAST2_TEST_ASSERT_STRING_EQ(config.workspace_root, "./build/test_runtime/config_valid");
    BEAST2_TEST_ASSERT_STRING_EQ(config.log_file, "logs/test.log");
    BEAST2_TEST_ASSERT(config.log_to_stderr == 0);
    BEAST2_TEST_ASSERT(config.create_missing_directories == 0);
    BEAST2_TEST_ASSERT(config.scan_directory_count == 3);
    BEAST2_TEST_ASSERT_STRING_EQ(config.scan_directories[0], "generators");
    BEAST2_TEST_ASSERT_STRING_EQ(config.scan_directories[1], "outputs");
    BEAST2_TEST_ASSERT_STRING_EQ(config.scan_directories[2], "cache");

    return 0;
}

static int test_invalid_bool_fails(void) {
    beast2_config config;
    char error_message[512];

    BEAST2_TEST_ASSERT(
        beast2_config_load(
            &config,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/config/invalid_bool.conf",
            error_message,
            sizeof(error_message)
        ) != 0
    );
    BEAST2_TEST_ASSERT(strstr(error_message, "boolean values must be one of") != NULL);

    return 0;
}

static int test_unknown_key_fails(void) {
    beast2_config config;
    char error_message[512];

    BEAST2_TEST_ASSERT(
        beast2_config_load(
            &config,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/config/unknown_key.conf",
            error_message,
            sizeof(error_message)
        ) != 0
    );
    BEAST2_TEST_ASSERT(strstr(error_message, "unknown config key") != NULL);

    return 0;
}

int main(void) {
    if (test_defaults() != 0) {
        return 1;
    }

    if (test_valid_config_load() != 0) {
        return 1;
    }

    if (test_invalid_bool_fails() != 0) {
        return 1;
    }

    if (test_unknown_key_fails() != 0) {
        return 1;
    }

    return 0;
}
