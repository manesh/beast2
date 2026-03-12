#include "beast2/config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void beast2_config_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static int beast2_config_copy_string(
    char *destination,
    size_t destination_size,
    const char *value,
    char *error_message,
    size_t error_message_size
) {
    if (strlen(value) >= destination_size) {
        beast2_config_set_error(error_message, error_message_size, "configuration value is too long");
        return -1;
    }

    snprintf(destination, destination_size, "%s", value);
    return 0;
}

static char *beast2_config_trim(char *value) {
    char *end = NULL;

    while (*value != '\0' && isspace((unsigned char) *value)) {
        value++;
    }

    if (*value == '\0') {
        return value;
    }

    end = value + strlen(value) - 1;

    while (end > value && isspace((unsigned char) *end)) {
        *end = '\0';
        end--;
    }

    return value;
}

static int beast2_config_parse_bool(
    const char *value,
    int *parsed_value,
    char *error_message,
    size_t error_message_size
) {
    if (
        strcmp(value, "1") == 0 ||
        strcmp(value, "true") == 0 ||
        strcmp(value, "yes") == 0 ||
        strcmp(value, "on") == 0
    ) {
        *parsed_value = 1;
        return 0;
    }

    if (
        strcmp(value, "0") == 0 ||
        strcmp(value, "false") == 0 ||
        strcmp(value, "no") == 0 ||
        strcmp(value, "off") == 0
    ) {
        *parsed_value = 0;
        return 0;
    }

    beast2_config_set_error(
        error_message,
        error_message_size,
        "boolean values must be one of: true, false, yes, no, on, off, 1, 0"
    );
    return -1;
}

static int beast2_config_set_default_scan_directories(beast2_config *config) {
    static const char *defaults[] = {
        "models",
        "generators",
        "latents",
        "outputs",
        "thumbs",
        "db",
        "cache"
    };
    size_t index = 0;

    config->scan_directory_count = sizeof(defaults) / sizeof(defaults[0]);

    for (index = 0; index < config->scan_directory_count; index++) {
        snprintf(
            config->scan_directories[index],
            sizeof(config->scan_directories[index]),
            "%s",
            defaults[index]
        );
    }

    return 0;
}

static int beast2_config_parse_scan_directories(
    beast2_config *config,
    const char *value,
    char *error_message,
    size_t error_message_size
) {
    char buffer[8192];
    char *token = NULL;
    char *cursor = NULL;

    if (strlen(value) >= sizeof(buffer)) {
        beast2_config_set_error(error_message, error_message_size, "scan_directories value is too long");
        return -1;
    }

    snprintf(buffer, sizeof(buffer), "%s", value);
    config->scan_directory_count = 0;
    cursor = buffer;

    while ((token = strtok(cursor, ",")) != NULL) {
        char *trimmed = beast2_config_trim(token);

        cursor = NULL;

        if (*trimmed == '\0') {
            continue;
        }

        if (config->scan_directory_count >= BEAST2_MAX_SCAN_DIRECTORIES) {
            beast2_config_set_error(error_message, error_message_size, "too many scan directories configured");
            return -1;
        }

        if (
            beast2_config_copy_string(
                config->scan_directories[config->scan_directory_count],
                sizeof(config->scan_directories[config->scan_directory_count]),
                trimmed,
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }

        config->scan_directory_count++;
    }

    if (config->scan_directory_count == 0) {
        beast2_config_set_error(error_message, error_message_size, "scan_directories must contain at least one path");
        return -1;
    }

    return 0;
}

void beast2_config_set_defaults(beast2_config *config) {
    memset(config, 0, sizeof(*config));
    snprintf(config->workspace_root, sizeof(config->workspace_root), "%s", "./runtime/beast2");
    snprintf(config->log_file, sizeof(config->log_file), "%s", "db/performance_logs/beast2.log");
    config->log_to_stderr = 1;
    config->create_missing_directories = 1;
    beast2_config_set_default_scan_directories(config);
}

int beast2_config_load(
    beast2_config *config,
    const char *path,
    char *error_message,
    size_t error_message_size
) {
    FILE *file = NULL;
    char line[8192];
    size_t line_number = 0;

    beast2_config_set_defaults(config);
    file = fopen(path, "r");

    if (file == NULL) {
        snprintf(error_message, error_message_size, "failed to open config file: %s", path);
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *key = NULL;
        char *value = NULL;
        char *separator = NULL;
        char *trimmed = NULL;

        line_number++;
        trimmed = beast2_config_trim(line);

        if (*trimmed == '\0' || *trimmed == '#' || *trimmed == ';') {
            continue;
        }

        separator = strchr(trimmed, '=');

        if (separator == NULL) {
            snprintf(error_message, error_message_size, "config parse error on line %zu", line_number);
            fclose(file);
            return -1;
        }

        *separator = '\0';
        key = beast2_config_trim(trimmed);
        value = beast2_config_trim(separator + 1);

        if (strcmp(key, "workspace_root") == 0) {
            if (
                beast2_config_copy_string(
                    config->workspace_root,
                    sizeof(config->workspace_root),
                    value,
                    error_message,
                    error_message_size
                ) != 0
            ) {
                fclose(file);
                return -1;
            }
        } else if (strcmp(key, "log_file") == 0) {
            if (
                beast2_config_copy_string(
                    config->log_file,
                    sizeof(config->log_file),
                    value,
                    error_message,
                    error_message_size
                ) != 0
            ) {
                fclose(file);
                return -1;
            }
        } else if (strcmp(key, "log_to_stderr") == 0) {
            if (
                beast2_config_parse_bool(
                    value,
                    &config->log_to_stderr,
                    error_message,
                    error_message_size
                ) != 0
            ) {
                fclose(file);
                return -1;
            }
        } else if (strcmp(key, "create_missing_directories") == 0) {
            if (
                beast2_config_parse_bool(
                    value,
                    &config->create_missing_directories,
                    error_message,
                    error_message_size
                ) != 0
            ) {
                fclose(file);
                return -1;
            }
        } else if (strcmp(key, "scan_directories") == 0) {
            if (
                beast2_config_parse_scan_directories(
                    config,
                    value,
                    error_message,
                    error_message_size
                ) != 0
            ) {
                fclose(file);
                return -1;
            }
        } else {
            snprintf(error_message, error_message_size, "unknown config key '%s' on line %zu", key, line_number);
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}
