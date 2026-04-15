#include "ui_sidebar.h"

#include "theme.h"
#include "ui_draw.h"
#include "workflow_panel.h"

#include <math.h>
#include <string.h>

#include <raylib.h>

#define K_GRIP_W 6
#define K_SIDEBAR_MIN_W 200.0f
#define K_SIDEBAR_MAX_W 480.0f
#define K_SIDEBAR_DEFAULT_W 280.0f

static int s_visible;
static float s_width;
static float s_scroll_y;
static int s_resize_drag;
static float s_drag_start_x;
static float s_width_at_drag_start;

void ui_sidebar_init(void) {
    s_visible = 0;
    s_width = K_SIDEBAR_DEFAULT_W;
    s_scroll_y = 0.0f;
    s_resize_drag = 0;
}

void ui_sidebar_toggle_visible(void) {
    s_visible = !s_visible;
}

int ui_sidebar_is_visible(void) {
    return s_visible;
}

float ui_sidebar_get_width(void) {
    return s_visible ? s_width : 0.0f;
}

float ui_sidebar_get_scroll_y(void) {
    return s_scroll_y;
}

static Rectangle sidebar_grip_rect(const Beast2UiRootLayout *layout) {
    if (layout->sidebar.width < 1.0f) {
        return (Rectangle){0, 0, 0, 0};
    }

    return (Rectangle){
        layout->sidebar.x - (float) K_GRIP_W,
        layout->sidebar.y,
        (float) K_GRIP_W,
        layout->sidebar.height
    };
}

static float sidebar_content_height(void) {
    return workflow_panel_scrollable_height();
}

void ui_sidebar_update_interaction(const Beast2UiRootLayout *layout) {
    Rectangle grip = sidebar_grip_rect(layout);
    Vector2 mouse = GetMousePosition();

    if (!s_visible || layout->sidebar.width < 1.0f) {
        s_resize_drag = 0;
        return;
    }

    if (s_resize_drag) {
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            s_resize_drag = 0;
            return;
        }

        {
            float dx = mouse.x - s_drag_start_x;
            float nw = s_width_at_drag_start - dx;

            if (nw < K_SIDEBAR_MIN_W) {
                nw = K_SIDEBAR_MIN_W;
            }
            if (nw > K_SIDEBAR_MAX_W) {
                nw = K_SIDEBAR_MAX_W;
            }

            s_width = nw;
        }
        return;
    }

    if (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(mouse, grip)
    ) {
        s_resize_drag = 1;
        s_drag_start_x = mouse.x;
        s_width_at_drag_start = s_width;
    }
}

void ui_sidebar_draw(const Beast2UiRootLayout *layout) {
    const Rectangle *s = &layout->sidebar;
    float inner_h;
    float content_h;
    float max_scroll;
    Rectangle grip;

    if (!s_visible || s->width < 1.0f) {
        return;
    }

    grip = sidebar_grip_rect(layout);
    DrawRectangleRec(grip, BEAST2_UI_COLOR_SCROLL_TRACK);
    DrawRectangleLinesEx(grip, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);

    DrawRectangleRec(*s, BEAST2_UI_COLOR_PANEL);
    DrawRectangleLinesEx(*s, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);

    beast2_ui_draw_text("Workflows", s->x + 10.0f, s->y + 8.0f, 16, BEAST2_UI_COLOR_TEXT_PRIMARY);

    inner_h = s->height - 20.0f;
    content_h = sidebar_content_height();
    max_scroll = (content_h > inner_h) ? (content_h - inner_h) : 0.0f;

    if (s_scroll_y < 0.0f) {
        s_scroll_y = 0.0f;
    }
    if (s_scroll_y > max_scroll) {
        s_scroll_y = max_scroll;
    }

    {
        const int sx = (int) floorf(s->x);
        const int sy = (int) floorf(s->y + 36.0f);
        const int sw = (int) floorf(s->width);
        const int sh = (int) floorf(inner_h);

        BeginScissorMode(sx, sy, sw, sh);
    }

    workflow_panel_draw(layout, s_scroll_y);

    EndScissorMode();

    if (max_scroll > 1.0f) {
        const float track_h = inner_h - 8.0f;
        const float thumb_h = fmaxf(20.0f, track_h * (inner_h / content_h));
        const float t = (max_scroll > 0.0f) ? (s_scroll_y / max_scroll) : 0.0f;
        const float thumb_y = s->y + 40.0f + (track_h - thumb_h) * t;
        const float bar_x = s->x + s->width - 8.0f;

        DrawRectangle((int) bar_x, (int) (s->y + 40.0f), 4, (int) track_h, BEAST2_UI_COLOR_SCROLL_TRACK);
        DrawRectangle((int) bar_x, (int) thumb_y, 4, (int) thumb_h, BEAST2_UI_COLOR_SCROLL_THUMB);
    }
}

int ui_sidebar_handle_click(Vector2 mouse, bool left_pressed, const Beast2UiRootLayout *layout) {
    if (!s_visible || layout->sidebar.width < 1.0f) {
        return 0;
    }

    if (!left_pressed) {
        return 0;
    }

    if (CheckCollisionPointRec(mouse, sidebar_grip_rect(layout))) {
        return 1;
    }

    return 0;
}

void ui_sidebar_scroll(float wheel_delta) {
    s_scroll_y -= wheel_delta * 24.0f;
}
