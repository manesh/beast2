#ifndef BEAST2_LLM_INTEGRATION_H
#define BEAST2_LLM_INTEGRATION_H

#include <stddef.h>

#include "beast2/config.h"
#include "beast2/media_library.h"

typedef enum beast2_llm_task_kind {
    BEAST2_LLM_TASK_NONE = 0,
    BEAST2_LLM_TASK_PROMPT_MUTATION = 1,
    BEAST2_LLM_TASK_GENERATOR_EDIT = 2,
    BEAST2_LLM_TASK_TAG_GENERATION = 3,
    BEAST2_LLM_TASK_DATABASE_QUERY = 4,
    BEAST2_LLM_TASK_KNOWLEDGE_QUERY = 5,
    BEAST2_LLM_TASK_BELIEF_CONDITIONING = 6,
    BEAST2_LLM_TASK_PROMPT_LIBRARY_QUERY = 7
} beast2_llm_task_kind;

typedef struct beast2_llm_workflow {
    beast2_llm_task_kind kind;
    const char *task_name;
    const char *instruction;
    const char *source_generator_path;
    const char *query_sql;
    const char *knowledge_source;
    const char *knowledge_terms;
    size_t attempts;
} beast2_llm_workflow;

typedef struct beast2_llm_task_result {
    char prepared_prompt[32768];
    char final_output[BEAST2_MAX_PATH_LENGTH * 4];
    char generated_tags_csv[1024];
    char generated_generator_relative_path[BEAST2_MAX_PATH_LENGTH];
    char generated_generator_path[BEAST2_MAX_PATH_LENGTH];
    int has_generated_generator;
} beast2_llm_task_result;

beast2_llm_task_kind beast2_llm_task_kind_from_string(const char *value);

int beast2_llm_prepare_prompt(
    beast2_media_library_context *media_library,
    const beast2_llm_workflow *workflow,
    const char *base_prompt,
    char *prepared_prompt,
    size_t prepared_prompt_size,
    char *error_message,
    size_t error_message_size
);

int beast2_llm_finalize_output(
    const beast2_config *config,
    const beast2_llm_workflow *workflow,
    const char *job_id,
    const char *prepared_prompt,
    const char *runtime_output,
    beast2_llm_task_result *result,
    char *error_message,
    size_t error_message_size
);

#endif
