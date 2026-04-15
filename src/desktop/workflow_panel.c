#include "workflow_panel.h"

#include "desktop_execution.h"
#include "theme.h"

#include "beast2/config.h"
#include "beast2/filesystem.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

enum {
    kMaxEntries = 32,
    kNameLen = 96,
    kRowH = 22,
    kBtnH = 24,
    kHintLines = 2,
};

static char s_workspace[BEAST2_MAX_PATH_LENGTH];
static char s_store_path[BEAST2_MAX_PATH_LENGTH];
static char s_names[kMaxEntries][kNameLen];
static char s_paths[kMaxEntries][BEAST2_MAX_PATH_LENGTH];
static size_t s_count;
static size_t s_selected;

static int s_modal_open;
static char s_modal_name[kNameLen];
static char s_modal_path[BEAST2_MAX_PATH_LENGTH];
static int s_modal_field;

static void workflow_modal_try_confirm(void);

static int workflow_try_open(const char *path) {
    FILE *f = fopen(path, "rb");

    if (f == NULL) {
        return -1;
    }

    fclose(f);
    return 0;
}

static int workflow_resolve_for_run(const char *rel, char *out, size_t out_size) {
    char joined[BEAST2_MAX_PATH_LENGTH];

    if (rel == NULL || rel[0] == '\0') {
        return -1;
    }

    if (beast2_fs_is_absolute(rel)) {
        snprintf(out, out_size, "%s", rel);
        return workflow_try_open(out);
    }

    if (workflow_try_open(rel) == 0) {
        snprintf(out, out_size, "%s", rel);
        return 0;
    }

    if (beast2_fs_join_path(joined, sizeof(joined), s_workspace, rel) != 0) {
        return -1;
    }

    if (workflow_try_open(joined) == 0) {
        snprintf(out, out_size, "%s", joined);
        return 0;
    }

    return -1;
}

static void workflow_save(void) {
    FILE *f = NULL;
    size_t i;

    f = fopen(s_store_path, "wb");
    if (f == NULL) {
        return;
    }

    for (i = 0; i < s_count; i++) {
        fprintf(f, "%s|%s\n", s_names[i], s_paths[i]);
    }

    fclose(f);
}

static void workflow_strip_newline(char *line) {
    size_t n = strlen(line);

    while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r')) {
        line[--n] = '\0';
    }
}

static void workflow_load(void) {
    FILE *f = NULL;
    char line[2048];

    s_count = 0;
    s_selected = (size_t) -1;

    f = fopen(s_store_path, "rb");
    if (f == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), f) != NULL && s_count < kMaxEntries) {
        char *sep = NULL;

        workflow_strip_newline(line);
        sep = strchr(line, '|');
        if (sep == NULL) {
            continue;
        }

        *sep = '\0';
        snprintf(s_names[s_count], sizeof(s_names[s_count]), "%s", line);
        snprintf(s_paths[s_count], sizeof(s_paths[s_count]), "%s", sep + 1);
        s_count++;
    }

    fclose(f);

    if (s_count > 0) {
        s_selected = 0;
    }
}

static void workflow_seed_defaults(void) {
    snprintf(s_names[0], sizeof(s_names[0]), "%s", "SDXL example");
    snprintf(s_paths[0], sizeof(s_paths[0]), "%s", "examples/sdxl_character_concept.b2");
    s_count = 1;
    s_selected = 0;
    workflow_save();
}

void workflow_panel_init(const char *workspace_root) {
    snprintf(s_workspace, sizeof s_workspace, "%s", workspace_root != NULL ? workspace_root : ".");

    if (beast2_fs_join_path(s_store_path, sizeof s_store_path, s_workspace, "db/desktop_workflows.txt") != 0) {
        s_store_path[0] = '\0';
    }

    workflow_load();
    if (s_count == 0 && s_store_path[0] != '\0') {
        workflow_seed_defaults();
    }

    s_modal_open = 0;
}

void workflow_panel_shutdown(void) {
}

void workflow_panel_sync_desktop_generator(const char *config_path, int use_cli_generator) {
    char resolved[BEAST2_MAX_PATH_LENGTH];

    if (use_cli_generator) {
        return;
    }

    if (
        s_count > 0 &&
        s_selected != (size_t) -1 &&
        s_selected < s_count &&
        workflow_resolve_for_run(s_paths[s_selected], resolved, sizeof resolved) == 0
    ) {
        desktop_execution_set_paths(config_path, resolved);
    } else {
        desktop_execution_set_paths(config_path, "examples/sdxl_character_concept.b2");
    }
}

int workflow_panel_is_modal_open(void) {
    return s_modal_open;
}

static float workflow_content_height(void) {
    return (float) (kHintLines * kRowH + (int) s_count * kRowH + 6 + kBtnH + 6 + kBtnH + 16);
}

float workflow_panel_scrollable_height(void) {
    return workflow_content_height();
}

static float workflow_content_top(const Beast2UiRootLayout *layout) {
    return layout->sidebar.y + 36.0f;
}

void workflow_panel_draw(const Beast2UiRootLayout *layout, float scroll_y) {
    const Rectangle *s = &layout->sidebar;
    float y;
    size_t i;

    if (s->width < 1.0f) {
        return;
    }

    y = workflow_content_top(layout) - scroll_y;

    DrawText(
        "Select a workflow for Run, or add one.",
        (int) (s->x + 10.0f),
        (int) y,
        11,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
    y += (float) kRowH;
    DrawText(
        "Paths are relative to workspace or cwd.",
        (int) (s->x + 10.0f),
        (int) y,
        11,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
    y += (float) kRowH + 4.0f;

    for (i = 0; i < s_count; i++) {
        Rectangle row = {s->x + 6.0f, y, s->width - 12.0f, (float) kRowH};
        int sel = (i == s_selected);

        DrawRectangleRec(row, sel ? BEAST2_UI_COLOR_TAB_ACTIVE : BEAST2_UI_COLOR_CARD);
        DrawRectangleLinesEx(row, 1.0f, BEAST2_UI_COLOR_CARD_BORDER);
        DrawText(
            s_names[i],
            (int) (row.x + 6.0f),
            (int) (row.y + 4.0f),
            12,
            BEAST2_UI_COLOR_TEXT_ON_CARD
        );
        y += (float) kRowH;
    }

    {
        Rectangle add_btn = {s->x + 8.0f, y + 6.0f, s->width - 16.0f, (float) kBtnH};
        Rectangle rem_btn = {s->x + 8.0f, y + 6.0f + (float) kBtnH + 6.0f, s->width - 16.0f, (float) kBtnH};

        DrawRectangleRec(add_btn, BEAST2_UI_COLOR_TAB_INACTIVE);
        DrawRectangleLinesEx(add_btn, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
        DrawText("Add workflow…", (int) (add_btn.x + 8.0f), (int) (add_btn.y + 5.0f), 12, BEAST2_UI_COLOR_TEXT_PRIMARY);

        DrawRectangleRec(rem_btn, BEAST2_UI_COLOR_TAB_INACTIVE);
        DrawRectangleLinesEx(rem_btn, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
        DrawText("Remove selected", (int) (rem_btn.x + 8.0f), (int) (rem_btn.y + 5.0f), 12, BEAST2_UI_COLOR_TEXT_PRIMARY);
    }
}

static int workflow_hit_add_remove(
    Vector2 mouse,
    bool left_pressed,
    const Beast2UiRootLayout *layout,
    float scroll_y
) {
    const Rectangle *s = &layout->sidebar;
    float y = workflow_content_top(layout) - scroll_y;
    Rectangle add_btn;
    Rectangle rem_btn;

    y += (float) (kHintLines * kRowH + 4);
    y += (float) s_count * (float) kRowH;

    add_btn = (Rectangle){s->x + 8.0f, y + 6.0f, s->width - 16.0f, (float) kBtnH};
    rem_btn = (Rectangle){s->x + 8.0f, y + 6.0f + (float) kBtnH + 6.0f, s->width - 16.0f, (float) kBtnH};

    if (!left_pressed) {
        return 0;
    }

    if (CheckCollisionPointRec(mouse, add_btn)) {
        s_modal_open = 1;
        s_modal_name[0] = '\0';
        s_modal_path[0] = '\0';
        s_modal_field = 0;
        return 1;
    }

    if (CheckCollisionPointRec(mouse, rem_btn)) {
        if (s_count > 0 && s_selected != (size_t) -1 && s_selected < s_count) {
            size_t j;
            for (j = s_selected; j + 1 < s_count; j++) {
                snprintf(s_names[j], sizeof s_names[j], "%s", s_names[j + 1]);
                snprintf(s_paths[j], sizeof s_paths[j], "%s", s_paths[j + 1]);
            }
            s_count--;
            if (s_count == 0) {
                s_selected = (size_t) -1;
            } else if (s_selected >= s_count) {
                s_selected = s_count - 1;
            }
            workflow_save();
        }
        return 1;
    }

    return 0;
}

static int workflow_hit_rows(
    Vector2 mouse,
    bool left_pressed,
    const Beast2UiRootLayout *layout,
    float scroll_y
) {
    const Rectangle *s = &layout->sidebar;
    float y = workflow_content_top(layout) - scroll_y;
    size_t i;

    y += (float) (kHintLines * kRowH + 4);

    if (!left_pressed) {
        return 0;
    }

    for (i = 0; i < s_count; i++) {
        Rectangle row = {s->x + 6.0f, y, s->width - 12.0f, (float) kRowH};

        if (CheckCollisionPointRec(mouse, row)) {
            s_selected = i;
            return 1;
        }

        y += (float) kRowH;
    }

    return 0;
}

static int workflow_modal_handle_click(Vector2 mouse, bool left_pressed) {
    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();
    Rectangle panel = {(float) sw * 0.5f - 220.0f, (float) sh * 0.5f - 120.0f, 440.0f, 240.0f};
    Rectangle ok_btn = {panel.x + 20.0f, panel.y + 190.0f, 90.0f, 32.0f};
    Rectangle cancel_btn = {panel.x + 330.0f, panel.y + 190.0f, 90.0f, 32.0f};

    if (!left_pressed) {
        return 0;
    }

    if (CheckCollisionPointRec(mouse, ok_btn)) {
        workflow_modal_try_confirm();
        return 1;
    }

    if (CheckCollisionPointRec(mouse, cancel_btn)) {
        s_modal_open = 0;
        return 1;
    }

    if (CheckCollisionPointRec(mouse, panel)) {
        return 1;
    }

    s_modal_open = 0;
    return 1;
}

int workflow_panel_handle_click(
    Vector2 mouse,
    bool left_pressed,
    const Beast2UiRootLayout *layout,
    float scroll_y
) {
    if (s_modal_open) {
        return workflow_modal_handle_click(mouse, left_pressed);
    }

    if (layout == NULL || !CheckCollisionPointRec(mouse, layout->sidebar)) {
        return 0;
    }

    if (workflow_hit_add_remove(mouse, left_pressed, layout, scroll_y)) {
        return 1;
    }

    if (workflow_hit_rows(mouse, left_pressed, layout, scroll_y)) {
        return 1;
    }

    return 1;
}

void workflow_panel_update_keys(void) {
    int key;

    if (!s_modal_open) {
        return;
    }

    key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key < 127) {
            if (s_modal_field == 0) {
                size_t n = strlen(s_modal_name);
                if (n < sizeof s_modal_name - 1) {
                    s_modal_name[n] = (char) key;
                    s_modal_name[n + 1] = '\0';
                }
            } else {
                size_t n = strlen(s_modal_path);
                if (n < sizeof s_modal_path - 1) {
                    s_modal_path[n] = (char) key;
                    s_modal_path[n + 1] = '\0';
                }
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_TAB)) {
        s_modal_field = !s_modal_field;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (s_modal_field == 0) {
            size_t n = strlen(s_modal_name);
            if (n > 0) {
                s_modal_name[n - 1] = '\0';
            }
        } else {
            size_t n = strlen(s_modal_path);
            if (n > 0) {
                s_modal_path[n - 1] = '\0';
            }
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        s_modal_open = 0;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        workflow_modal_try_confirm();
    }
}

void workflow_panel_draw_modal_overlay(void) {
    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();
    Rectangle panel = {(float) sw * 0.5f - 220.0f, (float) sh * 0.5f - 120.0f, 440.0f, 240.0f};
    Rectangle ok_btn = {panel.x + 20.0f, panel.y + 190.0f, 90.0f, 32.0f};
    Rectangle cancel_btn = {panel.x + 330.0f, panel.y + 190.0f, 90.0f, 32.0f};

    if (!s_modal_open) {
        return;
    }

    DrawRectangle(0, 0, sw, sh, (Color){0, 0, 0, 170});
    DrawRectangleRec(panel, BEAST2_UI_COLOR_PANEL);
    DrawRectangleLinesEx(panel, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);

    DrawText("New workflow", (int) (panel.x + 16.0f), (int) (panel.y + 12.0f), 16, BEAST2_UI_COLOR_TEXT_PRIMARY);
    DrawText("Name:", (int) (panel.x + 16.0f), (int) (panel.y + 44.0f), 12, BEAST2_UI_COLOR_TEXT_MUTED);
    DrawText(s_modal_name, (int) (panel.x + 16.0f), (int) (panel.y + 60.0f), 14, BEAST2_UI_COLOR_TEXT_ON_CARD);

    DrawText("Generator path (.b2):", (int) (panel.x + 16.0f), (int) (panel.y + 88.0f), 12, BEAST2_UI_COLOR_TEXT_MUTED);
    DrawText(s_modal_path, (int) (panel.x + 16.0f), (int) (panel.y + 104.0f), 14, BEAST2_UI_COLOR_TEXT_ON_CARD);

    DrawRectangleRec(ok_btn, BEAST2_UI_COLOR_TAB_ACTIVE);
    DrawText("OK", (int) (ok_btn.x + 32.0f), (int) (ok_btn.y + 8.0f), 14, RAYWHITE);

    DrawRectangleRec(cancel_btn, BEAST2_UI_COLOR_TAB_INACTIVE);
    DrawText("Cancel", (int) (cancel_btn.x + 22.0f), (int) (cancel_btn.y + 8.0f), 14, RAYWHITE);

    DrawText(
        "[Tab] switch field   [Esc] cancel",
        (int) (panel.x + 16.0f),
        (int) (panel.y + 150.0f),
        11,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
}

static void workflow_modal_try_confirm(void) {
    char test[BEAST2_MAX_PATH_LENGTH];
    char display[kNameLen];

    while (strlen(s_modal_name) > 0 && isspace((unsigned char) s_modal_name[strlen(s_modal_name) - 1])) {
        s_modal_name[strlen(s_modal_name) - 1] = '\0';
    }
    while (strlen(s_modal_path) > 0 && isspace((unsigned char) s_modal_path[strlen(s_modal_path) - 1])) {
        s_modal_path[strlen(s_modal_path) - 1] = '\0';
    }

    if (s_modal_path[0] == '\0') {
        return;
    }

    if (workflow_resolve_for_run(s_modal_path, test, sizeof test) != 0) {
        return;
    }

    snprintf(display, sizeof display, "%s", s_modal_name[0] != '\0' ? s_modal_name : s_modal_path);

    if (s_count < kMaxEntries) {
        snprintf(s_names[s_count], sizeof s_names[s_count], "%s", display);
        snprintf(s_paths[s_count], sizeof s_paths[s_count], "%s", s_modal_path);
        s_selected = s_count;
        s_count++;
        workflow_save();
    }

    s_modal_open = 0;
}
