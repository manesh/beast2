#ifndef BEAST2_APP_H
#define BEAST2_APP_H

int beast2_run(const char *config_path);
int beast2_run_generator(const char *generator_path, int print_all_prompts);
int beast2_run_generator_execution(const char *config_path, const char *generator_path);

#endif
