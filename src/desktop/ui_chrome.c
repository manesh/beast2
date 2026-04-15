/*
 * Dense toolbar matching prototype gallery header (subset).
 */
#include "ui_chrome.h"

#include "beast2/config.h"

#include "gallery_model.h"
#include "theme.h"
#include "ui_layout.h"
#include "ui_selection.h"

#include <stdio.h>
#include <string.h>

void ui_gallery_reload_files(void);
void ui_gallery_set_column_count(int cols);

static int s_columns = 3;
static int s_show_filename = 1;
static int s_folder_view = 1;

void ui_chrome_init(void) {
    s_columns = 3;
    s_show_filename = 1;
    s_folder_view = 1;
}

int ui_chrome_get_columns(void) {
    return s_columns;
}

int ui_chrome_show_filename(void) {
    return s_show_filename;
}

int ui_chrome_is_folder_view(void) {
    return s_folder_view;
}

static bool hit_rect(Vector2 p, Rectangle r) {
    return CheckCollisionPointRec(p, r);
}

void ui_chrome_draw(const Beast2UiRootLayout *layout) {
    const Rectangle *t = &layout->toolbar;
    const char *root = gallery_model_browse_root();
    float x = t->x + 6.0f;
    const float y = t->y + 8.0f;
    Rectangle folder_tab = {x, t->y + 4.0f, 56.0f, t->height - 8.0f};
    Rectangle tag_tab = {x + 60.0f, t->y + 4.0f, 48.0f, t->height - 8.0f};

    DrawRectangleRec(*t, BEAST2_UI_COLOR_TOOLBAR_BG);
    DrawRectangleLinesEx(*t, 1.0f, BEAST2_UI_COLOR_TOOLBAR_BORDER);

    DrawRectangleRec(folder_tab, s_folder_view ? BEAST2_UI_COLOR_TAB_ACTIVE : BEAST2_UI_COLOR_TAB_INACTIVE);
    DrawRectangleLinesEx(folder_tab, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
    DrawText("Folder", (int) (folder_tab.x + 6), (int) (folder_tab.y + 8), 14, RAYWHITE);

    DrawRectangleRec(tag_tab, !s_folder_view ? BEAST2_UI_COLOR_TAB_ACTIVE : BEAST2_UI_COLOR_TAB_INACTIVE);
    DrawRectangleLinesEx(tag_tab, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
    DrawText("Tag", (int) (tag_tab.x + 10), (int) (tag_tab.y + 8), 14, RAYWHITE);

    x = tag_tab.x + tag_tab.width + 14.0f;
    {
        char br[BEAST2_MAX_PATH_LENGTH + 32];
        snprintf(br, sizeof br, "Browse: %s", root != NULL ? root : ".");
        DrawText(br, (int) x, (int) y, 14, BEAST2_UI_COLOR_TEXT_MUTED);
    }

    {
        const float refresh_w = 88.0f;
        const float col_w = 28.0f;
        float rx = t->x + t->width - refresh_w - col_w * 2.0f - 72.0f - 40.0f;
        Rectangle refresh_btn = {rx, t->y + 4.0f, refresh_w, t->height - 8.0f};
        Rectangle col_minus = {rx + refresh_w + 6.0f, t->y + 4.0f, col_w, t->height - 8.0f};
        Rectangle col_plus = {rx + refresh_w + 6.0f + col_w + 32.0f, t->y + 4.0f, col_w, t->height - 8.0f};
        Rectangle fname_btn = {t->x + t->width - 36.0f, t->y + 4.0f, 32.0f, t->height - 8.0f};

        DrawRectangleRec(refresh_btn, BEAST2_UI_COLOR_TAB_INACTIVE);
        DrawRectangleLinesEx(refresh_btn, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
        DrawText("Refresh", (int) (refresh_btn.x + 8), (int) (refresh_btn.y + 8), 14, RAYWHITE);

        DrawRectangleRec(col_minus, BEAST2_UI_COLOR_TAB_INACTIVE);
        DrawText("<", (int) (col_minus.x + 8), (int) (col_minus.y + 6), 16, RAYWHITE);

        {
            char nc[16];
            snprintf(nc, sizeof nc, "%d", s_columns);
            DrawText(nc, (int) (col_minus.x + col_w + 6.0f), (int) (col_minus.y + 8), 14, BEAST2_UI_COLOR_TEXT_PRIMARY);
        }

        DrawRectangleRec(col_plus, BEAST2_UI_COLOR_TAB_INACTIVE);
        DrawText(">", (int) (col_plus.x + 8), (int) (col_plus.y + 6), 16, RAYWHITE);

        DrawRectangleRec(fname_btn, s_show_filename ? BEAST2_UI_COLOR_TAB_ACTIVE : BEAST2_UI_COLOR_TAB_INACTIVE);
        DrawRectangleLinesEx(fname_btn, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
        DrawText("F", (int) (fname_btn.x + 10), (int) (fname_btn.y + 8), 14, RAYWHITE);
    }
}

bool ui_chrome_handle_click(Vector2 mouse, bool left_pressed, const Beast2UiRootLayout *layout) {
    const Rectangle *t = &layout->toolbar;
    float rx;
    Rectangle folder_tab;
    Rectangle tag_tab;
    Rectangle refresh_btn;
    Rectangle col_minus;
    Rectangle col_plus;
    Rectangle fname_btn;

    if (!left_pressed) {
        return false;
    }

    if (!CheckCollisionPointRec(mouse, *t)) {
        return false;
    }

    folder_tab = (Rectangle){t->x + 6.0f, t->y + 4.0f, 56.0f, t->height - 8.0f};
    tag_tab = (Rectangle){t->x + 66.0f, t->y + 4.0f, 48.0f, t->height - 8.0f};

    if (hit_rect(mouse, folder_tab)) {
        s_folder_view = 1;
        ui_selection_clear();
        return true;
    }

    if (hit_rect(mouse, tag_tab)) {
        s_folder_view = 0;
        ui_selection_clear();
        return true;
    }

    rx = t->x + t->width - 88.0f - 28.0f * 2.0f - 72.0f - 40.0f;
    refresh_btn = (Rectangle){rx, t->y + 4.0f, 88.0f, t->height - 8.0f};
    col_minus = (Rectangle){rx + 88.0f + 6.0f, t->y + 4.0f, 28.0f, t->height - 8.0f};
    col_plus = (Rectangle){rx + 88.0f + 6.0f + 28.0f + 32.0f, t->y + 4.0f, 28.0f, t->height - 8.0f};
    fname_btn = (Rectangle){t->x + t->width - 36.0f, t->y + 4.0f, 32.0f, t->height - 8.0f};

    if (hit_rect(mouse, refresh_btn)) {
        ui_gallery_reload_files();
        return true;
    }

    if (hit_rect(mouse, col_minus)) {
        if (s_columns > 2) {
            s_columns--;
            ui_gallery_set_column_count(s_columns);
        }
        return true;
    }

    if (hit_rect(mouse, col_plus)) {
        if (s_columns < 8) {
            s_columns++;
            ui_gallery_set_column_count(s_columns);
        }
        return true;
    }

    if (hit_rect(mouse, fname_btn)) {
        s_show_filename = !s_show_filename;
        return true;
    }

    return true;
}
