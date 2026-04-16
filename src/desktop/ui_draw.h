/*
 * Text drawing with DPI-aware font sizes (Phase 7).
 */
#ifndef BEAST2_UI_DRAW_H
#define BEAST2_UI_DRAW_H

#include <raylib.h>

void beast2_ui_draw_text(const char *text, float x, float y, int font_size_base, Color color);
int beast2_ui_measure_text(const char *text, int font_size_base);

#endif
