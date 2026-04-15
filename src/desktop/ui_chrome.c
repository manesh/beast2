/*
 * Dense toolbar matching prototype gallery header (subset).
 */
#include "ui_chrome.h"

#include "beast2/config.h"
#include "beast2/media_library.h"

#include "desktop_execution.h"
#include "gallery_model.h"
#include "ui_sidebar.h"
#include "media_bridge.h"
#include "theme.h"
#include "ui_layout.h"
#include "ui_selection.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>

void ui_gallery_reload_files(void);
void ui_gallery_set_column_count(int cols);

static int s_columns = 3;
static int s_show_filename = 1;
static int s_folder_view = 1;
static char s_active_tag[BEAST2_MEDIA_MAX_TAG_NAME];

void ui_chrome_init(void) {
    s_columns = 3;
    s_show_filename = 1;
    s_folder_view = 1;
    s_active_tag[0] = '\0';
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

const char *ui_chrome_get_active_tag(void) {
    return s_active_tag;
}

static bool hit_rect(Vector2 p, Rectangle r) {
    return CheckCollisionPointRec(p, r);
}

static void ui_chrome_default_active_tag_if_needed(void) {
    if (s_active_tag[0] != '\0') {
        return;
    }

    if (media_bridge_tag_count() > 0 && media_bridge_tag_name(0) != NULL) {
        snprintf(s_active_tag, sizeof s_active_tag, "%s", media_bridge_tag_name(0));
    }
}

static bool ui_chrome_hit_tag_strip(Vector2 mouse, const Rectangle *strip) {
    size_t i;
    float cx = strip->x + 6.0f + (float) MeasureText("Tags:", 12) + 10.0f;
    const float cy = strip->y + 4.0f;
    const float chip_h = 22.0f;

    for (i = 0; i < media_bridge_tag_count(); i++) {
        const char *name = media_bridge_tag_name(i);
        float text_w;
        float chip_w;

        if (name == NULL) {
            continue;
        }

        text_w = (float) MeasureText(name, 12);
        chip_w = fmaxf(text_w + 20.0f, 48.0f);
        if (cx + chip_w > strip->x + strip->width - 4.0f) {
            break;
        }

        {
            Rectangle chip = {cx, cy, chip_w, chip_h};
            if (hit_rect(mouse, chip)) {
                snprintf(s_active_tag, sizeof s_active_tag, "%s", name);
                ui_gallery_reload_files();
                return true;
            }
        }

        cx += chip_w + 6.0f;
    }

    return true;
}

void ui_chrome_draw(const Beast2UiRootLayout *layout) {
    const Rectangle *t = &layout->toolbar;
    const char *root = gallery_model_browse_root();
    float x = t->x + 6.0f;
    const float y = t->y + 8.0f;
    Rectangle folder_tab = {x, t->y + 4.0f, 56.0f, t->height - 8.0f};
    Rectangle tag_tab = {x + 60.0f, t->y + 4.0f, 48.0f, t->height - 8.0f};
    Rectangle flow_tab = {tag_tab.x + tag_tab.width + 6.0f, t->y + 4.0f, 52.0f, t->height - 8.0f};
    int flow_on = ui_sidebar_is_visible();

    DrawRectangleRec(*t, BEAST2_UI_COLOR_TOOLBAR_BG);
    DrawRectangleLinesEx(*t, 1.0f, BEAST2_UI_COLOR_TOOLBAR_BORDER);

    DrawRectangleRec(folder_tab, s_folder_view ? BEAST2_UI_COLOR_TAB_ACTIVE : BEAST2_UI_COLOR_TAB_INACTIVE);
    DrawRectangleLinesEx(folder_tab, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
    DrawText("Folder", (int) (folder_tab.x + 6), (int) (folder_tab.y + 8), 14, RAYWHITE);

    DrawRectangleRec(tag_tab, !s_folder_view ? BEAST2_UI_COLOR_TAB_ACTIVE : BEAST2_UI_COLOR_TAB_INACTIVE);
    DrawRectangleLinesEx(tag_tab, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
    DrawText("Tag", (int) (tag_tab.x + 10), (int) (tag_tab.y + 8), 14, RAYWHITE);

    DrawRectangleRec(flow_tab, flow_on ? BEAST2_UI_COLOR_TAB_ACTIVE : BEAST2_UI_COLOR_TAB_INACTIVE);
    DrawRectangleLinesEx(flow_tab, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
    DrawText("Flow", (int) (flow_tab.x + 8), (int) (flow_tab.y + 8), 14, RAYWHITE);

    x = flow_tab.x + flow_tab.width + 12.0f;
    {
        char br[BEAST2_MAX_PATH_LENGTH + 32];
        snprintf(br, sizeof br, "Browse: %s", root != NULL ? root : ".");
        DrawText(br, (int) x, (int) y, 14, BEAST2_UI_COLOR_TEXT_MUTED);
    }

    {
        const float refresh_w = 88.0f;
        const float run_w = 54.0f;
        const float col_w = 28.0f;
        float refresh_left = t->x + t->width - refresh_w - col_w * 2.0f - 72.0f - 40.0f;
        Rectangle run_btn = {refresh_left - run_w - 6.0f, t->y + 4.0f, run_w, t->height - 8.0f};
        Rectangle refresh_btn = {refresh_left, t->y + 4.0f, refresh_w, t->height - 8.0f};
        Rectangle col_minus = {refresh_left + refresh_w + 6.0f, t->y + 4.0f, col_w, t->height - 8.0f};
        Rectangle col_plus = {
            refresh_left + refresh_w + 6.0f + col_w + 32.0f,
            t->y + 4.0f,
            col_w,
            t->height - 8.0f
        };
        Rectangle fname_btn = {t->x + t->width - 36.0f, t->y + 4.0f, 32.0f, t->height - 8.0f};
        int running = desktop_execution_is_running();

        DrawRectangleRec(
            run_btn,
            running ? BEAST2_UI_COLOR_TAB_INACTIVE : BEAST2_UI_COLOR_TAB_ACTIVE
        );
        DrawRectangleLinesEx(run_btn, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
        DrawText(
            "Run",
            (int) (run_btn.x + 12),
            (int) (run_btn.y + 8),
            14,
            running ? BEAST2_UI_COLOR_TEXT_MUTED : RAYWHITE
        );

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

    if (!s_folder_view && layout->tag_strip.height > 1.0f) {
        size_t i;
        float cx = layout->tag_strip.x + 6.0f;
        const float cy = layout->tag_strip.y + 4.0f;
        const float chip_h = 22.0f;

        DrawRectangleRec(layout->tag_strip, BEAST2_UI_COLOR_PANEL);
        DrawRectangleLinesEx(layout->tag_strip, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
        DrawText("Tags:", (int) cx, (int) (cy + 3), 12, BEAST2_UI_COLOR_TEXT_MUTED);
        cx += (float) MeasureText("Tags:", 12) + 10.0f;

        for (i = 0; i < media_bridge_tag_count(); i++) {
            const char *name = media_bridge_tag_name(i);
            float text_w;
            float chip_w;
            int active;

            if (name == NULL) {
                continue;
            }

            text_w = (float) MeasureText(name, 12);
            chip_w = fmaxf(text_w + 20.0f, 48.0f);
            if (cx + chip_w > layout->tag_strip.x + layout->tag_strip.width - 4.0f) {
                break;
            }

            active = (strcmp(name, s_active_tag) == 0);
            {
                Rectangle chip = {cx, cy, chip_w, chip_h};
                DrawRectangleRec(chip, active ? BEAST2_UI_COLOR_TAB_ACTIVE : BEAST2_UI_COLOR_TAB_INACTIVE);
                DrawRectangleLinesEx(chip, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
                DrawText(
                    name,
                    (int) (cx + 8.0f),
                    (int) (cy + 4.0f),
                    12,
                    RAYWHITE
                );
            }

            cx += chip_w + 6.0f;
        }
    }
}

bool ui_chrome_handle_click(Vector2 mouse, bool left_pressed, const Beast2UiRootLayout *layout) {
    const Rectangle *t = &layout->toolbar;
    Rectangle folder_tab;
    Rectangle tag_tab;
    Rectangle flow_tab;
    Rectangle refresh_btn;
    Rectangle col_minus;
    Rectangle col_plus;
    Rectangle fname_btn;

    if (!left_pressed) {
        return false;
    }

    if (!s_folder_view && layout->tag_strip.height > 1.0f && CheckCollisionPointRec(mouse, layout->tag_strip)) {
        return ui_chrome_hit_tag_strip(mouse, &layout->tag_strip);
    }

    if (!CheckCollisionPointRec(mouse, *t)) {
        return false;
    }

    folder_tab = (Rectangle){t->x + 6.0f, t->y + 4.0f, 56.0f, t->height - 8.0f};
    tag_tab = (Rectangle){t->x + 66.0f, t->y + 4.0f, 48.0f, t->height - 8.0f};
    flow_tab = (Rectangle){tag_tab.x + tag_tab.width + 6.0f, t->y + 4.0f, 52.0f, t->height - 8.0f};

    if (hit_rect(mouse, folder_tab)) {
        s_folder_view = 1;
        ui_selection_clear();
        ui_gallery_reload_files();
        return true;
    }

    if (hit_rect(mouse, tag_tab)) {
        s_folder_view = 0;
        ui_selection_clear();
        media_bridge_refresh_tag_names();
        ui_chrome_default_active_tag_if_needed();
        ui_gallery_reload_files();
        return true;
    }

    if (hit_rect(mouse, flow_tab)) {
        ui_sidebar_toggle_visible();
        return true;
    }

    {
        const float refresh_w = 88.0f;
        const float run_w = 54.0f;
        const float col_w = 28.0f;
        float refresh_left = t->x + t->width - refresh_w - col_w * 2.0f - 72.0f - 40.0f;
        Rectangle run_btn = {refresh_left - run_w - 6.0f, t->y + 4.0f, run_w, t->height - 8.0f};

        refresh_btn = (Rectangle){refresh_left, t->y + 4.0f, refresh_w, t->height - 8.0f};
        col_minus = (Rectangle){refresh_left + refresh_w + 6.0f, t->y + 4.0f, col_w, t->height - 8.0f};
        col_plus = (Rectangle){
            refresh_left + refresh_w + 6.0f + col_w + 32.0f,
            t->y + 4.0f,
            col_w,
            t->height - 8.0f
        };
        fname_btn = (Rectangle){t->x + t->width - 36.0f, t->y + 4.0f, 32.0f, t->height - 8.0f};

        if (hit_rect(mouse, run_btn)) {
            if (!desktop_execution_is_running()) {
                (void) desktop_execution_try_start();
            }
            return true;
        }
    }

    {
        const float refresh_w = 88.0f;
        const float col_w = 28.0f;
        float refresh_left = t->x + t->width - refresh_w - col_w * 2.0f - 72.0f - 40.0f;

        refresh_btn = (Rectangle){refresh_left, t->y + 4.0f, refresh_w, t->height - 8.0f};
        col_minus = (Rectangle){refresh_left + refresh_w + 6.0f, t->y + 4.0f, col_w, t->height - 8.0f};
        col_plus = (Rectangle){
            refresh_left + refresh_w + 6.0f + col_w + 32.0f,
            t->y + 4.0f,
            col_w,
            t->height - 8.0f
        };
        fname_btn = (Rectangle){t->x + t->width - 36.0f, t->y + 4.0f, 32.0f, t->height - 8.0f};
    }

    if (hit_rect(mouse, refresh_btn)) {
        media_bridge_refresh_tag_names();
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
