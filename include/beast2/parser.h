#ifndef BEAST2_PARSER_H
#define BEAST2_PARSER_H

#include <stddef.h>

typedef enum beast2_concat_mode {
    BEAST2_CONCAT_NONE = 0,
    BEAST2_CONCAT_COMMA = 1,
    BEAST2_CONCAT_SPACE = 2,
    BEAST2_CONCAT_NEWLINE = 3
} beast2_concat_mode;

typedef struct beast2_instruction {
    char *name;
    char *value;
} beast2_instruction;

typedef struct beast2_metadata_section {
    char *name;
    beast2_instruction *instructions;
    size_t instruction_count;
    size_t instruction_capacity;
} beast2_metadata_section;

typedef struct beast2_prompt_section {
    char *name;
    beast2_concat_mode separator_before;
    char **snippets;
    size_t snippet_count;
    size_t snippet_capacity;
} beast2_prompt_section;

typedef struct beast2_prompt_block {
    char *name;
    beast2_prompt_section *sections;
    size_t section_count;
    size_t section_capacity;
    beast2_concat_mode pending_separator;
} beast2_prompt_block;

typedef struct beast2_generator_document {
    char *source_path;
    char *generator_name;
    beast2_prompt_block *prompt_blocks;
    size_t prompt_block_count;
    size_t prompt_block_capacity;
    beast2_metadata_section *metadata_sections;
    size_t metadata_section_count;
    size_t metadata_section_capacity;
    char **warnings;
    size_t warning_count;
    size_t warning_capacity;
} beast2_generator_document;

void beast2_generator_document_init(beast2_generator_document *document);
void beast2_generator_document_free(beast2_generator_document *document);

int beast2_generator_parse_file(
    beast2_generator_document *document,
    const char *path,
    char *error_message,
    size_t error_message_size
);

const beast2_prompt_block *beast2_generator_primary_prompt_block(
    const beast2_generator_document *document
);

const beast2_metadata_section *beast2_generator_find_metadata_section(
    const beast2_generator_document *document,
    const char *section_name
);

const char *beast2_metadata_first_value(
    const beast2_metadata_section *section,
    const char *instruction_name
);

size_t beast2_prompt_block_variant_count(const beast2_prompt_block *block);

int beast2_prompt_block_render_variant(
    const beast2_prompt_block *block,
    size_t variant_index,
    char *destination,
    size_t destination_size,
    char *error_message,
    size_t error_message_size
);

const char *beast2_concat_separator(beast2_concat_mode mode);

#endif
