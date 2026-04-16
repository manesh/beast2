#include "ui_toast.h"

#include "theme.h"
#include "ui_draw.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>

static char s_msg[640];
static float s_time_left;

void ui_toast_init(void) {
    s_msg[0] = '\0';
    s_time_left = 0.0f;
}

void ui_toast_begin_frame(float delta_seconds) {
    if (s_time_left > 0.0f) {
        s_time_left -= delta_seconds;
        if (s_time_left < 0.0f) {
            s_time_left = 0.0f;
        }
    }
}

void ui_toast_show(const char *message, float duration_sec) {
    if (message == NULL) {
        return;
    }

    snprintf(s_msg, sizeof s_msg, "%s", message);
    s_time_left = duration_sec > 0.1f ? duration_sec : 3.0f;
}

void ui_toast_draw(const Rectangle *above_info_bar, float screen_w) {
    const float pad = 12.0f;
    const float box_h = 36.0f;
    float alpha;
    float y;
    Rectangle box;
    int tw;

    if (s_time_left <= 0.0f || s_msg[0] == '\0') {
        return;
    }

    alpha = fminf(1.0f, s_time_left * 2.0f);
    y = above_info_bar->y - box_h - 8.0f;
    if (y < 40.0f) {
        y = 40.0f;
    }

    tw = beast2_ui_measure_text(s_msg, 13);
    box = (Rectangle){
        floorf((screen_w - (float) tw - pad * 2.0f) * 0.5f),
        y,
        (float) tw + pad * 2.0f,
        box_h
    };

    DrawRectangleRec(box, (Color){32, 28, 40, (unsigned char) (220.0f * alpha)});
    DrawRectangleLinesEx(box, 1.0f, (Color){90, 85, 110, (unsigned char) (255.0f * alpha)});
    beast2_ui_draw_text(
        s_msg,
        box.x + pad,
        box.y + 10.0f,
        13,
        (Color){235, 235, 245, (unsigned char) (255.0f * alpha)}
    );
}
