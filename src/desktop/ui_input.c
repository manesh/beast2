/*
 * Centralize raylib input reads and map to layout rects / hover ids.
 */
#include "ui_input.h"

#include "ui_layout.h"

bool beast2_ui_point_in_rect(Vector2 p, Rectangle r) {
    return CheckCollisionPointRec(p, r);
}

void beast2_ui_input_begin_frame(void) {
    /* Reserved for debouncing / gesture state later */
}

void beast2_ui_input_end_frame(const Beast2UiRootLayout *layout, Beast2UiInput *out) {
    out->mouse = GetMousePosition();
    out->wheel_raw = GetMouseWheelMove();
    out->left_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    out->focus_id = BEAST2_UI_ID_NONE;
    out->hover_id = BEAST2_UI_ID_NONE;

    if (beast2_ui_point_in_rect(out->mouse, layout->gallery)) {
        out->hover_id = BEAST2_UI_ID_GALLERY_PANEL;
        out->wheel_for_gallery = out->wheel_raw;
    } else {
        out->wheel_for_gallery = 0.0f;
        if (beast2_ui_point_in_rect(out->mouse, layout->header)) {
            out->hover_id = BEAST2_UI_ID_HEADER;
        }
    }
}
