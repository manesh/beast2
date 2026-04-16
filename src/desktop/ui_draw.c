#include "ui_draw.h"

#include "ui_scale.h"

#include <raylib.h>

void beast2_ui_draw_text(const char *text, float x, float y, int font_size_base, Color color) {
    DrawText(text, (int) x, (int) y, ui_scale_font_px(font_size_base), color);
}

int beast2_ui_measure_text(const char *text, int font_size_base) {
    return MeasureText(text, ui_scale_font_px(font_size_base));
}
