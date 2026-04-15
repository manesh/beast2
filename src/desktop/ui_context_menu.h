/*
 * Right-click context menu (tags) for the gallery.
 */
#ifndef BEAST2_UI_CONTEXT_MENU_H
#define BEAST2_UI_CONTEXT_MENU_H

#include <raylib.h>
#include <stdbool.h>

void ui_context_menu_init(void);
void ui_context_menu_open(Vector2 screen_position, int file_index);
void ui_context_menu_close(void);
bool ui_context_menu_is_open(void);

void ui_context_menu_draw(void);
/*
 * Returns true if the menu (or tag prompt) consumed this click.
 */
bool ui_context_menu_handle_click(Vector2 mouse, bool left_pressed);

void ui_context_menu_update_keys(void);

#endif
