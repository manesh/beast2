#include "beast2/executor.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "beast2/filesystem.h"
#include "beast2/parser.h"
#include "beast2/runtime.h"

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

typedef struct beast2_scheduled_job {
    beast2_execution_job job;
    beast2_model_request request;
    beast2_model_category category;
    beast2_runtime_backend backend;
    beast2_precision_mode precision;
    beast2_gpu_job_ticket scheduler_ticket;
} beast2_scheduled_job;

typedef struct beast2_execution_context {
    const beast2_config *config;
    beast2_logger *logger;
    beast2_model_runtime_context *runtime_context;
    beast2_gpu_scheduler_context *scheduler;
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
    const char *backend;
    const char *precision;
    const char *runtime;
    const char *model_type;
    const char *output_template;
    const char *artifact_template;
    const char *state_template;
    const char *output_ext_override;
    const char *resolved_output_ext;
    const char *priority_label;
    beast2_model_category active_category;
    beast2_gpu_job_priority active_priority;
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

static const char *beast2_default_output_extension_for_category(beast2_model_category category) {
    switch (category) {
        case BEAST2_MODEL_CATEGORY_DIFFUSION:
            return "ppm";
        case BEAST2_MODEL_CATEGORY_VIDEO:
        case BEAST2_MODEL_CATEGORY_LLM:
        case BEAST2_MODEL_CATEGORY_UNKNOWN:
        default:
            return "txt";
    }
}

static beast2_gpu_job_priority beast2_execution_parse_priority(const char *value) {
    if (value == NULL || *value == '\0') {
        return BEAST2_GPU_JOB_PRIORITY_BATCH_GENERATION;
    }

    if (strcmp(value, "preview") == 0 || strcmp(value, "interactive_preview") == 0) {
        return BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW;
    }

    if (strcmp(value, "interactive") == 0 || strcmp(value, "interactive_generation") == 0) {
        return BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_GENERATION;
    }

    if (strcmp(value, "background") == 0) {
        return BEAST2_GPU_JOB_PRIORITY_BACKGROUND;
    }

    return BEAST2_GPU_JOB_PRIORITY_BATCH_GENERATION;
}

static void beast2_execution_estimate_vram(
    const beast2_model_request *request,
    beast2_gpu_job_priority priority,
    size_t *model_vram_mb,
    size_t *job_vram_mb,
    size_t *ui_vram_mb,
    size_t *runtime_ms
) {
    beast2_model_category category = beast2_model_request_category(request);
    size_t width = 32;
    size_t height = 32;
    size_t pixels = 0;
    size_t steps = 1;

    if (request->resolution != NULL) {
        const char *separator = strchr(request->resolution, 'x');

        if (separator == NULL) {
            separator = strchr(request->resolution, 'X');
        }

        if (separator != NULL) {
            char width_buffer[32];
            char height_buffer[32];
            size_t width_length = (size_t) (separator - request->resolution);
            size_t height_length = strlen(separator + 1);

            if (
                width_length > 0 &&
                height_length > 0 &&
                width_length < sizeof(width_buffer) &&
                height_length < sizeof(height_buffer)
            ) {
                memcpy(width_buffer, request->resolution, width_length);
                width_buffer[width_length] = '\0';
                memcpy(height_buffer, separator + 1, height_length);
                height_buffer[height_length] = '\0';
                width = (size_t) strtoull(width_buffer, NULL, 10);
                height = (size_t) strtoull(height_buffer, NULL, 10);
            }
        }
    }

    if (request->steps != NULL && *request->steps != '\0') {
        steps = (size_t) strtoull(request->steps, NULL, 10);
        if (steps == 0) {
            steps = 1;
        }
    }

    pixels = width * height;

    switch (category) {
        case BEAST2_MODEL_CATEGORY_VIDEO:
            *model_vram_mb = 6144;
            *job_vram_mb = 1024 + (pixels / 512);
            *runtime_ms = 60 + (steps * 4);
            break;
        case BEAST2_MODEL_CATEGORY_LLM:
            *model_vram_mb = 3072;
            *job_vram_mb = 512 + (steps * 8);
            *runtime_ms = 20 + (steps * 2);
            break;
        case BEAST2_MODEL_CATEGORY_DIFFUSION:
        case BEAST2_MODEL_CATEGORY_UNKNOWN:
        default:
            *model_vram_mb = 4096;
            *job_vram_mb = 768 + (pixels / 1024) + (steps * 4);
            *runtime_ms = 30 + (steps * 3);
            break;
    }

    if (priority == BEAST2_GPU_JOB_PRIORITY_INTERACTIVE_PREVIEW) {
        *ui_vram_mb = 512 + (pixels / 2048);
        if (*job_vram_mb > 256) {
            *job_vram_mb = 256;
        }
    } else {
        *ui_vram_mb = 0;
    }
}

static const char *beast2_default_output_root_for_category(beast2_model_category category) {
    switch (category) {
        case BEAST2_MODEL_CATEGORY_VIDEO:
            return "outputs/videos";
        case BEAST2_MODEL_CATEGORY_DIFFUSION:
        case BEAST2_MODEL_CATEGORY_LLM:
        case BEAST2_MODEL_CATEGORY_UNKNOWN:
        default:
            return "outputs/images";
    }
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
        if (context->resolved_output_ext != NULL) {
            return context->resolved_output_ext;
        }

        if (context->output_ext_override != NULL) {
            return context->output_ext_override;
        }

        return "txt";
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
    beast2_gpu_scheduler_context *scheduler,
    beast2_model_runtime_context *runtime_context,
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
    context->scheduler = scheduler;
    context->runtime_context = runtime_context;
    context->document = document;
    context->prompt_block = prompt_block;
    context->workflow_section = beast2_generator_find_metadata_section(document, "b2_workflow");
    context->tags_section = beast2_generator_find_metadata_section(document, "b2_tags");
    context->generator_name = document->generator_name != NULL ? document->generator_name : "unnamed_generator";
    context->engine = "stable_diffusion";
    context->checkpoint = "stable_diffusion#default";
    context->seed = "0";
    context->steps = "1";
    context->resolution = "32x32";
    context->backend = NULL;
    context->precision = "fp32";
    context->runtime = NULL;
    context->model_type = NULL;
    context->output_ext_override = NULL;
    context->resolved_output_ext = NULL;
    context->priority_label = "batch";
    context->active_category = BEAST2_MODEL_CATEGORY_UNKNOWN;
    context->active_priority = BEAST2_GPU_JOB_PRIORITY_BATCH_GENERATION;

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

        value = beast2_metadata_first_value(context->workflow_section, "b2_backend");
        if (value != NULL && *value != '\0') {
            context->backend = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_precision");
        if (value != NULL && *value != '\0') {
            context->precision = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_runtime");
        if (value != NULL && *value != '\0') {
            context->runtime = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_model_type");
        if (value != NULL && *value != '\0') {
            context->model_type = value;
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
            context->output_ext_override = value;
        }

        value = beast2_metadata_first_value(context->workflow_section, "b2_priority");
        if (value != NULL && *value != '\0') {
            context->priority_label = value;
            context->active_priority = beast2_execution_parse_priority(value);
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
    const char *output_root = beast2_default_output_root_for_category(context->active_category);
    const char *output_ext = context->resolved_output_ext != NULL ? context->resolved_output_ext : "txt";

    shard[0] = job->job_id[0];
    shard[1] = job->job_id[1];
    shard[2] = '\0';

    if (
        snprintf(
            job->output_relative_path,
            sizeof(job->output_relative_path),
            "%s/%s/%s.%s",
            output_root,
            shard,
            job->job_id,
            output_ext
        ) >= (int) sizeof(job->output_relative_path)
    ) {
        beast2_execution_set_error(error_message, error_message_size, "default output path exceeds supported length");
        return -1;
    }

    if (
        snprintf(
            job->generator_artifact_relative_path,
            sizeof(job->generator_artifact_relative_path),
            "%s/%s/%s.b2",
            output_root,
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
    const beast2_model_result *result,
    const beast2_execution_job *job,
    char *error_message,
    size_t error_message_size
) {
    return beast2_write_text_file(job->output_path, result->content, error_message, error_message_size);
}

static int beast2_write_generator_artifact(
    const beast2_execution_context *context,
    const beast2_execution_job *job,
    const beast2_model_handle *handle,
    const beast2_model_result *result,
    char *error_message,
    size_t error_message_size
) {
    char contents[16384];
    size_t length = 0;
    size_t index = 0;

    length += (size_t) snprintf(
        contents + length,
        sizeof(contents) - length,
        "# Beast2 phase 3 reproducibility artifact\n\n"
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
        "b2_backend %s\n"
        "b2_precision %s\n"
        "b2_model_type %s\n"
        "b2_output_kind %s\n"
        "b2_seed %s\n"
        "b2_steps %s\n"
        "b2_resolution %s\n"
        "b2_output_path %s\n",
        job->prompt_hash,
        job->job_id,
        context->engine,
        context->checkpoint,
        beast2_runtime_backend_name(handle->backend),
        beast2_precision_mode_name(handle->precision),
        beast2_model_category_name(handle->category),
        beast2_output_kind_name(result->output_kind),
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
    const beast2_model_handle *handle,
    const beast2_model_result *result,
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
            "engine: %s\n"
            "checkpoint: %s\n"
            "backend: %s\n"
            "precision: %s\n"
            "model_type: %s\n"
            "output_kind: %s\n"
            "step_1: b2_prompt_build completed\n"
            "step_2: b2_model_run completed\n"
            "step_3: b2_save_media completed\n"
            "inference_steps: %zu\n"
            "output_path: %s\n"
            "generator_artifact: %s\n",
            context->generator_name,
            job->job_id,
            job->variant_index + 1,
            context->engine,
            context->checkpoint,
            beast2_runtime_backend_name(handle->backend),
            beast2_precision_mode_name(handle->precision),
            beast2_model_category_name(handle->category),
            beast2_output_kind_name(result->output_kind),
            result->inference_steps,
            job->output_relative_path,
            job->generator_artifact_relative_path
        ) >= (int) sizeof(contents)
    ) {
        beast2_execution_set_error(error_message, error_message_size, "state report exceeds supported length");
        return -1;
    }

    return beast2_write_text_file(job->state_report_path, contents, error_message, error_message_size);
}

static beast2_scheduled_job *beast2_find_scheduled_job(
    beast2_scheduled_job *jobs,
    size_t job_count,
    const char *job_key
) {
    size_t index = 0;

    for (index = 0; index < job_count; index++) {
        if (strcmp(jobs[index].job.job_id, job_key) == 0) {
            return &jobs[index];
        }
    }

    return NULL;
}

int beast2_execute_generator(
    const beast2_config *config,
    beast2_logger *logger,
    beast2_media_library_context *media_library,
    beast2_gpu_scheduler_context *scheduler,
    beast2_model_runtime_context *runtime_context,
    const char *generator_path,
    beast2_execution_summary *summary,
    char *error_message,
    size_t error_message_size
) {
    beast2_generator_document document;
    beast2_execution_context context;
    beast2_scheduled_job *scheduled_jobs = NULL;
    beast2_gpu_scheduler_telemetry scheduler_telemetry;
    size_t variant_count = 0;
    size_t variant_index = 0;

    if (media_library == NULL || scheduler == NULL) {
        beast2_execution_set_error(error_message, error_message_size, "media library and scheduler contexts are required");
        return -1;
    }

    memset(summary, 0, sizeof(*summary));
    memset(&scheduler_telemetry, 0, sizeof(scheduler_telemetry));
    beast2_generator_document_init(&document);
    snprintf(summary->database_path, sizeof(summary->database_path), "%s", media_library->db_path);

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
            scheduler,
            runtime_context,
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
    scheduled_jobs = (beast2_scheduled_job *) calloc(variant_count, sizeof(*scheduled_jobs));

    if (scheduled_jobs == NULL) {
        beast2_generator_document_free(&document);
        beast2_execution_set_error(error_message, error_message_size, "out of memory while allocating scheduled jobs");
        return -1;
    }

    beast2_logger_log(
        logger,
        BEAST2_LOG_LEVEL_INFO,
        "phase five execution starting: generator=%s engine=%s variants=%zu checkpoint=%s seed=%s priority=%s",
        context.generator_name,
        context.engine,
        variant_count,
        context.checkpoint,
        context.seed,
        beast2_gpu_job_priority_name(context.active_priority)
    );

    for (variant_index = 0; variant_index < document.warning_count; variant_index++) {
        beast2_logger_log(logger, BEAST2_LOG_LEVEL_WARN, "generator parse warning: %s", document.warnings[variant_index]);
    }

    for (variant_index = 0; variant_index < variant_count; variant_index++) {
        beast2_scheduled_job *scheduled_job = &scheduled_jobs[variant_index];
        beast2_gpu_job_request scheduler_request;
        const char *output_ext = NULL;
        size_t model_vram_mb = 0;
        size_t job_vram_mb = 0;
        size_t ui_vram_mb = 0;
        size_t runtime_ms = 0;

        memset(scheduled_job, 0, sizeof(*scheduled_job));
        memset(&scheduler_request, 0, sizeof(scheduler_request));
        scheduled_job->job.variant_index = variant_index;
        scheduled_job->job.status = BEAST2_JOB_STATUS_PENDING;

        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] state=queued priority=%s",
            variant_index + 1,
            variant_count,
            beast2_gpu_job_priority_name(context.active_priority)
        );

        if (
            beast2_prompt_block_render_variant(
                context.prompt_block,
                variant_index,
                scheduled_job->job.prompt,
                sizeof(scheduled_job->job.prompt),
                error_message,
                error_message_size
            ) != 0
        ) {
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }

        beast2_build_job_id(&context, &scheduled_job->job);

        scheduled_job->request.engine = context.engine;
        scheduled_job->request.checkpoint = context.checkpoint;
        scheduled_job->request.prompt = scheduled_job->job.prompt;
        scheduled_job->request.seed = context.seed;
        scheduled_job->request.steps = context.steps;
        scheduled_job->request.resolution = context.resolution;
        scheduled_job->request.backend = context.backend;
        scheduled_job->request.precision = context.precision;
        scheduled_job->request.runtime = context.runtime;
        scheduled_job->request.model_type = context.model_type;
        scheduled_job->category = beast2_model_request_category(&scheduled_job->request);
        scheduled_job->backend = beast2_model_request_backend(&scheduled_job->request);
        scheduled_job->precision = beast2_model_request_precision(&scheduled_job->request);

        output_ext = context.output_ext_override != NULL
            ? context.output_ext_override
            : beast2_default_output_extension_for_category(scheduled_job->category);
        context.resolved_output_ext = output_ext;
        context.active_category = scheduled_job->category;

        if (
            beast2_resolve_job_paths(
                &context,
                &scheduled_job->job,
                error_message,
                error_message_size
            ) != 0
        ) {
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }

        beast2_execution_estimate_vram(
            &scheduled_job->request,
            context.active_priority,
            &model_vram_mb,
            &job_vram_mb,
            &ui_vram_mb,
            &runtime_ms
        );

        scheduler_request.job_key = scheduled_job->job.job_id;
        scheduler_request.engine = context.engine;
        scheduler_request.checkpoint = context.checkpoint;
        scheduler_request.category = scheduled_job->category;
        scheduler_request.priority = context.active_priority;
        scheduler_request.estimated_model_vram_mb = model_vram_mb;
        scheduler_request.estimated_job_vram_mb = job_vram_mb;
        scheduler_request.estimated_ui_vram_mb = ui_vram_mb;
        scheduler_request.estimated_runtime_ms = runtime_ms;

        if (
            beast2_gpu_scheduler_enqueue(
                scheduler,
                &scheduler_request,
                &scheduled_job->scheduler_ticket,
                error_message,
                error_message_size
            ) != 0
        ) {
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }
    }

    for (;;) {
        beast2_scheduled_job *scheduled_job = NULL;
        beast2_media_record media_record;
        beast2_media_record_result media_result;
        beast2_model_handle handle;
        beast2_model_result result;
        beast2_gpu_job_ticket active_ticket;
        int start_status = 0;

        memset(&media_record, 0, sizeof(media_record));
        memset(&media_result, 0, sizeof(media_result));
        memset(&handle, 0, sizeof(handle));
        memset(&result, 0, sizeof(result));
        memset(&active_ticket, 0, sizeof(active_ticket));

        start_status = beast2_gpu_scheduler_start_next(
            scheduler,
            &active_ticket,
            error_message,
            error_message_size
        );

        if (start_status < 0) {
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }

        if (start_status == 0) {
            break;
        }

        scheduled_job = beast2_find_scheduled_job(
            scheduled_jobs,
            variant_count,
            active_ticket.job_key
        );

        if (scheduled_job == NULL) {
            beast2_execution_set_error(error_message, error_message_size, "scheduler returned an unknown job key");
            beast2_gpu_scheduler_fail(scheduler, &active_ticket, error_message, error_message_size);
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }

        scheduled_job->job.status = BEAST2_JOB_STATUS_RUNNING;
        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] state=running step=b2_prompt_build id=%s queue_priority=%s",
            scheduled_job->job.variant_index + 1,
            variant_count,
            scheduled_job->job.job_id,
            beast2_gpu_job_priority_name(active_ticket.priority)
        );

        if (
            beast2_model_load(
                runtime_context,
                &scheduled_job->request,
                &handle,
                error_message,
                error_message_size
            ) != 0
        ) {
            scheduled_job->job.status = BEAST2_JOB_STATUS_FAILED;
            beast2_gpu_scheduler_fail(scheduler, &active_ticket, error_message, error_message_size);
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }

        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] step=b2_model_run engine=%s checkpoint=%s backend=%s precision=%s prompt_hash=%s scheduler_cache_hit=%s runtime_cache_hit=%s",
            scheduled_job->job.variant_index + 1,
            variant_count,
            context.engine,
            context.checkpoint,
            beast2_runtime_backend_name(handle.backend),
            beast2_precision_mode_name(handle.precision),
            scheduled_job->job.prompt_hash,
            active_ticket.model_cache_hit ? "true" : "false",
            handle.cache_hit ? "true" : "false"
        );

        if (
            beast2_model_infer(
                runtime_context,
                &handle,
                &scheduled_job->request,
                &result,
                error_message,
                error_message_size
            ) != 0 ||
            beast2_write_model_output(&result, &scheduled_job->job, error_message, error_message_size) != 0
        ) {
            scheduled_job->job.status = BEAST2_JOB_STATUS_FAILED;
            beast2_model_unload(runtime_context, &handle);
            beast2_gpu_scheduler_fail(
                scheduler,
                &active_ticket,
                error_message,
                error_message_size
            );
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }

        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] step=b2_save_media output=%s kind=%s",
            scheduled_job->job.variant_index + 1,
            variant_count,
            scheduled_job->job.output_relative_path,
            beast2_output_kind_name(result.output_kind)
        );

        if (
            beast2_write_generator_artifact(
                &context,
                &scheduled_job->job,
                &handle,
                &result,
                error_message,
                error_message_size
            ) != 0 ||
            beast2_write_state_report(
                &context,
                &scheduled_job->job,
                &handle,
                &result,
                error_message,
                error_message_size
            ) != 0
        ) {
            scheduled_job->job.status = BEAST2_JOB_STATUS_FAILED;
            beast2_model_unload(runtime_context, &handle);
            beast2_gpu_scheduler_fail(
                scheduler,
                &active_ticket,
                error_message,
                error_message_size
            );
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }

        media_record.generator_name = context.generator_name;
        media_record.generator_source_path = context.document->source_path;
        media_record.generator_artifact_relative_path = scheduled_job->job.generator_artifact_relative_path;
        media_record.generator_artifact_path = scheduled_job->job.generator_artifact_path;
        media_record.output_relative_path = scheduled_job->job.output_relative_path;
        media_record.output_path = scheduled_job->job.output_path;
        media_record.output_kind = beast2_output_kind_name(result.output_kind);
        media_record.engine = context.engine;
        media_record.checkpoint = context.checkpoint;
        media_record.backend = beast2_runtime_backend_name(handle.backend);
        media_record.precision = beast2_precision_mode_name(handle.precision);
        media_record.seed = context.seed;
        media_record.resolution = context.resolution;
        media_record.prompt = scheduled_job->job.prompt;
        media_record.prompt_hash = scheduled_job->job.prompt_hash;
        media_record.tags_csv = context.tags_joined;

        if (
            beast2_media_library_record_output(
                media_library,
                &media_record,
                &media_result,
                error_message,
                error_message_size
            ) != 0
        ) {
            scheduled_job->job.status = BEAST2_JOB_STATUS_FAILED;
            beast2_model_unload(runtime_context, &handle);
            beast2_gpu_scheduler_fail(
                scheduler,
                &active_ticket,
                error_message,
                error_message_size
            );
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }

        beast2_model_unload(runtime_context, &handle);
        if (
            beast2_gpu_scheduler_complete(
                scheduler,
                &active_ticket,
                error_message,
                error_message_size
            ) != 0
        ) {
            scheduled_job->job.status = BEAST2_JOB_STATUS_FAILED;
            summary->failed_jobs++;
            free(scheduled_jobs);
            beast2_generator_document_free(&document);
            return -1;
        }

        scheduled_job->job.status = BEAST2_JOB_STATUS_COMPLETED;
        summary->completed_jobs++;

        snprintf(summary->backend, sizeof(summary->backend), "%s", beast2_runtime_backend_name(handle.backend));
        snprintf(summary->precision, sizeof(summary->precision), "%s", beast2_precision_mode_name(handle.precision));
        snprintf(summary->model_category, sizeof(summary->model_category), "%s", beast2_model_category_name(handle.category));
        snprintf(summary->output_kind, sizeof(summary->output_kind), "%s", beast2_output_kind_name(result.output_kind));
        beast2_gpu_scheduler_get_telemetry(scheduler, &scheduler_telemetry);
        summary->cache_hits = scheduler_telemetry.cache_hits;
        summary->cache_misses = scheduler_telemetry.cache_misses;
        summary->scheduler_peak_queue_length = scheduler_telemetry.peak_queue_length;
        summary->scheduler_model_evictions = scheduler_telemetry.model_evictions;
        summary->scheduler_peak_reserved_vram_mb = scheduler_telemetry.peak_reserved_vram_mb;

        if (summary->first_output_path[0] == '\0') {
            snprintf(summary->first_output_path, sizeof(summary->first_output_path), "%s", scheduled_job->job.output_path);
            snprintf(summary->first_thumbnail_path, sizeof(summary->first_thumbnail_path), "%s", media_result.thumbnail_path);
            snprintf(
                summary->first_generator_artifact_path,
                sizeof(summary->first_generator_artifact_path),
                "%s",
                scheduled_job->job.generator_artifact_path
            );
        }

        beast2_logger_log(
            logger,
            BEAST2_LOG_LEVEL_INFO,
            "job[%zu/%zu] state=completed id=%s model=%s",
            scheduled_job->job.variant_index + 1,
            variant_count,
            scheduled_job->job.job_id,
            handle.model_id
        );
    }

    beast2_gpu_scheduler_get_telemetry(scheduler, &scheduler_telemetry);
    summary->cache_hits = scheduler_telemetry.cache_hits;
    summary->cache_misses = scheduler_telemetry.cache_misses;
    summary->scheduler_peak_queue_length = scheduler_telemetry.peak_queue_length;
    summary->scheduler_model_evictions = scheduler_telemetry.model_evictions;
    summary->scheduler_peak_reserved_vram_mb = scheduler_telemetry.peak_reserved_vram_mb;

    beast2_logger_log(
        logger,
        BEAST2_LOG_LEVEL_INFO,
        "phase five execution complete: generator=%s completed=%zu failed=%zu queue_peak=%zu cache_hits=%zu cache_misses=%zu model_evictions=%zu peak_reserved_vram_mb=%zu",
        context.generator_name,
        summary->completed_jobs,
        summary->failed_jobs,
        summary->scheduler_peak_queue_length,
        summary->cache_hits,
        summary->cache_misses,
        summary->scheduler_model_evictions,
        summary->scheduler_peak_reserved_vram_mb
    );

    free(scheduled_jobs);
    beast2_generator_document_free(&document);
    return 0;
}
