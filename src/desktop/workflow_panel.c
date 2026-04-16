#include "workflow_panel.h"

#include "desktop_execution.h"
#include "theme.h"
#include "ui_draw.h"

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
    kHintLines = 3,
    kWorkflowBtnCount = 5,
};

static char s_workspace[BEAST2_MAX_PATH_LENGTH];
static char s_store_path[BEAST2_MAX_PATH_LENGTH];
static char s_names[kMaxEntries][kNameLen];
static char s_paths[kMaxEntries][BEAST2_MAX_PATH_LENGTH];
static size_t s_count;
static size_t s_selected;

static int s_modal_open;
static int s_modal_edit_index;
static char s_modal_name[kNameLen];
static char s_modal_path[BEAST2_MAX_PATH_LENGTH];
static int s_modal_field;

static void workflow_modal_try_confirm(void);
static int workflow_resolve_for_run(const char *rel, char *out, size_t out_size);

static void workflow_trim(char *s) {
    size_t n;
    char *p = s;

    while (*p != '\0' && isspace((unsigned char) *p)) {
        p++;
    }

    if (p != s) {
        memmove(s, p, strlen(p) + 1);
    }

    n = strlen(s);
    while (n > 0 && isspace((unsigned char) s[n - 1])) {
        s[--n] = '\0';
    }
}

static int workflow_spec_step_count_display(const char *spec) {
    int n = 0;
    const char *p;
    int in_seg = 0;

    if (spec == NULL || spec[0] == '\0') {
        return 0;
    }

    for (p = spec; *p != '\0'; p++) {
        if (*p == ';') {
            if (in_seg) {
                n++;
            }
            in_seg = 0;
        } else if (!isspace((unsigned char) *p)) {
            in_seg = 1;
        }
    }
    if (in_seg) {
        n++;
    }
    return n > 0 ? n : 1;
}

static int workflow_split_resolve(
    const char *spec,
    char resolved[BEAST2_DESKTOP_PIPELINE_MAX_STEPS][BEAST2_MAX_PATH_LENGTH],
    int *out_count
) {
    char buf[BEAST2_MAX_PATH_LENGTH * 2];
    const char *p;
    int n = 0;

    *out_count = 0;

    if (spec == NULL || spec[0] == '\0') {
        return -1;
    }

    snprintf(buf, sizeof buf, "%s", spec);
    p = buf;

    while (*p != '\0' && n < BEAST2_DESKTOP_PIPELINE_MAX_STEPS) {
        char seg[BEAST2_MAX_PATH_LENGTH];
        size_t k = 0;

        while (*p != '\0' && *p != ';' && k + 1 < sizeof seg) {
            seg[k++] = *p++;
        }
        seg[k] = '\0';
        if (*p == ';') {
            p++;
        }

        workflow_trim(seg);
        if (seg[0] == '\0') {
            continue;
        }

        if (workflow_resolve_for_run(seg, resolved[n], BEAST2_MAX_PATH_LENGTH) != 0) {
            return -1;
        }
        n++;
    }

    if (n < 1) {
        return -1;
    }

    *out_count = n;
    return 0;
}

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
    char line[16384];

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
    s_modal_edit_index = -1;
}

void workflow_panel_shutdown(void) {
}

void workflow_panel_sync_desktop_generator(const char *config_path, int use_cli_generator) {
    char resolved[BEAST2_DESKTOP_PIPELINE_MAX_STEPS][BEAST2_MAX_PATH_LENGTH];
    const char *ptrs[BEAST2_DESKTOP_PIPELINE_MAX_STEPS];
    int n;
    int i;

    if (use_cli_generator) {
        return;
    }

    if (
        s_count > 0 &&
        s_selected != (size_t) -1 &&
        s_selected < s_count &&
        workflow_split_resolve(s_paths[s_selected], resolved, &n) == 0
    ) {
        for (i = 0; i < n; i++) {
            ptrs[i] = resolved[i];
        }
        desktop_execution_set_paths_pipeline(config_path, ptrs, n);
    } else {
        desktop_execution_set_paths(config_path, "examples/sdxl_character_concept.b2");
    }
}

int workflow_panel_is_modal_open(void) {
    return s_modal_open;
}

static float workflow_content_height(void) {
    return (float) (
        kHintLines * kRowH + (int) s_count * kRowH + 4 + kWorkflowBtnCount * (kBtnH + 6) + 16
    );
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

    beast2_ui_draw_text(
        "Select a workflow for Run, or add one.",
        s->x + 10.0f,
        y,
        11,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
    y += (float) kRowH;
    beast2_ui_draw_text(
        "Paths: workspace / cwd. Pipeline: a.b2;b2.b2",
        s->x + 10.0f,
        y,
        11,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
    y += (float) kRowH;
    beast2_ui_draw_text(
        "Edit / reorder below.",
        s->x + 10.0f,
        y,
        11,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
    y += (float) kRowH + 4.0f;

    for (i = 0; i < s_count; i++) {
        Rectangle row = {s->x + 6.0f, y, s->width - 12.0f, (float) kRowH};
        int sel = (i == s_selected);
        int steps = workflow_spec_step_count_display(s_paths[i]);
        char badge[24];

        DrawRectangleRec(row, sel ? BEAST2_UI_COLOR_TAB_ACTIVE : BEAST2_UI_COLOR_CARD);
        DrawRectangleLinesEx(row, 1.0f, BEAST2_UI_COLOR_CARD_BORDER);
        beast2_ui_draw_text(
            s_names[i],
            row.x + 6.0f,
            row.y + 4.0f,
            12,
            BEAST2_UI_COLOR_TEXT_ON_CARD
        );
        if (steps > 1) {
            snprintf(badge, sizeof badge, "(%d)", steps);
            beast2_ui_draw_text(
                badge,
                row.x + row.width - 44.0f,
                row.y + 4.0f,
                11,
                BEAST2_UI_COLOR_TEXT_MUTED
            );
        }
        y += (float) kRowH;
    }

    {
        float by = y + 6.0f;
        int b;
        const char *labels[] = {
            "Add workflow…",
            "Remove selected",
            "Move up",
            "Move down",
            "Edit selected…",
        };

        for (b = 0; b < kWorkflowBtnCount; b++) {
            Rectangle btn = {s->x + 8.0f, by, s->width - 16.0f, (float) kBtnH};

            DrawRectangleRec(btn, BEAST2_UI_COLOR_TAB_INACTIVE);
            DrawRectangleLinesEx(btn, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);
            beast2_ui_draw_text(labels[b], btn.x + 8.0f, btn.y + 5.0f, 12, BEAST2_UI_COLOR_TEXT_PRIMARY);
            by += (float) kBtnH + 6.0f;
        }
    }
}

static int workflow_hit_buttons(
    Vector2 mouse,
    bool left_pressed,
    const Beast2UiRootLayout *layout,
    float scroll_y
) {
    const Rectangle *s = &layout->sidebar;
    float y = workflow_content_top(layout) - scroll_y;
    float by;
    int b;

    y += (float) (kHintLines * kRowH + 4);
    y += (float) s_count * (float) kRowH;
    by = y + 6.0f;

    if (!left_pressed) {
        return 0;
    }

    for (b = 0; b < kWorkflowBtnCount; b++) {
        Rectangle btn = {s->x + 8.0f, by, s->width - 16.0f, (float) kBtnH};

        if (CheckCollisionPointRec(mouse, btn)) {
            if (b == 0) {
                s_modal_open = 1;
                s_modal_edit_index = -1;
                s_modal_name[0] = '\0';
                s_modal_path[0] = '\0';
                s_modal_field = 0;
            } else if (b == 1) {
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
            } else if (b == 2) {
                if (s_count > 0 && s_selected > 0 && s_selected < s_count) {
                    char tn[kNameLen];
                    char tp[BEAST2_MAX_PATH_LENGTH];
                    size_t i = s_selected;
                    snprintf(tn, sizeof tn, "%s", s_names[i]);
                    snprintf(tp, sizeof tp, "%s", s_paths[i]);
                    snprintf(s_names[i], sizeof s_names[i], "%s", s_names[i - 1]);
                    snprintf(s_paths[i], sizeof s_paths[i], "%s", s_paths[i - 1]);
                    snprintf(s_names[i - 1], sizeof s_names[i - 1], "%s", tn);
                    snprintf(s_paths[i - 1], sizeof s_paths[i - 1], "%s", tp);
                    s_selected = i - 1;
                    workflow_save();
                }
            } else if (b == 3) {
                if (s_count > 0 && s_selected != (size_t) -1 && s_selected + 1 < s_count) {
                    char tn[kNameLen];
                    char tp[BEAST2_MAX_PATH_LENGTH];
                    size_t i = s_selected;
                    snprintf(tn, sizeof tn, "%s", s_names[i]);
                    snprintf(tp, sizeof tp, "%s", s_paths[i]);
                    snprintf(s_names[i], sizeof s_names[i], "%s", s_names[i + 1]);
                    snprintf(s_paths[i], sizeof s_paths[i], "%s", s_paths[i + 1]);
                    snprintf(s_names[i + 1], sizeof s_names[i + 1], "%s", tn);
                    snprintf(s_paths[i + 1], sizeof s_paths[i + 1], "%s", tp);
                    s_selected = i + 1;
                    workflow_save();
                }
            } else if (b == 4) {
                if (s_count > 0 && s_selected != (size_t) -1 && s_selected < s_count) {
                    s_modal_open = 1;
                    s_modal_edit_index = (int) s_selected;
                    snprintf(s_modal_name, sizeof s_modal_name, "%s", s_names[s_selected]);
                    snprintf(s_modal_path, sizeof s_modal_path, "%s", s_paths[s_selected]);
                    s_modal_field = 0;
                }
            }
            return 1;
        }

        by += (float) kBtnH + 6.0f;
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
        s_modal_edit_index = -1;
        return 1;
    }

    if (CheckCollisionPointRec(mouse, panel)) {
        return 1;
    }

    s_modal_open = 0;
    s_modal_edit_index = -1;
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

    if (workflow_hit_buttons(mouse, left_pressed, layout, scroll_y)) {
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
        s_modal_edit_index = -1;
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

    beast2_ui_draw_text(
        s_modal_edit_index >= 0 ? "Edit workflow" : "New workflow",
        panel.x + 16.0f,
        panel.y + 12.0f,
        16,
        BEAST2_UI_COLOR_TEXT_PRIMARY
    );
    beast2_ui_draw_text("Name:", panel.x + 16.0f, panel.y + 44.0f, 12, BEAST2_UI_COLOR_TEXT_MUTED);
    beast2_ui_draw_text(s_modal_name, panel.x + 16.0f, panel.y + 60.0f, 14, BEAST2_UI_COLOR_TEXT_ON_CARD);

    beast2_ui_draw_text("Path(s) (.b2), use ; for pipeline:", panel.x + 16.0f, panel.y + 88.0f, 12, BEAST2_UI_COLOR_TEXT_MUTED);
    beast2_ui_draw_text(s_modal_path, panel.x + 16.0f, panel.y + 104.0f, 14, BEAST2_UI_COLOR_TEXT_ON_CARD);

    DrawRectangleRec(ok_btn, BEAST2_UI_COLOR_TAB_ACTIVE);
    beast2_ui_draw_text("OK", ok_btn.x + 32.0f, ok_btn.y + 8.0f, 14, RAYWHITE);

    DrawRectangleRec(cancel_btn, BEAST2_UI_COLOR_TAB_INACTIVE);
    beast2_ui_draw_text("Cancel", cancel_btn.x + 22.0f, cancel_btn.y + 8.0f, 14, RAYWHITE);

    beast2_ui_draw_text(
        "[Tab] switch field   [Esc] cancel",
        panel.x + 16.0f,
        panel.y + 150.0f,
        11,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
}

static void workflow_modal_try_confirm(void) {
    char resolved[BEAST2_DESKTOP_PIPELINE_MAX_STEPS][BEAST2_MAX_PATH_LENGTH];
    char display[kNameLen];
    int n;

    while (strlen(s_modal_name) > 0 && isspace((unsigned char) s_modal_name[strlen(s_modal_name) - 1])) {
        s_modal_name[strlen(s_modal_name) - 1] = '\0';
    }
    while (strlen(s_modal_path) > 0 && isspace((unsigned char) s_modal_path[strlen(s_modal_path) - 1])) {
        s_modal_path[strlen(s_modal_path) - 1] = '\0';
    }

    if (s_modal_path[0] == '\0') {
        return;
    }

    if (workflow_split_resolve(s_modal_path, resolved, &n) != 0) {
        return;
    }

    snprintf(display, sizeof display, "%s", s_modal_name[0] != '\0' ? s_modal_name : s_modal_path);

    if (s_modal_edit_index >= 0 && s_modal_edit_index < (int) s_count) {
        snprintf(s_names[s_modal_edit_index], sizeof s_names[s_modal_edit_index], "%s", display);
        snprintf(s_paths[s_modal_edit_index], sizeof s_paths[s_modal_edit_index], "%s", s_modal_path);
        s_selected = (size_t) s_modal_edit_index;
        workflow_save();
    } else if (s_count < kMaxEntries) {
        snprintf(s_names[s_count], sizeof s_names[s_count], "%s", display);
        snprintf(s_paths[s_count], sizeof s_paths[s_count], "%s", s_modal_path);
        s_selected = s_count;
        s_count++;
        workflow_save();
    }

    s_modal_open = 0;
    s_modal_edit_index = -1;
}
