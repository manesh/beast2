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
    out->wheel_for_gallery = 0.0f;
    out->wheel_for_sidebar = 0.0f;

    if (layout->sidebar.width > 0.5f) {
        Rectangle side_zone = {
            layout->sidebar.x - 6.0f,
            layout->sidebar.y,
            layout->sidebar.width + 6.0f,
            layout->sidebar.height
        };

        if (beast2_ui_point_in_rect(out->mouse, side_zone)) {
            out->wheel_for_sidebar = out->wheel_raw;
            return;
        }
    }

    if (beast2_ui_point_in_rect(out->mouse, layout->gallery)) {
        out->hover_id = BEAST2_UI_ID_GALLERY_PANEL;
        out->wheel_for_gallery = out->wheel_raw;
    } else if (beast2_ui_point_in_rect(out->mouse, layout->header)) {
        out->hover_id = BEAST2_UI_ID_HEADER;
    }
}
