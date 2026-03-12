#include "beast2/llm_integration.h"

#include <ctype.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "beast2/filesystem.h"

static void beast2_llm_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static uint64_t beast2_llm_hash_append(uint64_t hash, const char *value) {
    const unsigned char *cursor = (const unsigned char *) value;

    while (*cursor != '\0') {
        hash ^= (uint64_t) *cursor;
        hash *= 1099511628211ULL;
        cursor++;
    }

    return hash;
}

static void beast2_llm_hash_hex(char *destination, size_t destination_size, const char **values) {
    uint64_t hash = 1469598103934665603ULL;
    size_t index = 0;

    for (index = 0; values[index] != NULL; index++) {
        hash = beast2_llm_hash_append(hash, values[index]);
        hash = beast2_llm_hash_append(hash, "\n");
    }

    snprintf(destination, destination_size, "%016llx", (unsigned long long) hash);
}

static char *beast2_llm_trim(char *value) {
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

static int beast2_llm_write_text_file(
    const char *path,
    const char *contents,
    char *error_message,
    size_t error_message_size
) {
    char parent_path[BEAST2_MAX_PATH_LENGTH];
    FILE *file = NULL;

    if (beast2_fs_parent_directory(path, parent_path, sizeof(parent_path)) != 0) {
        beast2_llm_set_error(error_message, error_message_size, "failed to derive llm artifact directory");
        return -1;
    }

    if (beast2_fs_mkdirs(parent_path, error_message, error_message_size) != 0) {
        return -1;
    }

    file = fopen(path, "w");
    if (file == NULL) {
        snprintf(error_message, error_message_size, "failed to open llm artifact: %s", path);
        return -1;
    }

    if (fputs(contents, file) == EOF) {
        fclose(file);
        snprintf(error_message, error_message_size, "failed to write llm artifact: %s", path);
        return -1;
    }

    if (fclose(file) != 0) {
        snprintf(error_message, error_message_size, "failed to close llm artifact: %s", path);
        return -1;
    }

    return 0;
}

static int beast2_llm_read_text_file(
    const char *path,
    char *buffer,
    size_t buffer_size,
    char *error_message,
    size_t error_message_size
) {
    FILE *file = NULL;
    size_t bytes_read = 0;

    file = fopen(path, "r");
    if (file == NULL) {
        snprintf(error_message, error_message_size, "failed to open source generator: %s", path);
        return -1;
    }

    bytes_read = fread(buffer, 1, buffer_size - 1, file);
    buffer[bytes_read] = '\0';

    if (ferror(file)) {
        fclose(file);
        beast2_llm_set_error(error_message, error_message_size, "failed to read source generator");
        return -1;
    }

    fclose(file);
    return 0;
}

static int beast2_llm_serialize_query_results(
    sqlite3 *db,
    const char *sql,
    char *buffer,
    size_t buffer_size,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    size_t length = 0;
    int column_count = 0;
    int row_count = 0;

    buffer[0] = '\0';

    if (sql == NULL || *sql == '\0') {
        return 0;
    }

    {
        char check_buffer[8192];
        char *trimmed = NULL;

        if (strlen(sql) >= sizeof(check_buffer)) {
            beast2_llm_set_error(error_message, error_message_size, "llm query is too long");
            return -1;
        }

        snprintf(check_buffer, sizeof(check_buffer), "%s", sql);
        trimmed = beast2_llm_trim(check_buffer);

        if (strncmp(trimmed, "SELECT", 6) != 0 && strncmp(trimmed, "select", 6) != 0) {
            beast2_llm_set_error(error_message, error_message_size, "only SELECT queries are allowed for llm database query tasks");
            return -1;
        }
    }

    if (sqlite3_prepare_v2(db, sql, -1, &statement, NULL) != SQLITE_OK) {
        snprintf(error_message, error_message_size, "sqlite prepare error: %s", sqlite3_errmsg(db));
        return -1;
    }

    column_count = sqlite3_column_count(statement);
    length += (size_t) snprintf(buffer + length, buffer_size - length, "query_results:\n");

    while (sqlite3_step(statement) == SQLITE_ROW && row_count < 8) {
        int column_index = 0;

        length += (size_t) snprintf(buffer + length, buffer_size - length, "row_%d:", row_count + 1);

        for (column_index = 0; column_index < column_count; column_index++) {
            const char *column_name = sqlite3_column_name(statement, column_index);
            const char *value = (const char *) sqlite3_column_text(statement, column_index);

            length += (size_t) snprintf(
                buffer + length,
                buffer_size - length,
                " %s=%s",
                column_name != NULL ? column_name : "column",
                value != NULL ? value : "NULL"
            );

            if (length >= buffer_size) {
                sqlite3_finalize(statement);
                beast2_llm_set_error(error_message, error_message_size, "query results exceeded supported length");
                return -1;
            }
        }

        length += (size_t) snprintf(buffer + length, buffer_size - length, "\n");
        row_count++;
    }

    sqlite3_finalize(statement);
    return 0;
}

static void beast2_llm_slugify_token(char *destination, size_t destination_size, const char *source) {
    size_t input_index = 0;
    size_t output_index = 0;
    int previous_dash = 0;

    if (source == NULL || *source == '\0') {
        destination[0] = '\0';
        return;
    }

    for (input_index = 0; source[input_index] != '\0' && output_index + 1 < destination_size; input_index++) {
        unsigned char character = (unsigned char) source[input_index];

        if (isalnum(character)) {
            destination[output_index++] = (char) tolower(character);
            previous_dash = 0;
        } else if (!previous_dash && output_index > 0) {
            destination[output_index++] = '-';
            previous_dash = 1;
        }
    }

    while (output_index > 0 && destination[output_index - 1] == '-') {
        output_index--;
    }

    destination[output_index] = '\0';
}

static void beast2_llm_generate_tags(
    const char *prompt,
    const char *instruction,
    size_t max_tags,
    char *csv,
    size_t csv_size,
    char *text,
    size_t text_size
) {
    char buffer[8192];
    char *token = NULL;
    char *cursor = NULL;
    char tags[8][64];
    size_t tag_count = 0;
    size_t index = 0;
    size_t text_length = 0;

    memset(tags, 0, sizeof(tags));
    csv[0] = '\0';
    text[0] = '\0';

    snprintf(buffer, sizeof(buffer), "%s %s", prompt != NULL ? prompt : "", instruction != NULL ? instruction : "");
    cursor = buffer;

    while ((token = strtok(cursor, " \t\r\n,.:;!?()[]{}<>/\\\"'")) != NULL && tag_count < max_tags) {
        char slug[64];
        int duplicate = 0;

        cursor = NULL;
        beast2_llm_slugify_token(slug, sizeof(slug), token);

        if (strlen(slug) < 4) {
            continue;
        }

        for (index = 0; index < tag_count; index++) {
            if (strcmp(tags[index], slug) == 0) {
                duplicate = 1;
                break;
            }
        }

        if (duplicate) {
            continue;
        }

        snprintf(tags[tag_count], sizeof(tags[tag_count]), "%s", slug);
        tag_count++;
    }

    for (index = 0; index < tag_count; index++) {
        if (index > 0) {
            strncat(csv, ", ", csv_size - strlen(csv) - 1);
            strncat(text, "\n", text_size - strlen(text) - 1);
        }

        strncat(csv, tags[index], csv_size - strlen(csv) - 1);
        text_length += (size_t) snprintf(text + text_length, text_size - text_length, "tag_%zu: %s", index + 1, tags[index]);
    }
}

beast2_llm_task_kind beast2_llm_task_kind_from_string(const char *value) {
    if (value == NULL || *value == '\0') {
        return BEAST2_LLM_TASK_NONE;
    }

    if (strcmp(value, "prompt_mutation") == 0) {
        return BEAST2_LLM_TASK_PROMPT_MUTATION;
    }

    if (strcmp(value, "generator_edit") == 0) {
        return BEAST2_LLM_TASK_GENERATOR_EDIT;
    }

    if (strcmp(value, "tag_generation") == 0) {
        return BEAST2_LLM_TASK_TAG_GENERATION;
    }

    if (strcmp(value, "database_query") == 0) {
        return BEAST2_LLM_TASK_DATABASE_QUERY;
    }

    return BEAST2_LLM_TASK_NONE;
}

int beast2_llm_prepare_prompt(
    beast2_media_library_context *media_library,
    const beast2_llm_workflow *workflow,
    const char *base_prompt,
    char *prepared_prompt,
    size_t prepared_prompt_size,
    char *error_message,
    size_t error_message_size
) {
    char source_buffer[16384];
    char query_buffer[16384];

    source_buffer[0] = '\0';
    query_buffer[0] = '\0';
    snprintf(prepared_prompt, prepared_prompt_size, "%s", base_prompt != NULL ? base_prompt : "");

    if (workflow == NULL || workflow->kind == BEAST2_LLM_TASK_NONE) {
        return 0;
    }

    if (
        workflow->kind == BEAST2_LLM_TASK_PROMPT_MUTATION ||
        workflow->kind == BEAST2_LLM_TASK_TAG_GENERATION
    ) {
        snprintf(prepared_prompt, prepared_prompt_size, "%s", base_prompt != NULL ? base_prompt : "");
        return 0;
    }

    if (workflow->kind == BEAST2_LLM_TASK_GENERATOR_EDIT) {
        if (workflow->source_generator_path == NULL || *workflow->source_generator_path == '\0') {
            beast2_llm_set_error(error_message, error_message_size, "generator_edit requires b2_llm_source_generator");
            return -1;
        }

        if (
            beast2_llm_read_text_file(
                workflow->source_generator_path,
                source_buffer,
                sizeof(source_buffer),
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }

        snprintf(
            prepared_prompt,
            prepared_prompt_size,
            "TASK: generator_edit\ninstruction: %s\nsource_generator:\n%s\nprompt:\n%s\n",
            workflow->instruction != NULL ? workflow->instruction : "",
            source_buffer,
            base_prompt != NULL ? base_prompt : ""
        );
        return 0;
    }

    if (workflow->kind == BEAST2_LLM_TASK_DATABASE_QUERY) {
        if (media_library == NULL || media_library->db == NULL) {
            beast2_llm_set_error(error_message, error_message_size, "database_query requires an initialized media library database");
            return -1;
        }

        if (
            beast2_llm_serialize_query_results(
                media_library->db,
                workflow->query_sql,
                query_buffer,
                sizeof(query_buffer),
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }

        snprintf(
            prepared_prompt,
            prepared_prompt_size,
            "TASK: database_query\ninstruction: %s\n%s\nprompt:\n%s\n",
            workflow->instruction != NULL ? workflow->instruction : "",
            query_buffer,
            base_prompt != NULL ? base_prompt : ""
        );
        return 0;
    }

    if (workflow->instruction != NULL && *workflow->instruction != '\0') {
        snprintf(
            prepared_prompt,
            prepared_prompt_size,
            "TASK: %s\ninstruction: %s\nprompt:\n%s\n",
            workflow->task_name != NULL ? workflow->task_name : "llm",
            workflow->instruction,
            base_prompt != NULL ? base_prompt : ""
        );
    }

    return 0;
}

int beast2_llm_finalize_output(
    const beast2_config *config,
    const beast2_llm_workflow *workflow,
    const char *job_id,
    const char *prepared_prompt,
    const char *runtime_output,
    beast2_llm_task_result *result,
    char *error_message,
    size_t error_message_size
) {
    char hash_buffer[17];
    const char *hash_values[5];

    memset(result, 0, sizeof(*result));
    snprintf(result->prepared_prompt, sizeof(result->prepared_prompt), "%s", prepared_prompt != NULL ? prepared_prompt : "");
    hash_values[0] = workflow != NULL ? workflow->task_name : "llm";
    hash_values[1] = prepared_prompt != NULL ? prepared_prompt : "";
    hash_values[2] = runtime_output != NULL ? runtime_output : "";
    hash_values[3] = job_id != NULL ? job_id : "";
    hash_values[4] = NULL;
    beast2_llm_hash_hex(hash_buffer, sizeof(hash_buffer), hash_values);

    if (workflow == NULL || workflow->kind == BEAST2_LLM_TASK_NONE) {
        snprintf(result->final_output, sizeof(result->final_output), "%s", runtime_output != NULL ? runtime_output : "");
        return 0;
    }

    switch (workflow->kind) {
        case BEAST2_LLM_TASK_PROMPT_MUTATION: {
            size_t attempts = workflow->attempts > 0 ? workflow->attempts : 4;
            size_t index = 0;
            size_t length = 0;

            length += (size_t) snprintf(
                result->final_output + length,
                sizeof(result->final_output) - length,
                "BEAST2_PHASE10_PROMPT_MUTATION\nbase_prompt: %s\ninstruction: %s\n",
                prepared_prompt != NULL ? prepared_prompt : "",
                workflow->instruction != NULL ? workflow->instruction : ""
            );

            for (index = 0; index < attempts && length + 128 < sizeof(result->final_output); index++) {
                length += (size_t) snprintf(
                    result->final_output + length,
                    sizeof(result->final_output) - length,
                    "variant_%zu: %s :: mutation_%s_%zu\n",
                    index + 1,
                    prepared_prompt != NULL ? prepared_prompt : "",
                    hash_buffer,
                    index + 1
                );
            }
            return 0;
        }

        case BEAST2_LLM_TASK_GENERATOR_EDIT: {
            char source_buffer[16384];
            char candidate_contents[32768];

            if (workflow->source_generator_path == NULL || *workflow->source_generator_path == '\0') {
                beast2_llm_set_error(error_message, error_message_size, "generator_edit requires b2_llm_source_generator");
                return -1;
            }

            if (
                beast2_llm_read_text_file(
                    workflow->source_generator_path,
                    source_buffer,
                    sizeof(source_buffer),
                    error_message,
                    error_message_size
                ) != 0
            ) {
                return -1;
            }

            snprintf(
                result->generated_generator_relative_path,
                sizeof(result->generated_generator_relative_path),
                "generators/llm/generated/%s.b2",
                job_id
            );
            if (
                beast2_fs_join_path(
                    result->generated_generator_path,
                    sizeof(result->generated_generator_path),
                    config->workspace_root,
                    result->generated_generator_relative_path
                ) != 0
            ) {
                beast2_llm_set_error(error_message, error_message_size, "failed to build generated generator path");
                return -1;
            }

            snprintf(
                candidate_contents,
                sizeof(candidate_contents),
                "# beast2 llm-generated generator candidate\n"
                "# instruction: %s\n"
                "# response_hash: %s\n"
                "%s\n"
                "# llm suggestion: %s\n",
                workflow->instruction != NULL ? workflow->instruction : "",
                hash_buffer,
                source_buffer,
                runtime_output != NULL ? runtime_output : ""
            );

            if (
                beast2_llm_write_text_file(
                    result->generated_generator_path,
                    candidate_contents,
                    error_message,
                    error_message_size
                ) != 0
            ) {
                return -1;
            }

            result->has_generated_generator = 1;
            snprintf(
                result->final_output,
                sizeof(result->final_output),
                "BEAST2_PHASE10_GENERATOR_EDIT\nsource: %s\ngenerated_candidate: %s\nresponse: %s\n",
                workflow->source_generator_path,
                result->generated_generator_relative_path,
                runtime_output != NULL ? runtime_output : ""
            );
            return 0;
        }

        case BEAST2_LLM_TASK_TAG_GENERATION: {
            char tags_text[1024];

            memset(tags_text, 0, sizeof(tags_text));
            beast2_llm_generate_tags(
                prepared_prompt,
                workflow->instruction,
                workflow->attempts > 0 && workflow->attempts < 8 ? workflow->attempts : 5,
                result->generated_tags_csv,
                sizeof(result->generated_tags_csv),
                tags_text,
                sizeof(tags_text)
            );

            snprintf(
                result->final_output,
                sizeof(result->final_output),
                "BEAST2_PHASE10_TAG_GENERATION\nprompt: %s\ninstruction: %s\n%s\nresponse: %s\n",
                prepared_prompt != NULL ? prepared_prompt : "",
                workflow->instruction != NULL ? workflow->instruction : "",
                tags_text,
                runtime_output != NULL ? runtime_output : ""
            );
            return 0;
        }

        case BEAST2_LLM_TASK_DATABASE_QUERY:
            snprintf(
                result->final_output,
                sizeof(result->final_output),
                "BEAST2_PHASE10_DATABASE_QUERY\nprompt: %s\nquery: %s\nresponse: %s\n",
                prepared_prompt != NULL ? prepared_prompt : "",
                workflow->query_sql != NULL ? workflow->query_sql : "",
                runtime_output != NULL ? runtime_output : ""
            );
            return 0;

        case BEAST2_LLM_TASK_NONE:
        default:
            snprintf(result->final_output, sizeof(result->final_output), "%s", runtime_output != NULL ? runtime_output : "");
            return 0;
    }
}
