/*
 * Phase 5: run beast2_execute_generator on a background thread; status line for UI.
 */
#ifndef BEAST2_DESKTOP_EXECUTION_H
#define BEAST2_DESKTOP_EXECUTION_H

#include <stddef.h>

void desktop_execution_init(void);
void desktop_execution_shutdown(void);

void desktop_execution_set_paths(const char *config_path, const char *generator_path);

/*
 * Starts execution if idle. Returns 0 if a worker was started, -1 if already running or paths missing.
 */
int desktop_execution_try_start(void);

int desktop_execution_is_running(void);
const char *desktop_execution_status_line(void);

/*
 * Returns 1 once after a run finishes (success or failure) so the UI can refresh the gallery.
 */
int desktop_execution_consume_completed_pulse(void);

#endif
