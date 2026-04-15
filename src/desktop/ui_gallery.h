/*
 * Gallery grid: Phase 3 — columns, selection highlight, folder/tag modes.
 */
#ifndef BEAST2_UI_GALLERY_H
#define BEAST2_UI_GALLERY_H

#include <raylib.h>

void ui_gallery_init(const char *config_path);
void ui_gallery_shutdown(void);
void ui_gallery_update(float wheel_move);
void ui_gallery_draw(Rectangle panel_bounds);

void ui_gallery_reload_files(void);
void ui_gallery_set_column_count(int cols);
int ui_gallery_pick_file_index(Vector2 mouse, Rectangle panel_bounds);

#endif
