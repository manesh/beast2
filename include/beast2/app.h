#ifndef BEAST2_APP_H
#define BEAST2_APP_H

#include "beast2/executor.h"

int beast2_run(const char *config_path);
int beast2_run_generator(const char *generator_path, int print_all_prompts);
int beast2_run_generator_execution(const char *config_path, const char *generator_path);

/*
 * Run a generator end-to-end (same as --run-generator) and fill summary; no stdout.
 * Used by the desktop shell worker thread.
 */
int beast2_run_generator_with_summary(
    const char *config_path,
    const char *generator_path,
    beast2_execution_summary *summary,
    char *error_message,
    size_t error_message_size
);

#endif
