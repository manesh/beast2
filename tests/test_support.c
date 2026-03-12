#include "test_support.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
#include <direct.h>
#include <io.h>
#define BEAST2_TEST_UNLINK _unlink
#define BEAST2_TEST_RMDIR _rmdir
#else
#include <dirent.h>
#include <unistd.h>
#define BEAST2_TEST_UNLINK unlink
#define BEAST2_TEST_RMDIR rmdir
#endif

#include "beast2/filesystem.h"

int beast2_test_path_exists(const char *path) {
    struct stat path_stat;

    return stat(path, &path_stat) == 0;
}

static int beast2_test_remove_tree_inner(const char *path) {
    struct stat path_stat;

    if (stat(path, &path_stat) != 0) {
        return errno == ENOENT ? 0 : -1;
    }

    if (!S_ISDIR(path_stat.st_mode)) {
        return BEAST2_TEST_UNLINK(path);
    }

#if defined(_WIN32)
    return BEAST2_TEST_RMDIR(path);
#else
    {
        DIR *directory = opendir(path);
        struct dirent *entry = NULL;

        if (directory == NULL) {
            return -1;
        }

        while ((entry = readdir(directory)) != NULL) {
            char child_path[BEAST2_MAX_PATH_LENGTH];

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            if (beast2_fs_join_path(child_path, sizeof(child_path), path, entry->d_name) != 0) {
                closedir(directory);
                return -1;
            }

            if (beast2_test_remove_tree_inner(child_path) != 0) {
                closedir(directory);
                return -1;
            }
        }

        closedir(directory);
        return BEAST2_TEST_RMDIR(path);
    }
#endif
}

int beast2_test_remove_tree(const char *path) {
    return beast2_test_remove_tree_inner(path);
}

int beast2_test_prepare_clean_directory(const char *path) {
    char error_message[512];

    if (beast2_test_remove_tree(path) != 0) {
        return -1;
    }

    return beast2_fs_mkdirs(path, error_message, sizeof(error_message));
}

int beast2_test_write_text_file(const char *path, const char *contents) {
    char parent_path[BEAST2_MAX_PATH_LENGTH];
    char error_message[512];
    FILE *file = NULL;

    if (beast2_fs_parent_directory(path, parent_path, sizeof(parent_path)) != 0) {
        return -1;
    }

    if (beast2_fs_mkdirs(parent_path, error_message, sizeof(error_message)) != 0) {
        return -1;
    }

    file = fopen(path, "w");

    if (file == NULL) {
        return -1;
    }

    if (fputs(contents, file) == EOF) {
        fclose(file);
        return -1;
    }

    if (fclose(file) != 0) {
        return -1;
    }

    return 0;
}
