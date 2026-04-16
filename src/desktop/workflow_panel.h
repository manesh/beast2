/*
 * Phase 6: named workflow entries (generator .b2 paths), persisted under workspace db/.
 */
#ifndef BEAST2_WORKFLOW_PANEL_H
#define BEAST2_WORKFLOW_PANEL_H

#include "ui_layout.h"

#include <raylib.h>
#include <stdbool.h>

void workflow_panel_init(const char *workspace_root);
void workflow_panel_shutdown(void);

/*
 * When use_cli_generator is false, sets desktop_execution generator path from the
 * selected workflow (or default example if none).
 */
void workflow_panel_sync_desktop_generator(const char *config_path, int use_cli_generator);

int workflow_panel_is_modal_open(void);

float workflow_panel_scrollable_height(void);

void workflow_panel_draw(
    const Beast2UiRootLayout *layout,
    float scroll_y
);

/*
 * Returns 1 if the click was used (modal, list, or buttons). Call before chrome when modal may be open.
 */
int workflow_panel_handle_click(
    Vector2 mouse,
    bool left_pressed,
    const Beast2UiRootLayout *layout,
    float scroll_y
);

void workflow_panel_update_keys(void);

void workflow_panel_draw_modal_overlay(void);

#endif
