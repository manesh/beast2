#include <stdio.h>
#include <string.h>

#include "beast2/app.h"

static void beast2_print_usage(const char *program_name) {
    printf("Usage: %s [--config <path>] [--help] [--version]\n", program_name);
}

int main(int argc, char **argv) {
    const char *config_path = "config/beast2.conf";
    int argument_index = 0;

    for (argument_index = 1; argument_index < argc; argument_index++) {
        if (strcmp(argv[argument_index], "--help") == 0 || strcmp(argv[argument_index], "-h") == 0) {
            beast2_print_usage(argv[0]);
            return 0;
        }

        if (strcmp(argv[argument_index], "--version") == 0) {
            printf("beast2 0.1.0\n");
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

        fprintf(stderr, "unknown argument: %s\n", argv[argument_index]);
        beast2_print_usage(argv[0]);
        return 1;
    }

    return beast2_run(config_path);
}
