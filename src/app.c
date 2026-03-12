#include "beast2/app.h"

#include <stdio.h>
#include <string.h>

#include "beast2/config.h"
#include "beast2/filesystem.h"
#include "beast2/logger.h"

int beast2_run(const char *config_path) {
    beast2_config config;
    beast2_logger logger;
    beast2_scan_result scan_result;
    char error_message[512];
    char log_path[BEAST2_MAX_PATH_LENGTH];
    char log_directory[BEAST2_MAX_PATH_LENGTH];

    memset(&logger, 0, sizeof(logger));
    memset(error_message, 0, sizeof(error_message));

    if (beast2_config_load(&config, config_path, error_message, sizeof(error_message)) != 0) {
        fprintf(stderr, "beast2: failed to load configuration: %s\n", error_message);
        return 1;
    }

    if (config.create_missing_directories) {
        if (
            beast2_fs_ensure_layout(
                config.workspace_root,
                error_message,
                sizeof(error_message)
            ) != 0
        ) {
            fprintf(stderr, "beast2: failed to prepare workspace: %s\n", error_message);
            return 1;
        }
    }

    if (beast2_fs_is_absolute(config.log_file)) {
        snprintf(log_path, sizeof(log_path), "%s", config.log_file);
    } else if (
        beast2_fs_join_path(
            log_path,
            sizeof(log_path),
            config.workspace_root,
            config.log_file
        ) != 0
    ) {
        fprintf(stderr, "beast2: failed to build log path\n");
        return 1;
    }

    if (beast2_fs_parent_directory(log_path, log_directory, sizeof(log_directory)) != 0) {
        fprintf(stderr, "beast2: failed to derive log directory\n");
        return 1;
    }

    if (config.create_missing_directories) {
        if (
            beast2_fs_mkdirs(
                log_directory,
                error_message,
                sizeof(error_message)
            ) != 0
        ) {
            fprintf(stderr, "beast2: failed to prepare log directory: %s\n", error_message);
            return 1;
        }
    }

    if (
        beast2_logger_init(
            &logger,
            log_path,
            config.log_to_stderr,
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        fprintf(stderr, "beast2: %s\n", error_message);
        return 1;
    }

    beast2_logger_log(&logger, BEAST2_LOG_LEVEL_INFO, "Beast2 phase zero startup");
    beast2_logger_log(&logger, BEAST2_LOG_LEVEL_INFO, "loaded config from %s", config_path);
    beast2_logger_log(&logger, BEAST2_LOG_LEVEL_INFO, "workspace root: %s", config.workspace_root);
    beast2_logger_log(&logger, BEAST2_LOG_LEVEL_INFO, "log file: %s", log_path);
    beast2_logger_log(
        &logger,
        BEAST2_LOG_LEVEL_INFO,
        "create missing directories: %s",
        config.create_missing_directories ? "true" : "false"
    );

    if (
        beast2_fs_scan_directories(
            config.workspace_root,
            &config,
            &logger,
            &scan_result,
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        beast2_logger_log(&logger, BEAST2_LOG_LEVEL_ERROR, "directory scan failed: %s", error_message);
        beast2_logger_close(&logger);
        return 1;
    }

    beast2_logger_log(
        &logger,
        BEAST2_LOG_LEVEL_INFO,
        "scan complete: roots=%zu directories=%zu files=%zu missing_roots=%zu",
        scan_result.roots_scanned,
        scan_result.directories_seen,
        scan_result.files_seen,
        scan_result.missing_roots
    );

    printf("Beast2 phase zero boot complete.\n");
    printf("Config: %s\n", config_path);
    printf("Workspace: %s\n", config.workspace_root);
    printf("Log file: %s\n", log_path);
    printf(
        "Scan summary: roots=%zu directories=%zu files=%zu missing_roots=%zu\n",
        scan_result.roots_scanned,
        scan_result.directories_seen,
        scan_result.files_seen,
        scan_result.missing_roots
    );

    beast2_logger_close(&logger);
    return 0;
}
