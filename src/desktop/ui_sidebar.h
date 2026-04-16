/*
 * Phase 6: workflow sidebar host (dock, resize, scroll region; placeholder content).
 */
#ifndef BEAST2_UI_SIDEBAR_H
#define BEAST2_UI_SIDEBAR_H

#include "ui_layout.h"

#include <raylib.h>
#include <stdbool.h>

void ui_sidebar_init(void);

void ui_sidebar_toggle_visible(void);
int ui_sidebar_is_visible(void);
float ui_sidebar_get_width(void);
float ui_sidebar_get_scroll_y(void);

void ui_sidebar_update_interaction(const Beast2UiRootLayout *layout);

void ui_sidebar_draw(const Beast2UiRootLayout *layout);

/*
 * Returns 1 if the click was consumed (resize grip or sidebar body).
 */
int ui_sidebar_handle_click(Vector2 mouse, bool left_pressed, const Beast2UiRootLayout *layout);

void ui_sidebar_scroll(float wheel_delta);

#endif
