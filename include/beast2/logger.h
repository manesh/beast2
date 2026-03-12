#ifndef BEAST2_LOGGER_H
#define BEAST2_LOGGER_H

#include <stddef.h>
#include <stdio.h>

typedef enum beast2_log_level {
    BEAST2_LOG_LEVEL_DEBUG = 0,
    BEAST2_LOG_LEVEL_INFO = 1,
    BEAST2_LOG_LEVEL_WARN = 2,
    BEAST2_LOG_LEVEL_ERROR = 3
} beast2_log_level;

typedef struct beast2_logger {
    FILE *file;
    int stderr_enabled;
} beast2_logger;

int beast2_logger_init(
    beast2_logger *logger,
    const char *log_path,
    int stderr_enabled,
    char *error_message,
    size_t error_message_size
);

void beast2_logger_close(beast2_logger *logger);

void beast2_logger_log(
    beast2_logger *logger,
    beast2_log_level level,
    const char *fmt,
    ...
);

#endif
