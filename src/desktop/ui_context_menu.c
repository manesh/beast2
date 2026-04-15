#include "ui_context_menu.h"

#include "beast2/media_library.h"

#include "gallery_model.h"
#include "media_bridge.h"
#include "theme.h"
#include "ui_gallery.h"
#include "ui_selection.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int s_open;
static int s_target_index;
static float s_x;
static float s_y;
static float s_menu_w;
static int s_row_count;

static int s_prompt_open;
static char s_prompt_buf[BEAST2_MEDIA_MAX_TAG_NAME];
static int s_prompt_len;

enum {
    kRowH = 22,
    kMenuPad = 6,
    kMinW = 200,
};

static float ui_context_menu_height(void) {
    return (float) (kMenuPad * 2 + s_row_count * kRowH);
}

static Rectangle ui_context_menu_bounds(void) {
    float h = ui_context_menu_height();
    return (Rectangle){s_x, s_y, s_menu_w, h};
}

static int ui_context_menu_row_at(Vector2 mouse) {
    const Rectangle b = ui_context_menu_bounds();
    float ry;

    if (!CheckCollisionPointRec(mouse, b)) {
        return -1;
    }

    ry = mouse.y - (b.y + (float) kMenuPad);
    if (ry < 0.0f) {
        return -1;
    }

    return (int) (ry / (float) kRowH);
}

struct tag_apply_ctx {
    const char *tag;
};

static void ui_context_menu_apply_tag_one(size_t index, void *user) {
    struct tag_apply_ctx *ctx = (struct tag_apply_ctx *) user;
    char err[256];
    const char *path = gallery_model_full_path(index);

    if (path == NULL || ctx == NULL || ctx->tag == NULL) {
        return;
    }

    (void) media_bridge_add_tag_to_absolute_file(path, ctx->tag, err, sizeof err);
}

static void ui_context_menu_apply_tag(const char *tag_name) {
    struct tag_apply_ctx ctx;
    char err[256];

    ctx.tag = tag_name;

    if (tag_name == NULL || tag_name[0] == '\0') {
        return;
    }

    if (
        ui_selection_count() > 0 &&
        s_target_index >= 0 &&
        (size_t) s_target_index < gallery_model_file_count() &&
        ui_selection_is_selected((size_t) s_target_index)
    ) {
        ui_selection_for_each_selected(ui_context_menu_apply_tag_one, &ctx);
    } else if (s_target_index >= 0 && (size_t) s_target_index < gallery_model_file_count()) {
        (void) media_bridge_add_tag_to_absolute_file(
            gallery_model_full_path((size_t) s_target_index),
            tag_name,
            err,
            sizeof err
        );
    }

    media_bridge_refresh_tag_names();
    ui_gallery_reload_files();
}

static void ui_tag_prompt_begin(void) {
    s_prompt_open = 1;
    s_prompt_buf[0] = '\0';
    s_prompt_len = 0;
}

static void ui_tag_prompt_cancel(void) {
    s_prompt_open = 0;
    s_prompt_buf[0] = '\0';
    s_prompt_len = 0;
}

static void ui_tag_prompt_confirm(void) {
    while (s_prompt_len > 0 && isspace((unsigned char) s_prompt_buf[s_prompt_len - 1])) {
        s_prompt_buf[--s_prompt_len] = '\0';
    }

    if (s_prompt_len > 0) {
        ui_context_menu_apply_tag(s_prompt_buf);
    }

    ui_tag_prompt_cancel();
}

static void ui_tag_prompt_update_keys(void) {
    int key;

    if (!s_prompt_open) {
        return;
    }

    key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key < 127 && s_prompt_len < (int) sizeof(s_prompt_buf) - 1) {
            s_prompt_buf[s_prompt_len++] = (char) key;
            s_prompt_buf[s_prompt_len] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && s_prompt_len > 0) {
        s_prompt_buf[--s_prompt_len] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER)) {
        ui_tag_prompt_confirm();
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        ui_tag_prompt_cancel();
    }
}

static void ui_tag_prompt_draw(void) {
    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();
    Rectangle panel = {(float) sw * 0.5f - 180.0f, (float) sh * 0.5f - 40.0f, 360.0f, 80.0f};

    DrawRectangle(0, 0, sw, sh, (Color){0, 0, 0, 160});
    DrawRectangleRec(panel, BEAST2_UI_COLOR_PANEL);
    DrawRectangleLinesEx(panel, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
    DrawText("New tag name:", (int) panel.x + 10, (int) panel.y + 10, 14, BEAST2_UI_COLOR_TEXT_PRIMARY);
    DrawText(
        s_prompt_buf,
        (int) panel.x + 10,
        (int) panel.y + 36,
        20,
        BEAST2_UI_COLOR_TEXT_ON_CARD
    );
    DrawText(
        "[Enter] apply   [Esc] cancel",
        (int) panel.x + 10,
        (int) panel.y + 58,
        12,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
}

void ui_context_menu_init(void) {
    s_open = 0;
    s_target_index = -1;
    ui_tag_prompt_cancel();
}

void ui_context_menu_open(Vector2 screen_position, int file_index) {
    size_t tag_n = media_bridge_tag_count();
    size_t i;
    float max_tw = 0.0f;

    s_target_index = file_index;
    s_x = screen_position.x;
    s_y = screen_position.y;
    s_row_count = 1 + (int) tag_n + 1;
    media_bridge_refresh_tag_names();
    tag_n = media_bridge_tag_count();
    s_row_count = 1 + (int) tag_n + 1;

    for (i = 0; i < tag_n; i++) {
        const char *name = media_bridge_tag_name(i);
        float w;

        if (name == NULL) {
            continue;
        }

        w = (float) MeasureText(name, 12);
        if (w > max_tw) {
            max_tw = w;
        }
    }

    s_menu_w = max_tw + 80.0f;
    if (s_menu_w < (float) kMinW) {
        s_menu_w = (float) kMinW;
    }

    s_open = 1;
}

void ui_context_menu_close(void) {
    s_open = 0;
    s_target_index = -1;
}

bool ui_context_menu_is_open(void) {
    return s_open != 0 || s_prompt_open != 0;
}

void ui_context_menu_draw(void) {
    size_t i;
    float row_y;

    if (s_prompt_open) {
        ui_tag_prompt_draw();
        return;
    }

    if (!s_open) {
        return;
    }

    {
        const Rectangle b = ui_context_menu_bounds();
        DrawRectangleRec(b, BEAST2_UI_COLOR_PANEL);
        DrawRectangleLinesEx(b, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
    }

    row_y = s_y + (float) kMenuPad;
    DrawText("Tags", (int) (s_x + (float) kMenuPad), (int) row_y, 12, BEAST2_UI_COLOR_TEXT_MUTED);
    row_y += (float) kRowH;

    for (i = 0; i < media_bridge_tag_count(); i++) {
        const char *name = media_bridge_tag_name(i);
        char line[BEAST2_MEDIA_MAX_TAG_NAME + 32];

        if (name == NULL) {
            continue;
        }

        snprintf(line, sizeof line, "  %s", name);
        DrawText(line, (int) (s_x + (float) kMenuPad), (int) row_y, 12, BEAST2_UI_COLOR_TEXT_PRIMARY);
        row_y += (float) kRowH;
    }

    DrawText("  New tag…", (int) (s_x + (float) kMenuPad), (int) row_y, 12, BEAST2_UI_COLOR_TEXT_PRIMARY);
}

bool ui_context_menu_handle_click(Vector2 mouse, bool left_pressed) {
    int r;

    if (s_prompt_open) {
        return true;
    }

    if (!s_open) {
        return false;
    }

    if (!left_pressed) {
        return false;
    }

    r = ui_context_menu_row_at(mouse);
    if (r < 0) {
        ui_context_menu_close();
        return false;
    }

    if (r == 0) {
        return true;
    }

    if (r == s_row_count - 1) {
        ui_context_menu_close();
        ui_tag_prompt_begin();
        return true;
    }

    if (r > 0 && r < s_row_count - 1) {
        size_t ti = (size_t) (r - 1);
        const char *name = media_bridge_tag_name(ti);

        if (name != NULL) {
            ui_context_menu_apply_tag(name);
        }

        ui_context_menu_close();
        return true;
    }

    return true;
}

void ui_context_menu_update_keys(void) {
    ui_tag_prompt_update_keys();

    if (s_prompt_open) {
        return;
    }

    if (s_open && IsKeyPressed(KEY_ESCAPE)) {
        ui_context_menu_close();
    }
}
