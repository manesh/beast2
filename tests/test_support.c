#include "test_support.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "beast2/c_compat.h"

#if defined(_WIN32)
#include <direct.h>
#include <io.h>
#include <windows.h>
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
    {
        char search[BEAST2_MAX_PATH_LENGTH];
        HANDLE h_find = INVALID_HANDLE_VALUE;
        WIN32_FIND_DATAA find_data;

        if (strlen(path) + 3 >= sizeof(search)) {
            return -1;
        }

        if (snprintf(search, sizeof(search), "%s\\*", path) >= (int) sizeof(search)) {
            return -1;
        }

        h_find = FindFirstFileA(search, &find_data);
        if (h_find == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND) {
                return BEAST2_TEST_RMDIR(path);
            }
            return -1;
        }

        do {
            char child_path[BEAST2_MAX_PATH_LENGTH];

            if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
                continue;
            }

            if (beast2_fs_join_path(child_path, sizeof(child_path), path, find_data.cFileName) != 0) {
                FindClose(h_find);
                return -1;
            }

            if (beast2_test_remove_tree_inner(child_path) != 0) {
                FindClose(h_find);
                return -1;
            }
        } while (FindNextFileA(h_find, &find_data) != 0);

        FindClose(h_find);
        return BEAST2_TEST_RMDIR(path);
    }
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

int beast2_test_read_text_file(const char *path, char *buffer, size_t buffer_size) {
    FILE *file = NULL;
    size_t bytes_read = 0;

    if (buffer == NULL || buffer_size == 0) {
        return -1;
    }

    file = fopen(path, "r");

    if (file == NULL) {
        return -1;
    }

    bytes_read = fread(buffer, 1, buffer_size - 1, file);
    buffer[bytes_read] = '\0';

    if (ferror(file)) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}
