/*
 * HiDPI: window DPI scale from raylib (Phase 7).
 */
#ifndef BEAST2_UI_SCALE_H
#define BEAST2_UI_SCALE_H

void ui_scale_begin_frame(void);
float ui_scale_get(void);
int ui_scale_font_px(int base_px);
float ui_scale_layout_px(float base_px);

#endif
