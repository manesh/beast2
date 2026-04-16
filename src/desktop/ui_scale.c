#include "ui_scale.h"

#include <raylib.h>

static float s_scale = 1.0f;

void ui_scale_begin_frame(void) {
    Vector2 v = GetWindowScaleDPI();
    float sx = v.x;
    float sy = v.y;

    if (sx < 0.01f) {
        sx = 1.0f;
    }
    if (sy < 0.01f) {
        sy = 1.0f;
    }

    s_scale = (sx + sy) * 0.5f;
    if (s_scale < 0.75f) {
        s_scale = 0.75f;
    }
    if (s_scale > 3.0f) {
        s_scale = 3.0f;
    }
}

float ui_scale_get(void) {
    return s_scale;
}

int ui_scale_font_px(int base_px) {
    int n = (int) ((float) base_px * s_scale + 0.5f);

    if (n < 1) {
        n = 1;
    }
    if (n > 96) {
        n = 96;
    }
    return n;
}

float ui_scale_layout_px(float base_px) {
    float x = base_px * s_scale;

    if (x < 1.0f) {
        x = 1.0f;
    }
    return x;
}
