#include "beast2/executor.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "beast2/filesystem.h"
#include "beast2/parser.h"

typedef enum beast2_job_status {
    BEAST2_JOB_STATUS_PENDING = 0,
    BEAST2_JOB_STATUS_RUNNING = 1,
    BEAST2_JOB_STATUS_COMPLETED = 2,
    BEAST2_JOB_STATUS_FAILED = 3
} beast2_job_status;

typedef struct beast2_execution_job {
    size_t variant_index;
    char job_id[17];
    char prompt[8192];
    char prompt_hash[17];
    char output_relative_path[BEAST2_MAX_PATH_LENGTH];
    char output_path[BEAST2_MAX_PATH_LENGTH];
    char generator_artifact_relative_path[BEAST2_MAX_PATH_LENGTH];
    char generator_artifact_path[BEAST2_MAX_PATH_LENGTH];
    char state_report_relative_path[BEAST2_MAX_PATH_LENGTH];
    char state_report_path[BEAST2_MAX_PATH_LENGTH];
    beast2_job_status status;
} beast2_execution_job;

typedef struct beast2_execution_context {
    const beast2_config *config;
    beast2_logger *logger;
    const beast2_generator_document *document;
    const beast2_prompt_block *prompt_block;
    const beast2_metadata_section *workflow_section;
    const beast2_metadata_section *tags_section;
    const char *generator_name;
    const char *engine;
    const char *checkpoint;
    const char *seed;
    const char *steps;
    const char *resolution;
    const char *output_template;
    const char *artifact_template;
    const char *state_template;
    const char *output_ext;
    char checkpoint_hash[128];
    char tags_joined[1024];
} beast2_execution_context;

static void beast2_execution_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static uint64_t beast2_fnv1a_append(uint64_t hash, const char *value) {
    const unsigned char *cursor = (const unsigned char *) value;

    while (*cursor != '\0') {
        hash ^= (uint64_t) *cursor;
        hash *= 1099511628211ULL;
        cursor++;
    }

    return hash;
}

static void beast2_hash_hex_from_strings(char *destination, size_t destination_size, const char **values) {
    uint64_t hash = 1469598103934665603ULL;
    size_t index = 0;

    for (index = 0; values[index] != NULL; index++) {
        hash = beast2_fnv1a_append(hash, values[index]);
        hash = beast2_fnv1a_append(hash, "\n");
    }

    snprintf(destination, destination_size, "%016llx", (unsigned long long) hash);
}

static void beast2_sanitize_component(
    char *destination,
    size_t destination_size,
    const char *value,
    const char *fallback
) {
    size_t index = 0;
    size_t output_index = 0;

    if (value == NULL || *value == '\0') {
        snprintf(destination, destination_size, "%s", fallback);
        return;
    }

    for (index = 0; value[index] != '\0' && output_index + 1 < destination_size; index++) {
        unsigned char character = (unsigned char) value[index];

        if (isalnum(character) || character == '-' || character == '_' || character == '.') {
            destination[output_index] = (char) character;
        } else {
            destination[output_index] = '_';
        }

        output_index++;
    }

    if (output_index == 0) {
        snprintf(destination, destination_size, "%s", fallback);
        return;
    }

    destination[output_index] = '\0';
}

static void beast2_extract_checkpoint_hash(
    char *destination,
    size_t destination_size,
    const char *checkpoint
) {
    const char *separator = NULL;

    if (checkpoint == NULL || *checkpoint == '\0') {
        snprintf(destination, destination_size, "%s", "default");
        return;
    }

    separator = strrchr(checkpoint, '#');

    if (separator != NULL && separator[1] != '\0') {
        beast2_sanitize_component(destination, destination_size, separator + 1, "default");
        return;
    }

    beast2_sanitize_component(destination, destination_size, checkpoint, "default");
}

static int beast2_join_tags(
    const beast2_metadata_section *tags_section,
    char *destination,
    size_t destination_size,
    char *error_message,
    size_t error_message_size
) {
    size_t index = 0;
    size_t length = 0;

    destination[0] = '\0';

    if (tags_section == NULL) {
        return 0;
    }

    for (index = 0; index < tags_section->instruction_count; index++) {
        const beast2_instruction *instruction = &tags_section->instructions[index];
        size_t value_length = 0;

        if (strcmp(instruction->name, "b2_tag") != 0 || instruction->value[0] == '\0') {
            continue;
        }

        value_length = strlen(instruction->value);

        if (length > 0) {
            if (length + 2 >= destination_size) {
                beast2_execution_set_error(error_message, error_message_size, "tag list exceeds supported length");
                return -1;
            }

            destination[length++] = ',';
            destination[length++] = ' ';
            destination[length] = '\0';
        }

        if (length + value_length + 1 > destination_size) {
            beast2_execution_set_error(error_message, error_message_size, "tag list exceeds supported length");
            return -1;
        }

        memcpy(destination + length, instruction->value, value_length);
        length += value_length;
        destination[length] = '\0';
    }

    return 0;
}

static const char *beast2_template_variable_value(
    const beast2_execution_context *context,
    const beast2_execution_job *job,
    const char *name,
    char *variant_buffer,
    size_t variant_buffer_size
) {
    if (strcmp(name, "generator") == 0) {
        return context->generator_name;
    }

    if (strcmp(name, "variant") == 0) {
        snprintf(variant_buffer, variant_buffer_size, "%zu", job->variant_index + 1);
        return variant_buffer;
    }

    if (strcmp(name, "job_id") == 0) {
        return job->job_id;
    }

    if (strcmp(name, "engine") == 0) {
        return context->engine;
    }

    if (strcmp(name, "seed") == 0) {
        return context->seed;
    }

    if (strcmp(name, "checkpoint") == 0) {
        return context->checkpoint;
    }

    if (strcmp(name, "checkpoint_hash") == 0) {
        return context->checkpoint_hash;
    }

    if (strcmp(name, "ext") == 0) {
        return context->output_ext;
    }

    return NULL;
}

static int beast2_resolve_template(
    const char *template_value,
    const beast2_execution_context *context,
    const beast2_execution_job *job,
    char *destination,
    size_t destination_size,
    char *error_message,
    size_t error_message_size
) {
    size_t source_index = 0;
    size_t destination_index = 0;
    char variable_name[128];
    char variant_buffer[64];

    destination[0] = '\0';

    while (template_value[source_index] != '\0') {
        if (
            template_value[source_index] == '$' &&
            template_value[source_index + 1] == '{'
        ) {
            size_t variable_index = 0;
            const char *value = NULL;

            source_index += 2;

            while (
                template_value[source_index] != '\0' &&
                template_value[source_index] != '}' &&
                variable_index + 1 < sizeof(variable_name)
            ) {
                variable_name[variable_index++] = template_value[source_index++];
            }

            if (template_value[source_index] != '}') {
                beast2_execution_set_error(error_message, error_message_size, "unterminated variable placeholder");
                return -1;
            }

            variable_name[variable_index] = '\0';
            source_index++;

            value = beast2_template_variable_value(context, job, variable_name, variant_buffer, sizeof(variant_buffer));

            if (value == NULL) {
                snprintf(error_message, error_message_size, "unknown template variable '%s'", variable_name);
                return -1;
            }

            if (destination_index + strlen(value) + 1 > destination_size) {
                beast2_execution_set_error(error_message, error_message_size, "resolved template exceeds supported length");
                return -1;
            }

            memcpy(destination + destination_index, value, strlen(value));
            destination_index += strlen(value);
            destination[destination_index] = '\0';
            continue;
        }

        if (destination_index + 2 > destination_size) {
            beast2_execution_set_error(error_message, error_message_size, "resolved template exceeds supported length");
            return -1;
        }

        destination[destination_index++] = template_value[source_index++];
        destination[destination_index] = '\0';
    }

    return 0;
}

static int beast2_write_text_file(
    const char *path,
    const char *contents,
    char *error_message,
    size_t error_message_size
) {
    char parent_path[BEAST2_MAX_PATH_LENGTH];
    FILE *file = NULL;

    if (beast2_fs_parent_directory(path, parent_path, sizeof(parent_path)) != 0) {
        beast2_execution_set_error(error_message, error_message_size, "failed to derive output parent directory");
        return -1;
    }

    if (beast2_fs_mkdirs(parent_path, error_message, error_message_size) != 0) {
        return -1;
    }

    file = fopen(path, "w");

    if (file == NULL) {
        snprintf(error_message, error_message_size, "failed to open output file: %s", path);
        return -1;
    }

    if (fputs(contents, file) == EOF) {
        fclose(file);
        snprintf(error_message, error_message_size, "failed to write output file: %s", path);
        return -1;
    }

    if (fclose(file) != 0) {
        snprintf(error_message, error_message_size, "failed to close output file: %s", path);
        return -1;
    }

    return 0;
}

static int beast2_prepare_execution_context(
    beast2_execution_context *context,
    const beast2_config *config,
    beast2_logger *logger,
    const beast2_generator_document *document,
    char *error_message,
    size_t error_message_size
) {
    const beast2_prompt_block *prompt_block = NULL;

    memset(context, 0, sizeof(*context));
    prompt_block = beast2_generator_primary_prompt_block(document);

    if (prompt_block == NULL) {
        beast2_execution_set_error(error_message, error_message_size, "generator does not contain a prompt block");
        return -1;
    }

    context->config = config;
    context->logger = logger;
    context->document = document;
    context->prompt_block = prompt_block;
    context->workflow_section = beast2_generator_find_metadata_section(document, "b2_workflow");
    context->tags_section = beast2_generator_find_metadata_section(document, "b2_tags");
    context->generator_name = document->generator_name != NULL ? document->generator_name : "unnamed_generator";
    context->engine = "mock_model";
    context->checkpoint = "mock_model#default";
    context->seed = "0";
    context->steps = "1";
    context->resolution = "unknown";
    context->output_ext = "txt";

    if (context->workflow_section != NULL) {
        const char *value = NULL;

        value = beast2_metadata_first_value(context->workflow_section, "b2_engine");
        if (value != NULL && *value != '\0') {
            context->engine = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_checkpoint");
        if (value != NULL && *value != '\0') {
            context->checkpoint = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_seed");
        if (value != NULL && *value != '\0') {
            context->seed = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_steps");
        if (value != NULL && *value != '\0') {
            context->steps = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_resolution");
        if (value != NULL && *value != '\0') {
            context->resolution = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_output_template");
        if (value != NULL && *value != '\0') {
            context->output_template = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_output_generator_template");
        if (value != NULL && *value != '\0') {
            context->artifact_template = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_job_state_template");
        if (value != NULL && *value != '\0') {
            context->state_template = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_output_ext");
        if (value != NULL && *value != '\0') {
            context->output_ext = value;
        }
    }

    beast2_extract_checkpoint_hash(context->checkpoint_hash, sizeof(context->checkpoint_hash), context->checkpoint);

    if (
        beast2_join_tags(
            context->tags_section,
            context->tags_joined,
            sizeof(context->tags_joined),
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    return 0;
}

static void beast2_build_job_id(
    const beast2_execution_context *context,
    beast2_execution_job *job
) {
    char variant_buffer[64];
    const char *values[9];

    snprintf(variant_buffer, sizeof(variant_buffer), "%zu", job->variant_index + 1);
    values[0] = context->generator_name;
    values[1] = context->engine;
    values[2] = context->checkpoint;
    values[3] = context->seed;
    values[4] = context->resolution;
    values[5] = context->steps;
    values[6] = job->prompt;
    values[7] = variant_buffer;
    values[8] = NULL;

    beast2_hash_hex_from_strings(job->job_id, sizeof(job->job_id), values);
    values[0] = job->prompt;
    values[1] = variant_buffer;
    values[2] = NULL;
    beast2_hash_hex_from_strings(job->prompt_hash, sizeof(job->prompt_hash), values);
}

static int beast2_build_default_paths(
    const beast2_execution_context *context,
    beast2_execution_job *job,
    char *error_message,
    size_t error_message_size
) {
    char shard[3];

    shard[0] = job->job_id[0];
    shard[1] = job->job_id[1];
    shard[2] = '\0';

    if (
        snprintf(
            job->output_relative_path,
            sizeof(job->output_relative_path),
            "outputs/images/%s/%s.%s",
            shard,
            job->job_id,
            context->output_ext
        ) >= (int) sizeof(job->output_relative_path)
    ) {
        beast2_execution_set_error(error_message, error_message_size, "default output path exceeds supported length");
        return -1;
    }

    if (
        snprintf(
            job->generator_artifact_relative_path,
            sizeof(job->generator_artifact_relative_path),
            "outputs/images/%s/%s.b2",
            shard,
            job->job_id
        ) >= (int) sizeof(job->generator_artifact_relative_path)
    ) {
        beast2_execution_set_error(error_message, error_message_size, "default artifact path exceeds supported length");
        return -1;
    }

    if (
        snprintf(
            job->state_report_relative_path,
            sizeof(job->state_report_relative_path),
            "db/job_states/%s.txt",
            job->job_id
        ) >= (int) sizeof(job->state_report_relative_path)
    ) {
        beast2_execution_set_error(error_message, error_message_size, "default state path exceeds supported length");
        return -1;
    }

    return 0;
}

static int beast2_resolve_job_paths(
    const beast2_execution_context *context,
    beast2_execution_job *job,
    char *error_message,
    size_t error_message_size
) {
    if (beast2_build_default_paths(context, job, error_message, error_message_size) != 0) {
        return -1;
    }

    if (context->output_template != NULL) {
        if (
            beast2_resolve_template(
                context->output_template,
                context,
                job,
                job->output_relative_path,
                sizeof(job->output_relative_path),
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }
    }

    if (context->artifact_template != NULL) {
        if (
            beast2_resolve_template(
                context->artifact_template,
                context,
                job,
                job->generator_artifact_relative_path,
                sizeof(job->generator_artifact_relative_path),
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }
    }

    if (context->state_template != NULL) {
        if (
            beast2_resolve_template(
                context->state_template,
                context,
                job,
                job->state_report_relative_path,
                sizeof(job->state_report_relative_path),
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }
    }

    if (
        beast2_fs_join_path(
            job->output_path,
            sizeof(job->output_path),
            context->config->workspace_root,
            job->output_relative_path
        ) != 0 ||
        beast2_fs_join_path(
            job->generator_artifact_path,
            sizeof(job->generator_artifact_path),
            context->config->workspace_root,
            job->generator_artifact_relative_path
        ) != 0 ||
        beast2_fs_join_path(
            job->state_report_path,
            sizeof(job->state_report_path),
            context->config->workspace_root,
            job->state_report_relative_path
        ) != 0
    ) {
        beast2_execution_set_error(error_message, error_message_size, "failed to build absolute execution paths");
        return -1;
    }

    return 0;
}

static int beast2_write_model_output(
    const beast2_execution_context *context,
    const beast2_execution_job *job,
    char *error_message,
    size_t error_message_size
) {
    char contents[16384];

    if (
        snprintf(
            contents,
            sizeof(contents),
            "BEAST2_PHASE2_OUTPUT\n"
            "kind: simulated_media\n"
            "generator: %s\n"
            "engine: %s\n"
            "checkpoint: %s\n"
            "seed: %s\n"
            "steps: %s\n"
            "resolution: %s\n"
            "variant: %zu\n"
            "job_id: %s\n"
            "prompt_hash: %s\n"
            "prompt: %s\n"
            "tags: %s\n",
            context->generator_name,
            context->engine,
            context->checkpoint,
            context->seed,
            context->steps,
            context->resolution,
            job->variant_index + 1,
            job->job_id,
            job->prompt_hash,
            job->prompt,
            context->tags_joined[0] == '\0' ? "(none)" : context->tags_joined
        ) >= (int) sizeof(contents)
    ) {
        beast2_execution_set_error(error_message, error_message_size, "simulated output exceeds supported length");
        return -1;
    }

    return beast2_write_text_file(job->output_path, contents, error_message, error_message_size);
}

static int beast2_write_generator_artifact(
    const beast2_execution_context *context,
    const beast2_execution_job *job,
    char *error_message,
    size_t error_message_size
) {
    char contents[16384];
    size_t length = 0;
    size_t index = 0;

    length += (size_t) snprintf(
        contents + length,
        sizeof(contents) - length,
        "# Beast2 phase 2 reproducibility artifact\n\n"
        "$b2_generator\n"
        "b2_name %s\n"
        "b2_variant %zu\n"
        "b2_source_generator %s\n"
        "b2_job_id %s\n"
        "\n"
        "$b2_positive_prompt\n"
        "b2_section resolved_prompt\n"
        "b2_snippet %s\n"
        "\n",
        context->generator_name,
        job->variant_index + 1,
        context->document->source_path,
        job->job_id,
        job->prompt
    );

    if (length >= sizeof(contents)) {
        beast2_execution_set_error(error_message, error_message_size, "generator artifact exceeds supported length");
        return -1;
    }

    if (context->tags_section != NULL) {
        length += (size_t) snprintf(contents + length, sizeof(contents) - length, "$b2_tags\n");

        for (index = 0; index < context->tags_section->instruction_count; index++) {
            const beast2_instruction *instruction = &context->tags_section->instructions[index];

            if (strcmp(instruction->name, "b2_tag") != 0 || instruction->value[0] == '\0') {
                continue;
            }

            length += (size_t) snprintf(
                contents + length,
                sizeof(contents) - length,
                "b2_tag %s\n",
                instruction->value
            );

            if (length >= sizeof(contents)) {
                beast2_execution_set_error(error_message, error_message_size, "generator artifact exceeds supported length");
                return -1;
            }
        }

        length += (size_t) snprintf(contents + length, sizeof(contents) - length, "\n");
    }

    length += (size_t) snprintf(
        contents + length,
        sizeof(contents) - length,
        "$b2_workflow\n"
        "b2_prompt_build true\n"
        "b2_model_run %s\n"
        "b2_save_media %s\n"
        "b2_engine %s\n"
        "b2_checkpoint %s\n"
        "b2_seed %s\n"
        "b2_steps %s\n"
        "b2_resolution %s\n"
        "b2_output_path %s\n",
        job->prompt_hash,
        job->job_id,
        context->engine,
        context->checkpoint,
        context->seed,
        context->steps,
        context->resolution,
        job->output_relative_path
    );

    if (length >= sizeof(contents)) {
        beast2_execution_set_error(error_message, error_message_size, "generator artifact exceeds supported length");
        return -1;
    }

    return beast2_write_text_file(job->generator_artifact_path, contents, error_message, error_message_size);
}

static int beast2_write_state_report(
    const beast2_execution_context *context,
    const beast2_execution_job *job,
    char *error_message,
    size_t error_message_size
) {
    char contents[4096];

    if (
        snprintf(
            contents,
            sizeof(contents),
            "status: completed\n"
            "generator: %s\n"
            "job_id: %s\n"
            "variant: %zu\n"
            "step_1: b2_prompt_build completed\n"
            "step_2: b2_model_run completed\n"
            "step_3: b2_save_media completed\n"
            "output_path: %s\n"
            "generator_artifact: %s\n",
            context->generator_name,
            job->job_id,
            job->variant_index + 1,
            job->output_relative_path,
            job->generator_artifact_relative_path
        ) >= (int) sizeof(contents)
    ) {
        beast2_execution_set_error(error_message, error_message_size, "state report exceeds supported length");
        return -1;
    }

    return beast2_write_text_file(job->state_report_path, contents, error_message, error_message_size);
}

int beast2_execute_generator(
    const beast2_config *config,
    beast2_logger *logger,
    const char *generator_path,
    beast2_execution_summary *summary,
    char *error_message,
    size_t error_message_size
) {
    beast2_generator_document document;
    beast2_execution_context context;
    size_t variant_count = 0;
    size_t variant_index = 0;

    memset(summary, 0, sizeof(*summary));
    beast2_generator_document_init(&document);

    if (
        beast2_generator_parse_file(
            &document,
            generator_path,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    if (
        beast2_prepare_execution_context(
            &context,
            config,
            logger,
            &document,
            error_message,
            error_message_size
        ) != 0
    ) {
        beast2_generator_document_free(&document);
        return -1;
    }

    snprintf(summary->generator_name, sizeof(summary->generator_name), "%s", context.generator_name);
    snprintf(summary->engine, sizeof(summary->engine), "%s", context.engine);
    snprintf(summary->checkpoint, sizeof(summary->checkpoint), "%s", context.checkpoint);
    snprintf(summary->seed, sizeof(summary->seed), "%s", context.seed);

    variant_count = beast2_prompt_block_variant_count(context.prompt_block);
    summary->total_jobs = variant_count;

    beast2_logger_log(
        logger,
        BEAST2_LOG_LEVEL_INFO,
        "phase two execution starting: generator=%s engine=%s variants=%zu checkpoint=%s seed=%s",
        context.generator_name,
        context.engine,
        variant_count,
        context.checkpoint,
        context.seed
    );

    for (variant_index = 0; variant_index < document.warning_count; variant_index++) {
        beast2_logger_log(logger, BEAST2_LOG_LEVEL_WARN, "generator parse warning: %s", document.warnings[variant_index]);
    }

    for (variant_index = 0; variant_index < variant_count; variant_index++) {
        beast2_execution_job job;

        memset(&job, 0, sizeof(job));
        job.variant_index = variant_index;
        job.status = BEAST2_JOB_STATUS_PENDING;

        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] state=pending",
            variant_index + 1,
            variant_count
        );

        if (
            beast2_prompt_block_render_variant(
                context.prompt_block,
                variant_index,
                job.prompt,
                sizeof(job.prompt),
                error_message,
                error_message_size
            ) != 0
        ) {
            summary->failed_jobs++;
            beast2_generator_document_free(&document);
            return -1;
        }

        beast2_build_job_id(&context, &job);

        if (beast2_resolve_job_paths(&context, &job, error_message, error_message_size) != 0) {
            summary->failed_jobs++;
            beast2_generator_document_free(&document);
            return -1;
        }

        job.status = BEAST2_JOB_STATUS_RUNNING;
        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] state=running step=b2_prompt_build id=%s",
            variant_index + 1,
            variant_count,
            job.job_id
        );

        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] step=b2_model_run engine=%s checkpoint=%s prompt_hash=%s",
            variant_index + 1,
            variant_count,
            context.engine,
            context.checkpoint,
            job.prompt_hash
        );

        if (beast2_write_model_output(&context, &job, error_message, error_message_size) != 0) {
            job.status = BEAST2_JOB_STATUS_FAILED;
            summary->failed_jobs++;
            beast2_generator_document_free(&document);
            return -1;
        }

        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] step=b2_save_media output=%s",
            variant_index + 1,
            variant_count,
            job.output_relative_path
        );

        if (
            beast2_write_generator_artifact(
                &context,
                &job,
                error_message,
                error_message_size
            ) != 0 ||
            beast2_write_state_report(
                &context,
                &job,
                error_message,
                error_message_size
            ) != 0
        ) {
            job.status = BEAST2_JOB_STATUS_FAILED;
            summary->failed_jobs++;
            beast2_generator_document_free(&document);
            return -1;
        }

        job.status = BEAST2_JOB_STATUS_COMPLETED;
        summary->completed_jobs++;

        if (summary->first_output_path[0] == '\0') {
            snprintf(summary->first_output_path, sizeof(summary->first_output_path), "%s", job.output_path);
            snprintf(
                summary->first_generator_artifact_path,
                sizeof(summary->first_generator_artifact_path),
                "%s",
                job.generator_artifact_path
            );
        }

        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] state=completed id=%s",
            variant_index + 1,
            variant_count,
            job.job_id
        );
    }

    beast2_logger_log(
        logger,
        BEAST2_LOG_LEVEL_INFO,
        "phase two execution complete: generator=%s completed=%zu failed=%zu",
        context.generator_name,
        summary->completed_jobs,
        summary->failed_jobs
    );

    beast2_generator_document_free(&document);
    return 0;
}
