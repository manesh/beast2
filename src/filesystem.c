#include "beast2/filesystem.h"
#include "beast2/c_compat.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <direct.h>
#include <windows.h>
#else
#include <dirent.h>
#endif

static void beast2_fs_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static int beast2_fs_path_exists(const char *path, struct stat *path_stat) {
    if (stat(path, path_stat) != 0) {
        return -1;
    }

    return 0;
}

static int beast2_fs_make_directory(const char *path) {
#if defined(_WIN32)
    return _mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

#if defined(_WIN32)
static int beast2_fs_scan_tree(
    const char *path,
    beast2_scan_result *result,
    char *error_message,
    size_t error_message_size
) {
    char search[BEAST2_MAX_PATH_LENGTH];
    HANDLE h_find = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA find_data;

    if (strlen(path) + 3 >= sizeof(search)) {
        beast2_fs_set_error(error_message, error_message_size, "path too long for directory scan");
        return -1;
    }

    if (snprintf(search, sizeof(search), "%s\\*", path) >= (int) sizeof(search)) {
        beast2_fs_set_error(error_message, error_message_size, "path too long for directory scan");
        return -1;
    }

    h_find = FindFirstFileA(search, &find_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            result->directories_seen++;
            return 0;
        }
        snprintf(error_message, error_message_size, "failed to open directory: %s", path);
        return -1;
    }

    result->directories_seen++;

    do {
        char child_path[BEAST2_MAX_PATH_LENGTH];
        struct stat child_stat;

        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }

        if (beast2_fs_join_path(child_path, sizeof(child_path), path, find_data.cFileName) != 0) {
            beast2_fs_set_error(error_message, error_message_size, "encountered path longer than supported limit");
            FindClose(h_find);
            return -1;
        }

        if (beast2_fs_path_exists(child_path, &child_stat) != 0) {
            snprintf(error_message, error_message_size, "failed to stat path: %s", child_path);
            FindClose(h_find);
            return -1;
        }

        if (S_ISDIR(child_stat.st_mode)) {
            if (beast2_fs_scan_tree(child_path, result, error_message, error_message_size) != 0) {
                FindClose(h_find);
                return -1;
            }
        } else {
            result->files_seen++;
        }
    } while (FindNextFileA(h_find, &find_data) != 0);

    FindClose(h_find);
    return 0;
}
#else
static int beast2_fs_scan_tree(
    const char *path,
    beast2_scan_result *result,
    char *error_message,
    size_t error_message_size
) {
    DIR *directory = NULL;
    struct dirent *entry = NULL;

    directory = opendir(path);

    if (directory == NULL) {
        snprintf(error_message, error_message_size, "failed to open directory: %s", path);
        return -1;
    }

    result->directories_seen++;

    while ((entry = readdir(directory)) != NULL) {
        char child_path[BEAST2_MAX_PATH_LENGTH];
        struct stat child_stat;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (beast2_fs_join_path(child_path, sizeof(child_path), path, entry->d_name) != 0) {
            beast2_fs_set_error(error_message, error_message_size, "encountered path longer than supported limit");
            closedir(directory);
            return -1;
        }

        if (beast2_fs_path_exists(child_path, &child_stat) != 0) {
            snprintf(error_message, error_message_size, "failed to stat path: %s", child_path);
            closedir(directory);
            return -1;
        }

        if (S_ISDIR(child_stat.st_mode)) {
            if (beast2_fs_scan_tree(child_path, result, error_message, error_message_size) != 0) {
                closedir(directory);
                return -1;
            }
        } else {
            result->files_seen++;
        }
    }

    closedir(directory);
    return 0;
}
#endif

int beast2_fs_is_absolute(const char *path) {
    if (path == NULL || *path == '\0') {
        return 0;
    }

    if (path[0] == '/' || path[0] == '\\') {
        return 1;
    }

    if (strlen(path) > 2 && ((path[1] == ':') && (path[2] == '\\' || path[2] == '/'))) {
        return 1;
    }

    return 0;
}

int beast2_fs_join_path(
    char *destination,
    size_t destination_size,
    const char *left,
    const char *right
) {
    size_t left_length = 0;

    if (destination == NULL || destination_size == 0 || right == NULL) {
        return -1;
    }

    if (left == NULL || *left == '\0' || beast2_fs_is_absolute(right)) {
        if (strlen(right) >= destination_size) {
            return -1;
        }

        snprintf(destination, destination_size, "%s", right);
        return 0;
    }

    left_length = strlen(left);

    if (left_length == 0) {
        if (strlen(right) >= destination_size) {
            return -1;
        }

        snprintf(destination, destination_size, "%s", right);
        return 0;
    }

    if (left[left_length - 1] == '/' || left[left_length - 1] == '\\') {
        if (snprintf(destination, destination_size, "%s%s", left, right) >= (int) destination_size) {
            return -1;
        }
    } else {
        if (snprintf(destination, destination_size, "%s/%s", left, right) >= (int) destination_size) {
            return -1;
        }
    }

    return 0;
}

int beast2_fs_parent_directory(
    const char *path,
    char *destination,
    size_t destination_size
) {
    const char *last_forward = NULL;
    const char *last_backward = NULL;
    const char *separator = NULL;
    size_t length = 0;

    if (path == NULL || destination == NULL || destination_size == 0) {
        return -1;
    }

    last_forward = strrchr(path, '/');
    last_backward = strrchr(path, '\\');
    separator = last_forward;

    if (last_backward != NULL && (separator == NULL || last_backward > separator)) {
        separator = last_backward;
    }

    if (separator == NULL) {
        if (destination_size < 2) {
            return -1;
        }

        snprintf(destination, destination_size, "%s", ".");
        return 0;
    }

    length = (size_t) (separator - path);

    if (length == 0) {
        if (destination_size < 2) {
            return -1;
        }

        snprintf(destination, destination_size, "%s", "/");
        return 0;
    }

    if (length >= destination_size) {
        return -1;
    }

    memcpy(destination, path, length);
    destination[length] = '\0';

    return 0;
}

int beast2_fs_mkdirs(
    const char *path,
    char *error_message,
    size_t error_message_size
) {
    char buffer[BEAST2_MAX_PATH_LENGTH];
    size_t index = 0;
    size_t start_index = 1;
    struct stat path_stat;

    if (path == NULL || *path == '\0') {
        beast2_fs_set_error(error_message, error_message_size, "cannot create an empty path");
        return -1;
    }

    if (strlen(path) >= sizeof(buffer)) {
        beast2_fs_set_error(error_message, error_message_size, "path exceeds supported length");
        return -1;
    }

    snprintf(buffer, sizeof(buffer), "%s", path);

    if (beast2_fs_path_exists(buffer, &path_stat) == 0) {
        if (!S_ISDIR(path_stat.st_mode)) {
            snprintf(error_message, error_message_size, "path exists but is not a directory: %s", path);
            return -1;
        }

        return 0;
    }

    if (strlen(buffer) > 2 && buffer[1] == ':') {
        start_index = 3;
    }

    for (index = start_index; buffer[index] != '\0'; index++) {
        struct stat partial_stat;

        if (buffer[index] != '/' && buffer[index] != '\\') {
            continue;
        }

        buffer[index] = '\0';

        if (*buffer != '\0' && beast2_fs_path_exists(buffer, &partial_stat) != 0) {
            if (beast2_fs_make_directory(buffer) != 0 && errno != EEXIST) {
                snprintf(error_message, error_message_size, "failed to create directory: %s", buffer);
                return -1;
            }
        }

        buffer[index] = '/';
    }

    if (beast2_fs_make_directory(buffer) != 0 && errno != EEXIST) {
        snprintf(error_message, error_message_size, "failed to create directory: %s", buffer);
        return -1;
    }

    return 0;
}

int beast2_fs_ensure_layout(
    const char *workspace_root,
    char *error_message,
    size_t error_message_size
) {
    static const char *layout_paths[] = {
        "models",
        "models/diffusion",
        "models/video",
        "models/llm",
        "generators",
        "generators/image",
        "generators/video",
        "generators/llm",
        "latents",
        "latents/motion",
        "latents/styles",
        "latents/seeds",
        "outputs",
        "outputs/images",
        "outputs/videos",
        "thumbs",
        "db",
        "db/performance_logs",
        "cache"
    };
    size_t index = 0;

    if (beast2_fs_mkdirs(workspace_root, error_message, error_message_size) != 0) {
        return -1;
    }

    for (index = 0; index < sizeof(layout_paths) / sizeof(layout_paths[0]); index++) {
        char full_path[BEAST2_MAX_PATH_LENGTH];

        if (beast2_fs_join_path(full_path, sizeof(full_path), workspace_root, layout_paths[index]) != 0) {
            beast2_fs_set_error(error_message, error_message_size, "failed to build directory layout path");
            return -1;
        }

        if (beast2_fs_mkdirs(full_path, error_message, error_message_size) != 0) {
            return -1;
        }
    }

    return 0;
}

int beast2_fs_scan_directories(
    const char *workspace_root,
    const beast2_config *config,
    beast2_logger *logger,
    beast2_scan_result *result,
    char *error_message,
    size_t error_message_size
) {
    size_t index = 0;

    memset(result, 0, sizeof(*result));

    for (index = 0; index < config->scan_directory_count; index++) {
        char full_path[BEAST2_MAX_PATH_LENGTH];
        struct stat path_stat;

        if (beast2_fs_join_path(full_path, sizeof(full_path), workspace_root, config->scan_directories[index]) != 0) {
            beast2_fs_set_error(error_message, error_message_size, "failed to build scan path");
            return -1;
        }

        result->roots_scanned++;

        if (beast2_fs_path_exists(full_path, &path_stat) != 0) {
            result->missing_roots++;
            beast2_logger_log(logger, BEAST2_LOG_LEVEL_WARN, "scan root missing: %s", full_path);
            continue;
        }

        if (!S_ISDIR(path_stat.st_mode)) {
            beast2_logger_log(logger, BEAST2_LOG_LEVEL_WARN, "scan root is not a directory: %s", full_path);
            continue;
        }

        beast2_logger_log(logger, BEAST2_LOG_LEVEL_INFO, "scanning %s", full_path);

        if (beast2_fs_scan_tree(full_path, result, error_message, error_message_size) != 0) {
            return -1;
        }
    }

    return 0;
}

#if defined(_WIN32)
int beast2_fs_list_regular_files(
    const char *directory_path,
    char (*out_basenames)[BEAST2_MAX_PATH_LENGTH],
    size_t max_entries,
    size_t *out_count,
    char *error_message,
    size_t error_message_size
) {
    char search[BEAST2_MAX_PATH_LENGTH];
    HANDLE h_find = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA find_data;
    size_t n = 0;

    *out_count = 0;
    if (max_entries == 0) {
        return 0;
    }

    if (strlen(directory_path) + 3 >= sizeof(search)) {
        snprintf(error_message, error_message_size, "path too long: %s", directory_path);
        return -1;
    }

    if (snprintf(search, sizeof(search), "%s\\*", directory_path) >= (int) sizeof(search)) {
        snprintf(error_message, error_message_size, "path too long: %s", directory_path);
        return -1;
    }

    h_find = FindFirstFileA(search, &find_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            return 0;
        }
        snprintf(error_message, error_message_size, "failed to list directory: %s", directory_path);
        return -1;
    }

    do {
        char child_path[BEAST2_MAX_PATH_LENGTH];
        struct stat child_stat;

        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        if (beast2_fs_join_path(child_path, sizeof(child_path), directory_path, find_data.cFileName) != 0) {
            continue;
        }

        if (beast2_fs_path_exists(child_path, &child_stat) != 0) {
            continue;
        }

        if (!S_ISREG(child_stat.st_mode)) {
            continue;
        }

        if (n >= max_entries) {
            break;
        }

        snprintf(out_basenames[n], BEAST2_MAX_PATH_LENGTH, "%s", find_data.cFileName);
        n++;
    } while (FindNextFileA(h_find, &find_data) != 0);

    FindClose(h_find);
    *out_count = n;
    return 0;
}
#else
int beast2_fs_list_regular_files(
    const char *directory_path,
    char (*out_basenames)[BEAST2_MAX_PATH_LENGTH],
    size_t max_entries,
    size_t *out_count,
    char *error_message,
    size_t error_message_size
) {
    DIR *directory = NULL;
    struct dirent *entry = NULL;
    size_t n = 0;

    *out_count = 0;
    if (max_entries == 0) {
        return 0;
    }

    directory = opendir(directory_path);
    if (directory == NULL) {
        snprintf(error_message, error_message_size, "failed to open directory: %s", directory_path);
        return -1;
    }

    while ((entry = readdir(directory)) != NULL && n < max_entries) {
        char child_path[BEAST2_MAX_PATH_LENGTH];
        struct stat child_stat;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (beast2_fs_join_path(child_path, sizeof(child_path), directory_path, entry->d_name) != 0) {
            continue;
        }

        if (beast2_fs_path_exists(child_path, &child_stat) != 0) {
            continue;
        }

        if (!S_ISREG(child_stat.st_mode)) {
            continue;
        }

        snprintf(out_basenames[n], BEAST2_MAX_PATH_LENGTH, "%s", entry->d_name);
        n++;
    }

    closedir(directory);
    *out_count = n;
    return 0;
}
#endif
