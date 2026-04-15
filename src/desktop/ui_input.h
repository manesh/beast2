/*
 * Per-frame input snapshot for hit testing and future focus routing.
 */
#ifndef BEAST2_UI_INPUT_H
#define BEAST2_UI_INPUT_H

#include <raylib.h>
#include <stdbool.h>

enum {
    BEAST2_UI_ID_NONE = -1,
    /* Stub ids for Phase 1 — replace with per-widget ids in later phases */
    BEAST2_UI_ID_HEADER = 1,
    BEAST2_UI_ID_GALLERY_PANEL = 2,
};

typedef struct Beast2UiInput {
    Vector2 mouse;
    float wheel_raw;
    float wheel_for_gallery;
    float wheel_for_sidebar;
    int hover_id;
    int focus_id;
    bool left_pressed;
} Beast2UiInput;

struct Beast2UiRootLayout;

void beast2_ui_input_begin_frame(void);
void beast2_ui_input_end_frame(const struct Beast2UiRootLayout *layout, Beast2UiInput *out);

bool beast2_ui_point_in_rect(Vector2 p, Rectangle r);

#endif
