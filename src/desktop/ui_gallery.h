/*
 * Gallery MVP: scroll region, clipped grid, placeholder texture blit.
 */
#ifndef BEAST2_UI_GALLERY_H
#define BEAST2_UI_GALLERY_H

#include <raylib.h>

void ui_gallery_init(void);
void ui_gallery_shutdown(void);
void ui_gallery_update(float wheel_move);
void ui_gallery_draw(Rectangle panel_bounds);

#endif
