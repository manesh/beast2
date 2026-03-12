#include "beast2/parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void beast2_parser_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static char *beast2_parser_strdup(const char *value) {
    char *copy = NULL;
    size_t length = 0;

    if (value == NULL) {
        return NULL;
    }

    length = strlen(value) + 1;
    copy = (char *) malloc(length);

    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, value, length);
    return copy;
}

static char *beast2_parser_trim(char *value) {
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

static int beast2_parser_is_prompt_header(const char *header_name) {
    return strstr(header_name, "prompt") != NULL;
}

static int beast2_parser_ensure_prompt_blocks(
    beast2_generator_document *document,
    char *error_message,
    size_t error_message_size
) {
    beast2_prompt_block *blocks = NULL;
    size_t new_capacity = 0;

    if (document->prompt_block_count < document->prompt_block_capacity) {
        return 0;
    }

    new_capacity = document->prompt_block_capacity == 0 ? 4 : document->prompt_block_capacity * 2;
    blocks = (beast2_prompt_block *) realloc(document->prompt_blocks, new_capacity * sizeof(*blocks));

    if (blocks == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while growing prompt blocks");
        return -1;
    }

    document->prompt_blocks = blocks;
    document->prompt_block_capacity = new_capacity;
    return 0;
}

static int beast2_parser_ensure_metadata_sections(
    beast2_generator_document *document,
    char *error_message,
    size_t error_message_size
) {
    beast2_metadata_section *sections = NULL;
    size_t new_capacity = 0;

    if (document->metadata_section_count < document->metadata_section_capacity) {
        return 0;
    }

    new_capacity = document->metadata_section_capacity == 0 ? 4 : document->metadata_section_capacity * 2;
    sections = (beast2_metadata_section *) realloc(
        document->metadata_sections,
        new_capacity * sizeof(*sections)
    );

    if (sections == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while growing metadata sections");
        return -1;
    }

    document->metadata_sections = sections;
    document->metadata_section_capacity = new_capacity;
    return 0;
}

static int beast2_parser_ensure_prompt_sections(
    beast2_prompt_block *block,
    char *error_message,
    size_t error_message_size
) {
    beast2_prompt_section *sections = NULL;
    size_t new_capacity = 0;

    if (block->section_count < block->section_capacity) {
        return 0;
    }

    new_capacity = block->section_capacity == 0 ? 4 : block->section_capacity * 2;
    sections = (beast2_prompt_section *) realloc(block->sections, new_capacity * sizeof(*sections));

    if (sections == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while growing prompt sections");
        return -1;
    }

    block->sections = sections;
    block->section_capacity = new_capacity;
    return 0;
}

static int beast2_parser_ensure_snippets(
    beast2_prompt_section *section,
    char *error_message,
    size_t error_message_size
) {
    char **snippets = NULL;
    size_t new_capacity = 0;

    if (section->snippet_count < section->snippet_capacity) {
        return 0;
    }

    new_capacity = section->snippet_capacity == 0 ? 4 : section->snippet_capacity * 2;
    snippets = (char **) realloc(section->snippets, new_capacity * sizeof(*snippets));

    if (snippets == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while growing snippets");
        return -1;
    }

    section->snippets = snippets;
    section->snippet_capacity = new_capacity;
    return 0;
}

static int beast2_parser_ensure_instructions(
    beast2_metadata_section *section,
    char *error_message,
    size_t error_message_size
) {
    beast2_instruction *instructions = NULL;
    size_t new_capacity = 0;

    if (section->instruction_count < section->instruction_capacity) {
        return 0;
    }

    new_capacity = section->instruction_capacity == 0 ? 4 : section->instruction_capacity * 2;
    instructions = (beast2_instruction *) realloc(section->instructions, new_capacity * sizeof(*instructions));

    if (instructions == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while growing metadata instructions");
        return -1;
    }

    section->instructions = instructions;
    section->instruction_capacity = new_capacity;
    return 0;
}

static int beast2_parser_ensure_warnings(
    beast2_generator_document *document,
    char *error_message,
    size_t error_message_size
) {
    char **warnings = NULL;
    size_t new_capacity = 0;

    if (document->warning_count < document->warning_capacity) {
        return 0;
    }

    new_capacity = document->warning_capacity == 0 ? 4 : document->warning_capacity * 2;
    warnings = (char **) realloc(document->warnings, new_capacity * sizeof(*warnings));

    if (warnings == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while growing warnings");
        return -1;
    }

    document->warnings = warnings;
    document->warning_capacity = new_capacity;
    return 0;
}

static int beast2_parser_add_warning(
    beast2_generator_document *document,
    const char *message,
    char *error_message,
    size_t error_message_size
) {
    char *warning = NULL;

    if (beast2_parser_ensure_warnings(document, error_message, error_message_size) != 0) {
        return -1;
    }

    warning = beast2_parser_strdup(message);

    if (warning == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while storing parser warning");
        return -1;
    }

    document->warnings[document->warning_count] = warning;
    document->warning_count++;
    return 0;
}

static beast2_prompt_block *beast2_parser_add_prompt_block(
    beast2_generator_document *document,
    const char *name,
    char *error_message,
    size_t error_message_size
) {
    beast2_prompt_block *block = NULL;
    char *copy = NULL;

    if (beast2_parser_ensure_prompt_blocks(document, error_message, error_message_size) != 0) {
        return NULL;
    }

    copy = beast2_parser_strdup(name);

    if (copy == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while storing prompt block name");
        return NULL;
    }

    block = &document->prompt_blocks[document->prompt_block_count];
    memset(block, 0, sizeof(*block));
    block->name = copy;
    block->pending_separator = BEAST2_CONCAT_NONE;
    document->prompt_block_count++;
    return block;
}

static beast2_metadata_section *beast2_parser_add_metadata_section(
    beast2_generator_document *document,
    const char *name,
    char *error_message,
    size_t error_message_size
) {
    beast2_metadata_section *section = NULL;
    char *copy = NULL;

    if (beast2_parser_ensure_metadata_sections(document, error_message, error_message_size) != 0) {
        return NULL;
    }

    copy = beast2_parser_strdup(name);

    if (copy == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while storing metadata section name");
        return NULL;
    }

    section = &document->metadata_sections[document->metadata_section_count];
    memset(section, 0, sizeof(*section));
    section->name = copy;
    document->metadata_section_count++;
    return section;
}

static beast2_prompt_section *beast2_parser_add_prompt_section(
    beast2_generator_document *document,
    beast2_prompt_block *block,
    const char *name,
    size_t line_number,
    char *error_message,
    size_t error_message_size
) {
    beast2_prompt_section *section = NULL;
    char *copy = NULL;
    char warning[256];

    if (beast2_parser_ensure_prompt_sections(block, error_message, error_message_size) != 0) {
        return NULL;
    }

    copy = beast2_parser_strdup(name);

    if (copy == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while storing prompt section name");
        return NULL;
    }

    section = &block->sections[block->section_count];
    memset(section, 0, sizeof(*section));
    section->name = copy;

    if (block->section_count == 0) {
        section->separator_before = BEAST2_CONCAT_NONE;
    } else if (block->pending_separator == BEAST2_CONCAT_NONE) {
        section->separator_before = BEAST2_CONCAT_SPACE;
        snprintf(
            warning,
            sizeof(warning),
            "line %zu: missing concat instruction before section '%s'; defaulting to space",
            line_number,
            name
        );

        if (beast2_parser_add_warning(document, warning, error_message, error_message_size) != 0) {
            free(copy);
            return NULL;
        }
    } else {
        section->separator_before = block->pending_separator;
    }

    block->pending_separator = BEAST2_CONCAT_NONE;
    block->section_count++;
    return section;
}

static int beast2_parser_add_snippet(
    beast2_prompt_section *section,
    const char *value,
    char *error_message,
    size_t error_message_size
) {
    char *copy = NULL;

    if (beast2_parser_ensure_snippets(section, error_message, error_message_size) != 0) {
        return -1;
    }

    copy = beast2_parser_strdup(value);

    if (copy == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while storing snippet");
        return -1;
    }

    section->snippets[section->snippet_count] = copy;
    section->snippet_count++;
    return 0;
}

static int beast2_parser_add_instruction(
    beast2_generator_document *document,
    beast2_metadata_section *section,
    const char *name,
    const char *value,
    char *error_message,
    size_t error_message_size
) {
    beast2_instruction *instruction = NULL;
    char *name_copy = NULL;
    char *value_copy = NULL;

    if (beast2_parser_ensure_instructions(section, error_message, error_message_size) != 0) {
        return -1;
    }

    name_copy = beast2_parser_strdup(name);
    value_copy = beast2_parser_strdup(value);

    if (name_copy == NULL || value_copy == NULL) {
        free(name_copy);
        free(value_copy);
        beast2_parser_set_error(error_message, error_message_size, "out of memory while storing instruction");
        return -1;
    }

    instruction = &section->instructions[section->instruction_count];
    instruction->name = name_copy;
    instruction->value = value_copy;
    section->instruction_count++;

    if (
        document->generator_name == NULL &&
        strcmp(section->name, "b2_generator") == 0 &&
        strcmp(name, "b2_name") == 0 &&
        value[0] != '\0'
    ) {
        document->generator_name = beast2_parser_strdup(value);

        if (document->generator_name == NULL) {
            beast2_parser_set_error(error_message, error_message_size, "out of memory while storing generator name");
            return -1;
        }
    }

    return 0;
}

static int beast2_parser_append_text(
    char *destination,
    size_t destination_size,
    size_t *length,
    const char *text,
    char *error_message,
    size_t error_message_size
) {
    size_t text_length = strlen(text);

    if (*length + text_length + 1 > destination_size) {
        beast2_parser_set_error(error_message, error_message_size, "rendered prompt exceeds output buffer");
        return -1;
    }

    memcpy(destination + *length, text, text_length);
    *length += text_length;
    destination[*length] = '\0';
    return 0;
}

void beast2_generator_document_init(beast2_generator_document *document) {
    memset(document, 0, sizeof(*document));
}

void beast2_generator_document_free(beast2_generator_document *document) {
    size_t block_index = 0;
    size_t section_index = 0;
    size_t item_index = 0;

    free(document->source_path);
    free(document->generator_name);

    for (block_index = 0; block_index < document->prompt_block_count; block_index++) {
        beast2_prompt_block *block = &document->prompt_blocks[block_index];

        free(block->name);

        for (section_index = 0; section_index < block->section_count; section_index++) {
            beast2_prompt_section *section = &block->sections[section_index];

            free(section->name);

            for (item_index = 0; item_index < section->snippet_count; item_index++) {
                free(section->snippets[item_index]);
            }

            free(section->snippets);
        }

        free(block->sections);
    }

    for (section_index = 0; section_index < document->metadata_section_count; section_index++) {
        beast2_metadata_section *section = &document->metadata_sections[section_index];

        free(section->name);

        for (item_index = 0; item_index < section->instruction_count; item_index++) {
            free(section->instructions[item_index].name);
            free(section->instructions[item_index].value);
        }

        free(section->instructions);
    }

    for (item_index = 0; item_index < document->warning_count; item_index++) {
        free(document->warnings[item_index]);
    }

    free(document->prompt_blocks);
    free(document->metadata_sections);
    free(document->warnings);
    memset(document, 0, sizeof(*document));
}

int beast2_generator_parse_file(
    beast2_generator_document *document,
    const char *path,
    char *error_message,
    size_t error_message_size
) {
    FILE *file = NULL;
    char line[8192];
    size_t line_number = 0;
    beast2_prompt_block *current_prompt_block = NULL;
    beast2_prompt_section *current_prompt_section = NULL;
    beast2_metadata_section *current_metadata_section = NULL;

    beast2_generator_document_init(document);
    document->source_path = beast2_parser_strdup(path);

    if (document->source_path == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while storing source path");
        return -1;
    }

    file = fopen(path, "r");

    if (file == NULL) {
        snprintf(error_message, error_message_size, "failed to open generator file: %s", path);
        beast2_generator_document_free(document);
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *trimmed = NULL;
        char *token = NULL;
        char *value = NULL;
        char *separator = NULL;

        line_number++;
        trimmed = beast2_parser_trim(line);

        if (*trimmed == '\0' || *trimmed == '#') {
            continue;
        }

        if (*trimmed == '$') {
            char *header_name = beast2_parser_trim(trimmed + 1);

            if (*header_name == '\0') {
                snprintf(error_message, error_message_size, "line %zu: empty section header", line_number);
                fclose(file);
                beast2_generator_document_free(document);
                return -1;
            }

            if (strncmp(header_name, "b2_", 3) != 0) {
                snprintf(
                    error_message,
                    error_message_size,
                    "line %zu: section headers must begin with $b2_",
                    line_number
                );
                fclose(file);
                beast2_generator_document_free(document);
                return -1;
            }

            if (beast2_parser_is_prompt_header(header_name)) {
                current_prompt_block = beast2_parser_add_prompt_block(
                    document,
                    header_name,
                    error_message,
                    error_message_size
                );
                current_prompt_section = NULL;
                current_metadata_section = NULL;
            } else {
                current_metadata_section = beast2_parser_add_metadata_section(
                    document,
                    header_name,
                    error_message,
                    error_message_size
                );
                current_prompt_block = NULL;
                current_prompt_section = NULL;
            }

            if (current_prompt_block == NULL && current_metadata_section == NULL) {
                fclose(file);
                beast2_generator_document_free(document);
                return -1;
            }

            continue;
        }

        if (strncmp(trimmed, "b2_", 3) != 0) {
            snprintf(
                error_message,
                error_message_size,
                "line %zu: every non-empty line must begin with $, #, or b2_",
                line_number
            );
            fclose(file);
            beast2_generator_document_free(document);
            return -1;
        }

        if (current_prompt_block == NULL && current_metadata_section == NULL) {
            snprintf(error_message, error_message_size, "line %zu: instruction outside of a section", line_number);
            fclose(file);
            beast2_generator_document_free(document);
            return -1;
        }

        separator = trimmed;

        while (*separator != '\0' && !isspace((unsigned char) *separator)) {
            separator++;
        }

        if (*separator == '\0') {
            token = trimmed;
            value = separator;
        } else {
            *separator = '\0';
            token = trimmed;
            value = beast2_parser_trim(separator + 1);
        }

        if (current_prompt_block != NULL) {
            char warning[256];

            if (strcmp(token, "b2_section") == 0) {
                if (*value == '\0') {
                    snprintf(error_message, error_message_size, "line %zu: b2_section requires a name", line_number);
                    fclose(file);
                    beast2_generator_document_free(document);
                    return -1;
                }

                current_prompt_section = beast2_parser_add_prompt_section(
                    document,
                    current_prompt_block,
                    value,
                    line_number,
                    error_message,
                    error_message_size
                );

                if (current_prompt_section == NULL) {
                    fclose(file);
                    beast2_generator_document_free(document);
                    return -1;
                }
            } else if (strcmp(token, "b2_snippet") == 0) {
                if (current_prompt_section == NULL) {
                    snprintf(
                        error_message,
                        error_message_size,
                        "line %zu: b2_snippet must appear after a b2_section",
                        line_number
                    );
                    fclose(file);
                    beast2_generator_document_free(document);
                    return -1;
                }

                if (
                    beast2_parser_add_snippet(
                        current_prompt_section,
                        value,
                        error_message,
                        error_message_size
                    ) != 0
                ) {
                    fclose(file);
                    beast2_generator_document_free(document);
                    return -1;
                }
            } else if (strcmp(token, "b2_concat_comma") == 0) {
                current_prompt_block->pending_separator = BEAST2_CONCAT_COMMA;
            } else if (strcmp(token, "b2_concat_space") == 0) {
                current_prompt_block->pending_separator = BEAST2_CONCAT_SPACE;
            } else if (strcmp(token, "b2_concat_newline") == 0) {
                current_prompt_block->pending_separator = BEAST2_CONCAT_NEWLINE;
            } else {
                snprintf(
                    warning,
                    sizeof(warning),
                    "line %zu: ignoring unsupported prompt instruction '%s'",
                    line_number,
                    token
                );

                if (beast2_parser_add_warning(document, warning, error_message, error_message_size) != 0) {
                    fclose(file);
                    beast2_generator_document_free(document);
                    return -1;
                }
            }
        } else if (
            beast2_parser_add_instruction(
                document,
                current_metadata_section,
                token,
                value,
                error_message,
                error_message_size
            ) != 0
        ) {
            fclose(file);
            beast2_generator_document_free(document);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

const beast2_prompt_block *beast2_generator_primary_prompt_block(
    const beast2_generator_document *document
) {
    size_t index = 0;

    for (index = 0; index < document->prompt_block_count; index++) {
        if (strcmp(document->prompt_blocks[index].name, "b2_positive_prompt") == 0) {
            return &document->prompt_blocks[index];
        }
    }

    if (document->prompt_block_count == 0) {
        return NULL;
    }

    return &document->prompt_blocks[0];
}

const beast2_metadata_section *beast2_generator_find_metadata_section(
    const beast2_generator_document *document,
    const char *section_name
) {
    size_t index = 0;

    if (document == NULL || section_name == NULL) {
        return NULL;
    }

    for (index = 0; index < document->metadata_section_count; index++) {
        if (strcmp(document->metadata_sections[index].name, section_name) == 0) {
            return &document->metadata_sections[index];
        }
    }

    return NULL;
}

const char *beast2_metadata_first_value(
    const beast2_metadata_section *section,
    const char *instruction_name
) {
    size_t index = 0;

    if (section == NULL || instruction_name == NULL) {
        return NULL;
    }

    for (index = 0; index < section->instruction_count; index++) {
        if (strcmp(section->instructions[index].name, instruction_name) == 0) {
            return section->instructions[index].value;
        }
    }

    return NULL;
}

size_t beast2_prompt_block_variant_count(const beast2_prompt_block *block) {
    size_t index = 0;
    size_t count = 1;

    if (block == NULL || block->section_count == 0) {
        return 0;
    }

    for (index = 0; index < block->section_count; index++) {
        const beast2_prompt_section *section = &block->sections[index];

        if (section->snippet_count == 0) {
            return 0;
        }

        if (count > ((size_t) -1) / section->snippet_count) {
            return (size_t) -1;
        }

        count *= section->snippet_count;
    }

    return count;
}

const char *beast2_concat_separator(beast2_concat_mode mode) {
    switch (mode) {
        case BEAST2_CONCAT_COMMA:
            return ", ";
        case BEAST2_CONCAT_NEWLINE:
            return "\n";
        case BEAST2_CONCAT_SPACE:
        case BEAST2_CONCAT_NONE:
        default:
            return " ";
    }
}

int beast2_prompt_block_render_variant(
    const beast2_prompt_block *block,
    size_t variant_index,
    char *destination,
    size_t destination_size,
    char *error_message,
    size_t error_message_size
) {
    size_t *snippet_indices = NULL;
    size_t section_index = 0;
    size_t remaining = variant_index;
    size_t rendered_length = 0;
    size_t variant_count = 0;

    if (destination == NULL || destination_size == 0) {
        beast2_parser_set_error(error_message, error_message_size, "render destination is invalid");
        return -1;
    }

    destination[0] = '\0';
    variant_count = beast2_prompt_block_variant_count(block);

    if (variant_count == 0) {
        beast2_parser_set_error(error_message, error_message_size, "prompt block has no renderable variants");
        return -1;
    }

    if (variant_index >= variant_count) {
        beast2_parser_set_error(error_message, error_message_size, "prompt variant index is out of range");
        return -1;
    }

    snippet_indices = (size_t *) malloc(block->section_count * sizeof(*snippet_indices));

    if (snippet_indices == NULL) {
        beast2_parser_set_error(error_message, error_message_size, "out of memory while rendering prompt");
        return -1;
    }

    for (section_index = block->section_count; section_index > 0; section_index--) {
        const beast2_prompt_section *section = &block->sections[section_index - 1];

        snippet_indices[section_index - 1] = remaining % section->snippet_count;
        remaining /= section->snippet_count;
    }

    for (section_index = 0; section_index < block->section_count; section_index++) {
        const beast2_prompt_section *section = &block->sections[section_index];

        if (section_index > 0) {
            if (
                beast2_parser_append_text(
                    destination,
                    destination_size,
                    &rendered_length,
                    beast2_concat_separator(section->separator_before),
                    error_message,
                    error_message_size
                ) != 0
            ) {
                free(snippet_indices);
                return -1;
            }
        }

        if (
            beast2_parser_append_text(
                destination,
                destination_size,
                &rendered_length,
                section->snippets[snippet_indices[section_index]],
                error_message,
                error_message_size
            ) != 0
        ) {
            free(snippet_indices);
            return -1;
        }
    }

    free(snippet_indices);
    return 0;
}
