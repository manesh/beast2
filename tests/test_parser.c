#include <string.h>

#include "beast2/parser.h"
#include "test_support.h"

static int test_parse_example_generator(void) {
    beast2_generator_document document;
    const beast2_prompt_block *block = NULL;
    char error_message[512];
    char prompt[1024];
    int status = 0;

    beast2_generator_document_init(&document);

    if (
        beast2_generator_parse_file(
            &document,
            BEAST2_TEST_SOURCE_DIR "/examples/wan22_walk_cycle.b2",
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        return 1;
    }

    BEAST2_TEST_ASSERT(document.generator_name != NULL);
    BEAST2_TEST_ASSERT_STRING_EQ(document.generator_name, "wan22_walk_cycle_demo");
    BEAST2_TEST_ASSERT(document.prompt_block_count == 1);
    BEAST2_TEST_ASSERT(document.metadata_section_count == 3);
    BEAST2_TEST_ASSERT(document.warning_count == 0);

    block = beast2_generator_primary_prompt_block(&document);
    BEAST2_TEST_ASSERT(block != NULL);
    BEAST2_TEST_ASSERT(block->section_count == 3);
    BEAST2_TEST_ASSERT(beast2_prompt_block_variant_count(block) == 8);

    BEAST2_TEST_ASSERT(
        beast2_prompt_block_render_variant(
            block,
            0,
            prompt,
            sizeof(prompt),
            error_message,
            sizeof(error_message)
        ) == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(prompt, "Subject: a warrior walking, Style: cinematic realism jerky motion");

    BEAST2_TEST_ASSERT(
        beast2_prompt_block_render_variant(
            block,
            7,
            prompt,
            sizeof(prompt),
            error_message,
            sizeof(error_message)
        ) == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(prompt, "Subject: a knight walking, Style: pixel art animation smooth motion");

    beast2_generator_document_free(&document);
    return status;
}

static int test_newline_concat_rendering(void) {
    beast2_generator_document document;
    const beast2_prompt_block *block = NULL;
    char error_message[512];
    char prompt[1024];

    beast2_generator_document_init(&document);

    if (
        beast2_generator_parse_file(
            &document,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/newline_prompt.b2",
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        return 1;
    }

    block = beast2_generator_primary_prompt_block(&document);
    BEAST2_TEST_ASSERT(block != NULL);
    BEAST2_TEST_ASSERT(beast2_prompt_block_variant_count(block) == 1);
    BEAST2_TEST_ASSERT(
        beast2_prompt_block_render_variant(
            block,
            0,
            prompt,
            sizeof(prompt),
            error_message,
            sizeof(error_message)
        ) == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(prompt, "line one\nline two");

    beast2_generator_document_free(&document);
    return 0;
}

static int test_missing_concat_warning(void) {
    beast2_generator_document document;
    const beast2_prompt_block *block = NULL;
    char error_message[512];
    char prompt[1024];

    beast2_generator_document_init(&document);

    if (
        beast2_generator_parse_file(
            &document,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/missing_concat_warning.b2",
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        return 1;
    }

    BEAST2_TEST_ASSERT(document.warning_count == 1);
    BEAST2_TEST_ASSERT(strstr(document.warnings[0], "missing concat instruction") != NULL);
    block = beast2_generator_primary_prompt_block(&document);
    BEAST2_TEST_ASSERT(block != NULL);
    BEAST2_TEST_ASSERT(
        beast2_prompt_block_render_variant(
            block,
            0,
            prompt,
            sizeof(prompt),
            error_message,
            sizeof(error_message)
        ) == 0
    );
    BEAST2_TEST_ASSERT_STRING_EQ(prompt, "alpha beta");

    beast2_generator_document_free(&document);
    return 0;
}

static int test_unsupported_instruction_warning(void) {
    beast2_generator_document document;
    char error_message[512];

    beast2_generator_document_init(&document);

    if (
        beast2_generator_parse_file(
            &document,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/valid/unsupported_instruction_warning.b2",
            error_message,
            sizeof(error_message)
        ) != 0
    ) {
        return 1;
    }

    BEAST2_TEST_ASSERT(document.warning_count == 1);
    BEAST2_TEST_ASSERT(strstr(document.warnings[0], "unsupported prompt instruction") != NULL);
    BEAST2_TEST_ASSERT(strstr(document.warnings[0], "b2_style") != NULL);

    beast2_generator_document_free(&document);
    return 0;
}

static int test_invalid_line_prefix_fails(void) {
    beast2_generator_document document;
    char error_message[512];

    beast2_generator_document_init(&document);

    BEAST2_TEST_ASSERT(
        beast2_generator_parse_file(
            &document,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/invalid/bad_line_prefix.b2",
            error_message,
            sizeof(error_message)
        ) != 0
    );
    BEAST2_TEST_ASSERT(strstr(error_message, "must begin with $, #, or b2_") != NULL);

    return 0;
}

static int test_snippet_before_section_fails(void) {
    beast2_generator_document document;
    char error_message[512];

    beast2_generator_document_init(&document);

    BEAST2_TEST_ASSERT(
        beast2_generator_parse_file(
            &document,
            BEAST2_TEST_SOURCE_DIR "/tests/fixtures/invalid/snippet_before_section.b2",
            error_message,
            sizeof(error_message)
        ) != 0
    );
    BEAST2_TEST_ASSERT(strstr(error_message, "b2_snippet must appear after a b2_section") != NULL);

    return 0;
}

int main(void) {
    if (test_parse_example_generator() != 0) {
        return 1;
    }

    if (test_newline_concat_rendering() != 0) {
        return 1;
    }

    if (test_missing_concat_warning() != 0) {
        return 1;
    }

    if (test_unsupported_instruction_warning() != 0) {
        return 1;
    }

    if (test_invalid_line_prefix_fails() != 0) {
        return 1;
    }

    if (test_snippet_before_section_fails() != 0) {
        return 1;
    }

    return 0;
}
