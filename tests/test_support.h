#ifndef BEAST2_TEST_SUPPORT_H
#define BEAST2_TEST_SUPPORT_H

#include <stdio.h>
#include <string.h>

#define BEAST2_TEST_ASSERT(condition)                                                   \
    do {                                                                                \
        if (!(condition)) {                                                             \
            fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #condition, __FILE__, __LINE__); \
            return 1;                                                                   \
        }                                                                               \
    } while (0)

#define BEAST2_TEST_ASSERT_STRING_EQ(actual, expected)                                  \
    do {                                                                                \
        if (strcmp((actual), (expected)) != 0) {                                        \
            fprintf(                                                                    \
                stderr,                                                                 \
                "Assertion failed: expected \"%s\" but got \"%s\" (%s:%d)\n",          \
                (expected),                                                             \
                (actual),                                                               \
                __FILE__,                                                               \
                __LINE__                                                                \
            );                                                                          \
            return 1;                                                                   \
        }                                                                               \
    } while (0)

int beast2_test_path_exists(const char *path);
int beast2_test_remove_tree(const char *path);
int beast2_test_prepare_clean_directory(const char *path);
int beast2_test_write_text_file(const char *path, const char *contents);
int beast2_test_read_text_file(const char *path, char *buffer, size_t buffer_size);

#endif
