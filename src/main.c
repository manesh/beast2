#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "beast2/app.h"

static void beast2_print_usage(const char *program_name) {
    printf(
        "Usage: %s [--config <path>] [--generator <path>] [--run-generator <path>] "
        "[--explore-latents] [--latent-a <id>] [--latent-b <id>] [--latent-c <id>] [--latent-d <id>] "
        "[--x <0..1>] [--y <0..1>] [--preview-mode <mode>] "
        "[--all-prompts] [--help] [--version]\n",
        program_name
    );
}

int main(int argc, char **argv) {
    const char *config_path = "config/beast2.conf";
    const char *generator_path = NULL;
    const char *run_generator_path = NULL;
    const char *preview_mode = "decode-only";
    long long latent_ids[4] = {0, 0, 0, 0};
    double explore_x = 0.5;
    double explore_y = 0.5;
    int explore_latents = 0;
    int print_all_prompts = 0;
    int argument_index = 0;

    for (argument_index = 1; argument_index < argc; argument_index++) {
        if (strcmp(argv[argument_index], "--help") == 0 || strcmp(argv[argument_index], "-h") == 0) {
            beast2_print_usage(argv[0]);
            return 0;
        }

        if (strcmp(argv[argument_index], "--version") == 0) {
            printf("beast2 0.11.0\n");
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

        if (strcmp(argv[argument_index], "--run-generator") == 0) {
            argument_index++;

            if (argument_index >= argc) {
                fprintf(stderr, "--run-generator requires a path\n");
                return 1;
            }

            run_generator_path = argv[argument_index];
            continue;
        }

        if (strcmp(argv[argument_index], "--explore-latents") == 0) {
            explore_latents = 1;
            continue;
        }

        if (strcmp(argv[argument_index], "--latent-a") == 0) {
            argument_index++;
            if (argument_index >= argc) {
                fprintf(stderr, "--latent-a requires an id\n");
                return 1;
            }
            latent_ids[0] = strtoll(argv[argument_index], NULL, 10);
            continue;
        }

        if (strcmp(argv[argument_index], "--latent-b") == 0) {
            argument_index++;
            if (argument_index >= argc) {
                fprintf(stderr, "--latent-b requires an id\n");
                return 1;
            }
            latent_ids[1] = strtoll(argv[argument_index], NULL, 10);
            continue;
        }

        if (strcmp(argv[argument_index], "--latent-c") == 0) {
            argument_index++;
            if (argument_index >= argc) {
                fprintf(stderr, "--latent-c requires an id\n");
                return 1;
            }
            latent_ids[2] = strtoll(argv[argument_index], NULL, 10);
            continue;
        }

        if (strcmp(argv[argument_index], "--latent-d") == 0) {
            argument_index++;
            if (argument_index >= argc) {
                fprintf(stderr, "--latent-d requires an id\n");
                return 1;
            }
            latent_ids[3] = strtoll(argv[argument_index], NULL, 10);
            continue;
        }

        if (strcmp(argv[argument_index], "--x") == 0) {
            argument_index++;
            if (argument_index >= argc) {
                fprintf(stderr, "--x requires a numeric value\n");
                return 1;
            }
            explore_x = strtod(argv[argument_index], NULL);
            continue;
        }

        if (strcmp(argv[argument_index], "--y") == 0) {
            argument_index++;
            if (argument_index >= argc) {
                fprintf(stderr, "--y requires a numeric value\n");
                return 1;
            }
            explore_y = strtod(argv[argument_index], NULL);
            continue;
        }

        if (strcmp(argv[argument_index], "--preview-mode") == 0) {
            argument_index++;
            if (argument_index >= argc) {
                fprintf(stderr, "--preview-mode requires a value\n");
                return 1;
            }
            preview_mode = argv[argument_index];
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

    if ((generator_path != NULL && run_generator_path != NULL) || ((generator_path != NULL || run_generator_path != NULL) && explore_latents)) {
        fprintf(stderr, "--generator, --run-generator, and --explore-latents are mutually exclusive\n");
        return 1;
    }

    if (generator_path != NULL) {
        return beast2_run_generator(generator_path, print_all_prompts);
    }

    if (run_generator_path != NULL) {
        if (print_all_prompts) {
            fprintf(stderr, "--all-prompts only applies to --generator\n");
            return 1;
        }

        return beast2_run_generator_execution(config_path, run_generator_path);
    }

    if (explore_latents) {
        if (print_all_prompts) {
            fprintf(stderr, "--all-prompts only applies to --generator\n");
            return 1;
        }

        if (latent_ids[0] == 0 || latent_ids[1] == 0 || latent_ids[2] == 0 || latent_ids[3] == 0) {
            fprintf(stderr, "--explore-latents requires --latent-a/--latent-b/--latent-c/--latent-d\n");
            return 1;
        }

        return beast2_run_latent_explorer(config_path, latent_ids, explore_x, explore_y, preview_mode);
    }

    if (print_all_prompts) {
        fprintf(stderr, "--all-prompts requires --generator\n");
        return 1;
    }

    return beast2_run(config_path);
}
