#include "beast2/logger.h"

#include <stdarg.h>
#include <string.h>
#include <time.h>

static const char *beast2_logger_level_name(beast2_log_level level) {
    switch (level) {
        case BEAST2_LOG_LEVEL_DEBUG:
            return "DEBUG";
        case BEAST2_LOG_LEVEL_INFO:
            return "INFO";
        case BEAST2_LOG_LEVEL_WARN:
            return "WARN";
        case BEAST2_LOG_LEVEL_ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

static void beast2_logger_timestamp(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm local_time;
    struct tm *resolved_time = NULL;

    memset(&local_time, 0, sizeof(local_time));
#if defined(_WIN32)
    localtime_s(&local_time, &now);
#else
    resolved_time = localtime(&now);

    if (resolved_time != NULL) {
        local_time = *resolved_time;
    }
#endif
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", &local_time);
}

int beast2_logger_init(
    beast2_logger *logger,
    const char *log_path,
    int stderr_enabled,
    char *error_message,
    size_t error_message_size
) {
    logger->file = fopen(log_path, "a");
    logger->stderr_enabled = stderr_enabled;

    if (logger->file == NULL) {
        snprintf(error_message, error_message_size, "failed to open log file: %s", log_path);
        return -1;
    }

    return 0;
}

void beast2_logger_close(beast2_logger *logger) {
    if (logger->file != NULL) {
        fclose(logger->file);
        logger->file = NULL;
    }
}

void beast2_logger_log(
    beast2_logger *logger,
    beast2_log_level level,
    const char *fmt,
    ...
) {
    char timestamp[32];
    char message[4096];
    va_list args;

    if (logger == NULL) {
        return;
    }

    beast2_logger_timestamp(timestamp, sizeof(timestamp));

    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    if (logger->file != NULL) {
        fprintf(logger->file, "[%s] %-5s %s\n", timestamp, beast2_logger_level_name(level), message);
        fflush(logger->file);
    }

    if (logger->stderr_enabled) {
        fprintf(stderr, "[%s] %-5s %s\n", timestamp, beast2_logger_level_name(level), message);
        fflush(stderr);
    }
}
