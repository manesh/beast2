#include <stdio.h>
#include <string.h>

#include "beast2/app.h"

static void beast2_print_usage(const char *program_name) {
    printf("Usage: %s [--config <path>] [--generator <path>] [--all-prompts] [--help] [--version]\n", program_name);
}

int main(int argc, char **argv) {
    const char *config_path = "config/beast2.conf";
    const char *generator_path = NULL;
    int print_all_prompts = 0;
    int argument_index = 0;

    for (argument_index = 1; argument_index < argc; argument_index++) {
        if (strcmp(argv[argument_index], "--help") == 0 || strcmp(argv[argument_index], "-h") == 0) {
            beast2_print_usage(argv[0]);
            return 0;
        }

        if (strcmp(argv[argument_index], "--version") == 0) {
            printf("beast2 0.2.0\n");
            return 0;
        }

        if (strcmp(argv[argument_index], "--config") == 0) {
            argument_index++;

            if (argument_index >= argc) {
                fprintf(stderr, "--config requires a path\n");
                return 1;
            }

            config_path = argv[argument_index];
            continue;
        }

        if (strcmp(argv[argument_index], "--generator") == 0) {
            argument_index++;

            if (argument_index >= argc) {
                fprintf(stderr, "--generator requires a path\n");
                return 1;
            }

            generator_path = argv[argument_index];
            continue;
        }

        if (strcmp(argv[argument_index], "--all-prompts") == 0) {
            print_all_prompts = 1;
            continue;
        }

        fprintf(stderr, "unknown argument: %s\n", argv[argument_index]);
        beast2_print_usage(argv[0]);
        return 1;
    }

    if (generator_path != NULL) {
        return beast2_run_generator(generator_path, print_all_prompts);
    }

    if (print_all_prompts) {
        fprintf(stderr, "--all-prompts requires --generator\n");
        return 1;
    }

    return beast2_run(config_path);
}
