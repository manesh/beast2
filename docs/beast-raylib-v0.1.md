Here is a repo-ready raylib ui structs header v0.1 you can drop in and point Cursor at.

#ifndef BEAST_UI_TYPES_H
#define BEAST_UI_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BEAST_ID_MAX 64
#define BEAST_NAME_MAX 256
#define BEAST_PATH_MAX 512
#define BEAST_TEXT_SHORT 256
#define BEAST_TEXT_MEDIUM 1024
#define BEAST_TEXT_LONG 4096
#define BEAST_ARTIFACT_MAX 4096
#define BEAST_JOBS_MAX 256
#define BEAST_COMMAND_HISTORY_MAX 256
#define BEAST_SELECTION_NONE (-1)

typedef enum BeastArtifactType {
    BEAST_ARTIFACT_NONE = 0,
    BEAST_ARTIFACT_IMAGE,
    BEAST_ARTIFACT_VIDEO,
    BEAST_ARTIFACT_DOC,
    BEAST_ARTIFACT_CODE,
    BEAST_ARTIFACT_AUDIO,
    BEAST_ARTIFACT_DATA
} BeastArtifactType;

typedef enum BeastArtifactRating {
    BEAST_RATING_UNSET = 0,
    BEAST_RATING_DIAMOND,
    BEAST_RATING_OK,
    BEAST_RATING_LANDFILL
} BeastArtifactRating;

typedef enum BeastPanelId {
    BEAST_PANEL_NONE = 0,
    BEAST_PANEL_TOPBAR,
    BEAST_PANEL_SHELF,
    BEAST_PANEL_CANVAS,
    BEAST_PANEL_INSPECTOR,
    BEAST_PANEL_COMMAND,
    BEAST_PANEL_JOBS
} BeastPanelId;

typedef enum BeastTheme {
    BEAST_THEME_DARK = 0,
    BEAST_THEME_LIGHT
} BeastTheme;

typedef enum BeastShelfTab {
    BEAST_SHELF_RECENT = 0,
    BEAST_SHELF_DIAMOND,
    BEAST_SHELF_OK,
    BEAST_SHELF_LANDFILL
} BeastShelfTab;

typedef enum BeastSortMode {
    BEAST_SORT_NEWEST = 0,
    BEAST_SORT_OLDEST,
    BEAST_SORT_NAME
} BeastSortMode;

typedef enum BeastJobType {
    BEAST_JOB_NONE = 0,
    BEAST_JOB_IMAGE_GEN,
    BEAST_JOB_VIDEO_GEN,
    BEAST_JOB_LOAD_PREVIEW,
    BEAST_JOB_CODE_EDIT,
    BEAST_JOB_INDEX,
    BEAST_JOB_REPEAT
} BeastJobType;

typedef enum BeastJobStatus {
    BEAST_JOB_QUEUED = 0,
    BEAST_JOB_RUNNING,
    BEAST_JOB_COMPLETED,
    BEAST_JOB_FAILED,
    BEAST_JOB_CANCELLING,
    BEAST_JOB_CANCELLED
} BeastJobStatus;

typedef enum BeastCommandParseStatus {
    BEAST_PARSE_EMPTY = 0,
    BEAST_PARSE_VALID,
    BEAST_PARSE_INVALID,
    BEAST_PARSE_AMBIGUOUS
} BeastCommandParseStatus;

typedef enum BeastDragType {
    BEAST_DRAG_NONE = 0,
    BEAST_DRAG_ARTIFACT
} BeastDragType;

typedef struct BeastRect {
    float x;
    float y;
    float w;
    float h;
} BeastRect;

typedef struct BeastViewportState {
    float zoom;
    float pan_x;
    float pan_y;
    bool fit_to_view;
} BeastViewportState;

typedef struct BeastArtifact {
    char id[BEAST_ID_MAX];
    char name[BEAST_NAME_MAX];
    BeastArtifactType type;
    BeastArtifactRating rating;

    char thumbnail_path[BEAST_PATH_MAX];
    char preview_path[BEAST_PATH_MAX];

    char prompt[BEAST_TEXT_LONG];
    char negative_prompt[BEAST_TEXT_LONG];
    char model_name[BEAST_NAME_MAX];

    char signer[BEAST_NAME_MAX];
    bool signed_flag;

    uint32_t seed;
    int width;
    int height;
    float runtime_seconds;

    int64_t created_at_unix;
    bool loaded;
} BeastArtifact;

typedef struct BeastArtifactList {
    BeastArtifact items[BEAST_ARTIFACT_MAX];
    int count;
} BeastArtifactList;

typedef struct BeastGeneratorEditState {
    char prompt[BEAST_TEXT_LONG];
    char negative_prompt[BEAST_TEXT_LONG];
    char model_name[BEAST_NAME_MAX];

    uint32_t seed;
    int count;
    int width;
    int height;

    bool dirty;
} BeastGeneratorEditState;

typedef struct BeastCommandHistoryItem {
    char text[BEAST_TEXT_MEDIUM];
} BeastCommandHistoryItem;

typedef struct BeastCommandState {
    char input[BEAST_TEXT_MEDIUM];
    char parse_preview[BEAST_TEXT_MEDIUM];
    char status_text[BEAST_TEXT_MEDIUM];

    BeastCommandParseStatus parse_status;

    BeastCommandHistoryItem history[BEAST_COMMAND_HISTORY_MAX];
    int history_count;
    int history_index;
} BeastCommandState;

typedef struct BeastJob {
    char id[BEAST_ID_MAX];
    char name[BEAST_NAME_MAX];
    BeastJobType type;
    BeastJobStatus status;

    float progress_0_to_1;
    float elapsed_seconds;
    bool cancellable;
} BeastJob;

typedef struct BeastJobList {
    BeastJob items[BEAST_JOBS_MAX];
    int count;
} BeastJobList;

typedef struct BeastUIState {
    int window_width;
    int window_height;

    BeastTheme theme;
    BeastPanelId focused_panel;

    float left_panel_width;
    float right_panel_width;
    float bottom_panel_height;
    float top_bar_height;
    float jobs_strip_height;

    bool shelf_collapsed;
    bool inspector_collapsed;
    bool command_collapsed;
    bool jobs_collapsed;

    BeastRect topbar_rect;
    BeastRect shelf_rect;
    BeastRect canvas_rect;
    BeastRect inspector_rect;
    BeastRect command_rect;
    BeastRect jobs_rect;

    bool resizing_left;
    bool resizing_right;
    bool resizing_bottom;

    BeastViewportState canvas_view;
} BeastUIState;

typedef struct BeastProjectState {
    char workspace_path[BEAST_PATH_MAX];
    char project_name[BEAST_NAME_MAX];
    bool autosave_enabled;
    bool dirty;
} BeastProjectState;

typedef struct BeastArtifactState {
    BeastArtifactList all;
    BeastShelfTab active_tab;
    BeastSortMode sort_mode;

    int selected_index;
    int opened_index;
    int hovered_index;
} BeastArtifactState;

typedef struct BeastSelectionState {
    BeastPanelId selected_panel;
    int selected_artifact_index;
    int hovered_artifact_index;

    BeastDragType drag_type;
    int dragged_artifact_index;

    bool command_input_focused;
} BeastSelectionState;

typedef struct BeastAppState {
    BeastUIState ui;
    BeastProjectState project;
    BeastArtifactState artifacts;
    BeastGeneratorEditState generator_edit;
    BeastCommandState command;
    BeastJobList jobs_running;
    BeastJobList jobs_queued;
    BeastSelectionState selection;
} BeastAppState;

/* lifecycle */
void BeastUI_InitDefaultState(BeastAppState *app, int window_width, int window_height);
void BeastUI_RecalculateLayout(BeastAppState *app, int window_width, int window_height);

/* artifact helpers */
void BeastArtifacts_Clear(BeastArtifactState *state);
int BeastArtifacts_Add(BeastArtifactState *state, const BeastArtifact *artifact);
BeastArtifact *BeastArtifacts_Get(BeastArtifactState *state, int index);
const BeastArtifact *BeastArtifacts_GetConst(const BeastArtifactState *state, int index);

/* selection helpers */
void BeastSelection_Clear(BeastSelectionState *selection);
void BeastSelection_SelectArtifact(BeastAppState *app, int artifact_index);
const BeastArtifact *BeastSelection_GetSelectedArtifactConst(const BeastAppState *app);

/* generator edit helpers */
void BeastGeneratorEdit_FromArtifact(BeastGeneratorEditState *edit, const BeastArtifact *artifact);
void BeastGeneratorEdit_Clear(BeastGeneratorEditState *edit);

/* command helpers */
void BeastCommand_Clear(BeastCommandState *command);
void BeastCommand_PushHistory(BeastCommandState *command, const char *text);

/* job helpers */
void BeastJobs_Clear(BeastJobList *jobs);
int BeastJobs_Add(BeastJobList *jobs, const BeastJob *job);
BeastJob *BeastJobs_Get(BeastJobList *jobs, int index);

/* defaults */
float BeastUI_DefaultLeftPanelWidth(void);
float BeastUI_DefaultRightPanelWidth(void);
float BeastUI_DefaultBottomPanelHeight(void);
float BeastUI_DefaultTopBarHeight(void);
float BeastUI_DefaultJobsStripHeight(void);

#ifdef __cplusplus
}
#endif

#endif

And here is a matching implementation stub you can also drop in as a starting point.

#include "beast_ui_types.h"

#include <string.h>

static void BeastRect_Set(BeastRect *r, float x, float y, float w, float h) {
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
}

float BeastUI_DefaultLeftPanelWidth(void)  { return 280.0f; }
float BeastUI_DefaultRightPanelWidth(void) { return 360.0f; }
float BeastUI_DefaultBottomPanelHeight(void) { return 120.0f; }
float BeastUI_DefaultTopBarHeight(void) { return 40.0f; }
float BeastUI_DefaultJobsStripHeight(void) { return 72.0f; }

void BeastArtifacts_Clear(BeastArtifactState *state) {
    if (!state) return;
    memset(&state->all, 0, sizeof(state->all));
    state->active_tab = BEAST_SHELF_RECENT;
    state->sort_mode = BEAST_SORT_NEWEST;
    state->selected_index = BEAST_SELECTION_NONE;
    state->opened_index = BEAST_SELECTION_NONE;
    state->hovered_index = BEAST_SELECTION_NONE;
}

int BeastArtifacts_Add(BeastArtifactState *state, const BeastArtifact *artifact) {
    if (!state || !artifact) return BEAST_SELECTION_NONE;
    if (state->all.count >= BEAST_ARTIFACT_MAX) return BEAST_SELECTION_NONE;
    state->all.items[state->all.count] = *artifact;
    state->all.items[state->all.count].loaded = true;
    return state->all.count++;
}

BeastArtifact *BeastArtifacts_Get(BeastArtifactState *state, int index) {
    if (!state) return NULL;
    if (index < 0 || index >= state->all.count) return NULL;
    return &state->all.items[index];
}

const BeastArtifact *BeastArtifacts_GetConst(const BeastArtifactState *state, int index) {
    if (!state) return NULL;
    if (index < 0 || index >= state->all.count) return NULL;
    return &state->all.items[index];
}

void BeastSelection_Clear(BeastSelectionState *selection) {
    if (!selection) return;
    selection->selected_panel = BEAST_PANEL_NONE;
    selection->selected_artifact_index = BEAST_SELECTION_NONE;
    selection->hovered_artifact_index = BEAST_SELECTION_NONE;
    selection->drag_type = BEAST_DRAG_NONE;
    selection->dragged_artifact_index = BEAST_SELECTION_NONE;
    selection->command_input_focused = false;
}

void BeastSelection_SelectArtifact(BeastAppState *app, int artifact_index) {
    if (!app) return;
    if (artifact_index < 0 || artifact_index >= app->artifacts.all.count) return;

    app->artifacts.selected_index = artifact_index;
    app->artifacts.opened_index = artifact_index;
    app->selection.selected_artifact_index = artifact_index;
    app->selection.selected_panel = BEAST_PANEL_SHELF;

    const BeastArtifact *artifact = BeastArtifacts_GetConst(&app->artifacts, artifact_index);
    BeastGeneratorEdit_FromArtifact(&app->generator_edit, artifact);
}

const BeastArtifact *BeastSelection_GetSelectedArtifactConst(const BeastAppState *app) {
    if (!app) return NULL;
    return BeastArtifacts_GetConst(&app->artifacts, app->artifacts.selected_index);
}

void BeastGeneratorEdit_FromArtifact(BeastGeneratorEditState *edit, const BeastArtifact *artifact) {
    if (!edit) return;
    BeastGeneratorEdit_Clear(edit);
    if (!artifact) return;

    strncpy(edit->prompt, artifact->prompt, sizeof(edit->prompt) - 1);
    strncpy(edit->negative_prompt, artifact->negative_prompt, sizeof(edit->negative_prompt) - 1);
    strncpy(edit->model_name, artifact->model_name, sizeof(edit->model_name) - 1);
    edit->seed = artifact->seed;
    edit->count = 1;
    edit->width = artifact->width;
    edit->height = artifact->height;
    edit->dirty = false;
}

void BeastGeneratorEdit_Clear(BeastGeneratorEditState *edit) {
    if (!edit) return;
    memset(edit, 0, sizeof(*edit));
    edit->count = 1;
    edit->width = 1024;
    edit->height = 1024;
    edit->dirty = false;
}

void BeastCommand_Clear(BeastCommandState *command) {
    if (!command) return;
    memset(command, 0, sizeof(*command));
    command->parse_status = BEAST_PARSE_EMPTY;
    command->history_index = BEAST_SELECTION_NONE;
}

void BeastCommand_PushHistory(BeastCommandState *command, const char *text) {
    if (!command || !text || !text[0]) return;

    if (command->history_count < BEAST_COMMAND_HISTORY_MAX) {
        strncpy(command->history[command->history_count].text, text,
                sizeof(command->history[command->history_count].text) - 1);
        command->history_count++;
    } else {
        for (int i = 1; i < BEAST_COMMAND_HISTORY_MAX; ++i) {
            command->history[i - 1] = command->history[i];
        }
        strncpy(command->history[BEAST_COMMAND_HISTORY_MAX - 1].text, text,
                sizeof(command->history[BEAST_COMMAND_HISTORY_MAX - 1].text) - 1);
    }

    command->history_index = command->history_count;
}

void BeastJobs_Clear(BeastJobList *jobs) {
    if (!jobs) return;
    memset(jobs, 0, sizeof(*jobs));
}

int BeastJobs_Add(BeastJobList *jobs, const BeastJob *job) {
    if (!jobs || !job) return BEAST_SELECTION_NONE;
    if (jobs->count >= BEAST_JOBS_MAX) return BEAST_SELECTION_NONE;
    jobs->items[jobs->count] = *job;
    return jobs->count++;
}

BeastJob *BeastJobs_Get(BeastJobList *jobs, int index) {
    if (!jobs) return NULL;
    if (index < 0 || index >= jobs->count) return NULL;
    return &jobs->items[index];
}

void BeastUI_RecalculateLayout(BeastAppState *app, int window_width, int window_height) {
    if (!app) return;

    BeastUIState *ui = &app->ui;
    ui->window_width = window_width;
    ui->window_height = window_height;

    const float top_h = ui->top_bar_height;
    const float bottom_h = ui->command_collapsed ? 0.0f : ui->bottom_panel_height;
    const float jobs_h = ui->jobs_collapsed ? 0.0f : ui->jobs_strip_height;
    const float left_w = ui->shelf_collapsed ? 0.0f : ui->left_panel_width;
    const float right_w = ui->inspector_collapsed ? 0.0f : ui->right_panel_width;

    BeastRect_Set(&ui->topbar_rect, 0.0f, 0.0f, (float)window_width, top_h);

    const float content_y = top_h;
    const float content_h = (float)window_height - top_h - bottom_h - jobs_h;
    const float canvas_x = left_w;
    const float canvas_w = (float)window_width - left_w - right_w;

    BeastRect_Set(&ui->shelf_rect, 0.0f, content_y, left_w, content_h);
    BeastRect_Set(&ui->canvas_rect, canvas_x, content_y, canvas_w, content_h);
    BeastRect_Set(&ui->inspector_rect, canvas_x + canvas_w, content_y, right_w, content_h);
    BeastRect_Set(&ui->jobs_rect, 0.0f, top_h + content_h, (float)window_width, jobs_h);
    BeastRect_Set(&ui->command_rect, 0.0f, top_h + content_h + jobs_h, (float)window_width, bottom_h);
}

void BeastUI_InitDefaultState(BeastAppState *app, int window_width, int window_height) {
    if (!app) return;
    memset(app, 0, sizeof(*app));

    app->ui.window_width = window_width;
    app->ui.window_height = window_height;
    app->ui.theme = BEAST_THEME_DARK;
    app->ui.focused_panel = BEAST_PANEL_NONE;
    app->ui.left_panel_width = BeastUI_DefaultLeftPanelWidth();
    app->ui.right_panel_width = BeastUI_DefaultRightPanelWidth();
    app->ui.bottom_panel_height = BeastUI_DefaultBottomPanelHeight();
    app->ui.top_bar_height = BeastUI_DefaultTopBarHeight();
    app->ui.jobs_strip_height = BeastUI_DefaultJobsStripHeight();
    app->ui.shelf_collapsed = false;
    app->ui.inspector_collapsed = false;
    app->ui.command_collapsed = false;
    app->ui.jobs_collapsed = false;
    app->ui.canvas_view.zoom = 1.0f;
    app->ui.canvas_view.pan_x = 0.0f;
    app->ui.canvas_view.pan_y = 0.0f;
    app->ui.canvas_view.fit_to_view = true;

    app->project.autosave_enabled = true;
    strncpy(app->project.project_name, "beast", sizeof(app->project.project_name) - 1);

    BeastArtifacts_Clear(&app->artifacts);
    BeastGeneratorEdit_Clear(&app->generator_edit);
    BeastCommand_Clear(&app->command);
    BeastJobs_Clear(&app->jobs_running);
    BeastJobs_Clear(&app->jobs_queued);
    BeastSelection_Clear(&app->selection);

    BeastUI_RecalculateLayout(app, window_width, window_height);
}

And here is a short Cursor handoff note you can paste into the repo next to it.

# cursor handoff: beast ui structs v0.1

implement against `beast_ui_types.h`.

rules:

- do not redesign the state model unless necessary
- prefer adding helper functions over changing structs
- keep main thread non-blocking
- wire ui in this order:
  1. static layout
  2. artifact shelf
  3. canvas preview
  4. inspector
  5. command bar
  6. jobs strip

definition of done:
- select artifact
- inspect artifact
- edit generator fields
- rerun stub
- ui never freezes

If you want, next I can draft the raylib render/update function skeleton so agents have the exact entry points too.






yes






Here is a repo-ready raylib render/update function skeleton you can hand to Cursor agents.

⸻

beast ui app skeleton v0.1

purpose

define the first clear ui entry points for the beast desktop app

target:
	•	raylib
	•	immediate mode style
	•	non blocking main thread
	•	image gen workstation v0.1

⸻

file: src/ui/beast_ui_app.h

#ifndef BEAST_UI_APP_H
#define BEAST_UI_APP_H

#include <stdbool.h>
#include "../voice/../ui/../beast_ui_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BeastUIRuntime {
    bool initialized;
    bool should_quit;
    float dt_seconds;
} BeastUIRuntime;

bool BeastUI_AppInit(BeastAppState *app, BeastUIRuntime *rt, int window_width, int window_height);
void BeastUI_AppShutdown(BeastAppState *app, BeastUIRuntime *rt);

void BeastUI_AppHandleResize(BeastAppState *app, int window_width, int window_height);

void BeastUI_AppUpdate(BeastAppState *app, BeastUIRuntime *rt);
void BeastUI_AppRender(BeastAppState *app, BeastUIRuntime *rt);

#ifdef __cplusplus
}
#endif

#endif


⸻

file: src/ui/beast_ui_app.c

#include "beast_ui_app.h"

#include <string.h>

#include "raylib.h"
#include "../beast_ui_types.h"

/* forward declarations */
static void BeastUI_UpdateInput(BeastAppState *app, BeastUIRuntime *rt);
static void BeastUI_UpdateCommands(BeastAppState *app, BeastUIRuntime *rt);
static void BeastUI_UpdateJobs(BeastAppState *app, BeastUIRuntime *rt);
static void BeastUI_UpdateHover(BeastAppState *app, BeastUIRuntime *rt);

static void BeastUI_RenderTopbar(BeastAppState *app);
static void BeastUI_RenderShelf(BeastAppState *app);
static void BeastUI_RenderCanvas(BeastAppState *app);
static void BeastUI_RenderInspector(BeastAppState *app);
static void BeastUI_RenderJobs(BeastAppState *app);
static void BeastUI_RenderCommandBar(BeastAppState *app);

static Rectangle Beast_ToRayRect(BeastRect r);
static bool Beast_PointInRect(Vector2 p, BeastRect r);

bool BeastUI_AppInit(BeastAppState *app, BeastUIRuntime *rt, int window_width, int window_height) {
    if (!app || !rt) return false;

    memset(rt, 0, sizeof(*rt));
    BeastUI_InitDefaultState(app, window_width, window_height);

    rt->initialized = true;
    rt->should_quit = false;
    rt->dt_seconds = 0.0f;

    return true;
}

void BeastUI_AppShutdown(BeastAppState *app, BeastUIRuntime *rt) {
    (void)app;
    if (!rt) return;
    rt->initialized = false;
}

void BeastUI_AppHandleResize(BeastAppState *app, int window_width, int window_height) {
    if (!app) return;
    BeastUI_RecalculateLayout(app, window_width, window_height);
}

void BeastUI_AppUpdate(BeastAppState *app, BeastUIRuntime *rt) {
    if (!app || !rt || !rt->initialized) return;

    rt->dt_seconds = GetFrameTime();

    if (IsWindowResized()) {
        BeastUI_AppHandleResize(app, GetScreenWidth(), GetScreenHeight());
    }

    BeastUI_UpdateInput(app, rt);
    BeastUI_UpdateHover(app, rt);
    BeastUI_UpdateCommands(app, rt);
    BeastUI_UpdateJobs(app, rt);
}

void BeastUI_AppRender(BeastAppState *app, BeastUIRuntime *rt) {
    (void)rt;
    if (!app) return;

    BeginDrawing();
    ClearBackground((Color){18, 18, 22, 255});

    BeastUI_RenderTopbar(app);
    BeastUI_RenderShelf(app);
    BeastUI_RenderCanvas(app);
    BeastUI_RenderInspector(app);
    BeastUI_RenderJobs(app);
    BeastUI_RenderCommandBar(app);

    EndDrawing();
}

/* ---------------- update ---------------- */

static void BeastUI_UpdateInput(BeastAppState *app, BeastUIRuntime *rt) {
    (void)rt;
    Vector2 mouse = GetMousePosition();

    if (IsKeyPressed(KEY_ESCAPE)) {
        BeastSelection_Clear(&app->selection);
        app->command.parse_status = BEAST_PARSE_EMPTY;
        app->command.parse_preview[0] = '\0';
        strncpy(app->command.status_text, "cleared transient ui state", sizeof(app->command.status_text) - 1);
    }

    if (IsKeyPressed(KEY_SLASH)) {
        app->selection.command_input_focused = true;
        app->selection.selected_panel = BEAST_PANEL_COMMAND;
    }

    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_S)) {
        app->project.dirty = false;
        strncpy(app->command.status_text, "saved", sizeof(app->command.status_text) - 1);
    }

    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_R)) {
        strncpy(app->command.status_text, "rerun requested", sizeof(app->command.status_text) - 1);
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (Beast_PointInRect(mouse, app->ui.shelf_rect)) {
            app->selection.selected_panel = BEAST_PANEL_SHELF;
            app->selection.command_input_focused = false;
        } else if (Beast_PointInRect(mouse, app->ui.canvas_rect)) {
            app->selection.selected_panel = BEAST_PANEL_CANVAS;
            app->selection.command_input_focused = false;
        } else if (Beast_PointInRect(mouse, app->ui.inspector_rect)) {
            app->selection.selected_panel = BEAST_PANEL_INSPECTOR;
            app->selection.command_input_focused = false;
        } else if (Beast_PointInRect(mouse, app->ui.command_rect)) {
            app->selection.selected_panel = BEAST_PANEL_COMMAND;
            app->selection.command_input_focused = true;
        }
    }

    if (app->selection.command_input_focused) {
        int c = GetCharPressed();
        while (c > 0) {
            size_t len = strlen(app->command.input);
            if (c >= 32 && c <= 126 && len + 1 < sizeof(app->command.input)) {
                app->command.input[len] = (char)c;
                app->command.input[len + 1] = '\0';
            }
            c = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            size_t len = strlen(app->command.input);
            if (len > 0) app->command.input[len - 1] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER)) {
            BeastCommand_PushHistory(&app->command, app->command.input);

            if (strlen(app->command.input) == 0) {
                app->command.parse_status = BEAST_PARSE_EMPTY;
                strncpy(app->command.status_text, "empty command", sizeof(app->command.status_text) - 1);
            } else {
                app->command.parse_status = BEAST_PARSE_VALID;
                strncpy(app->command.parse_preview, app->command.input, sizeof(app->command.parse_preview) - 1);
                strncpy(app->command.status_text, "command queued", sizeof(app->command.status_text) - 1);
            }
        }
    }
}

static void BeastUI_UpdateHover(BeastAppState *app, BeastUIRuntime *rt) {
    (void)rt;
    Vector2 mouse = GetMousePosition();
    app->artifacts.hovered_index = BEAST_SELECTION_NONE;

    if (!Beast_PointInRect(mouse, app->ui.shelf_rect)) return;
    if (app->artifacts.all.count <= 0) return;

    const float card_h = 72.0f;
    const float start_y = app->ui.shelf_rect.y + 48.0f;
    const float rel_y = mouse.y - start_y;

    if (rel_y < 0.0f) return;

    int idx = (int)(rel_y / card_h);
    if (idx >= 0 && idx < app->artifacts.all.count) {
        app->artifacts.hovered_index = idx;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            BeastSelection_SelectArtifact(app, idx);
        }
    }
}

static void BeastUI_UpdateCommands(BeastAppState *app, BeastUIRuntime *rt) {
    (void)rt;

    /* v0.1 placeholder parser */
    if (strlen(app->command.input) == 0) {
        app->command.parse_status = BEAST_PARSE_EMPTY;
        app->command.parse_preview[0] = '\0';
        return;
    }

    if (strcmp(app->command.input, "run last gen") == 0 ||
        strcmp(app->command.input, "show last image") == 0 ||
        strcmp(app->command.input, "save this art") == 0 ||
        strcmp(app->command.input, "rate this diamond") == 0 ||
        strcmp(app->command.input, "rate this ok") == 0 ||
        strcmp(app->command.input, "rate this landfill") == 0) {
        app->command.parse_status = BEAST_PARSE_VALID;
        strncpy(app->command.parse_preview, app->command.input, sizeof(app->command.parse_preview) - 1);
    }
}

static void BeastUI_UpdateJobs(BeastAppState *app, BeastUIRuntime *rt) {
    for (int i = 0; i < app->jobs_running.count; ++i) {
        BeastJob *job = &app->jobs_running.items[i];
        if (job->status == BEAST_JOB_RUNNING) {
            job->elapsed_seconds += rt->dt_seconds;
            if (job->progress_0_to_1 < 1.0f) {
                job->progress_0_to_1 += rt->dt_seconds * 0.08f;
                if (job->progress_0_to_1 >= 1.0f) {
                    job->progress_0_to_1 = 1.0f;
                    job->status = BEAST_JOB_COMPLETED;
                }
            }
        }
    }
}

/* ---------------- render ---------------- */

static void BeastUI_RenderTopbar(BeastAppState *app) {
    Rectangle r = Beast_ToRayRect(app->ui.topbar_rect);
    DrawRectangleRec(r, (Color){28, 28, 34, 255});
    DrawText("beast", (int)r.x + 12, (int)r.y + 10, 20, RAYWHITE);

    const char *project = app->project.project_name[0] ? app->project.project_name : "untitled";
    DrawText(project, (int)r.x + 100, (int)r.y + 12, 16, GRAY);

    DrawText("local", (int)(r.x + r.width - 140), (int)r.y + 12, 16, LIGHTGRAY);
    DrawText("settings", (int)(r.x + r.width - 72), (int)r.y + 12, 16, GRAY);
}

static void BeastUI_RenderShelf(BeastAppState *app) {
    Rectangle r = Beast_ToRayRect(app->ui.shelf_rect);
    DrawRectangleRec(r, (Color){24, 24, 30, 255});

    DrawText("artifacts", (int)r.x + 12, (int)r.y + 12, 18, RAYWHITE);

    if (app->artifacts.all.count <= 0) {
        DrawText("no artifacts yet", (int)r.x + 12, (int)r.y + 48, 16, GRAY);
        DrawText("run a generator to begin", (int)r.x + 12, (int)r.y + 72, 16, GRAY);
        return;
    }

    const float card_h = 72.0f;
    float y = r.y + 48.0f;

    for (int i = 0; i < app->artifacts.all.count; ++i) {
        const BeastArtifact *a = &app->artifacts.all.items[i];
        Rectangle card = {r.x + 8.0f, y, r.width - 16.0f, card_h - 8.0f};

        Color bg = (i == app->artifacts.selected_index)
            ? (Color){58, 76, 108, 255}
            : (i == app->artifacts.hovered_index)
                ? (Color){38, 38, 48, 255}
                : (Color){30, 30, 38, 255};

        DrawRectangleRounded(card, 0.12f, 6, bg);
        DrawRectangle((int)card.x + 8, (int)card.y + 8, 48, 48, (Color){48, 48, 58, 255});

        DrawText(a->name[0] ? a->name : a->id, (int)card.x + 64, (int)card.y + 10, 16, RAYWHITE);
        DrawText(a->model_name[0] ? a->model_name : "no model", (int)card.x + 64, (int)card.y + 32, 14, GRAY);

        y += card_h;
        if (y > r.y + r.height - card_h) break;
    }
}

static void BeastUI_RenderCanvas(BeastAppState *app) {
    Rectangle r = Beast_ToRayRect(app->ui.canvas_rect);
    DrawRectangleRec(r, (Color){16, 16, 20, 255});
    DrawText("canvas", (int)r.x + 12, (int)r.y + 12, 18, RAYWHITE);

    const BeastArtifact *selected = BeastSelection_GetSelectedArtifactConst(app);
    if (!selected) {
        DrawText("drop artifact here", (int)r.x + 24, (int)r.y + 56, 22, GRAY);
        DrawText("or run a generator", (int)r.x + 24, (int)r.y + 84, 20, GRAY);
        return;
    }

    Rectangle preview = {
        r.x + 24.0f,
        r.y + 48.0f,
        r.width - 48.0f,
        r.height - 72.0f
    };

    DrawRectangleRounded(preview, 0.02f, 8, (Color){36, 36, 44, 255});
    DrawText(selected->name[0] ? selected->name : selected->id,
             (int)preview.x + 16, (int)preview.y + 16, 22, RAYWHITE);
    DrawText("preview placeholder", (int)preview.x + 16, (int)preview.y + 48, 18, GRAY);
}

static void BeastUI_RenderInspector(BeastAppState *app) {
    Rectangle r = Beast_ToRayRect(app->ui.inspector_rect);
    DrawRectangleRec(r, (Color){24, 24, 30, 255});
    DrawText("inspector", (int)r.x + 12, (int)r.y + 12, 18, RAYWHITE);

    const BeastArtifact *selected = BeastSelection_GetSelectedArtifactConst(app);
    if (!selected) {
        DrawText("select an artifact to inspect", (int)r.x + 12, (int)r.y + 48, 16, GRAY);
        return;
    }

    int y = (int)r.y + 48;
    DrawText("summary", (int)r.x + 12, y, 16, RAYWHITE); y += 24;
    DrawText(selected->id, (int)r.x + 12, y, 14, GRAY); y += 22;
    DrawText(selected->model_name[0] ? selected->model_name : "no model", (int)r.x + 12, y, 14, GRAY); y += 30;

    DrawText("quick edit", (int)r.x + 12, y, 16, RAYWHITE); y += 24;
    DrawText("prompt:", (int)r.x + 12, y, 14, LIGHTGRAY); y += 18;
    DrawRectangle((int)r.x + 12, y, (int)r.width - 24, 72, (Color){32, 32, 40, 255});
    DrawText(app->generator_edit.prompt[0] ? app->generator_edit.prompt : "(empty)",
             (int)r.x + 18, y + 8, 14, GRAY);
    y += 88;

    DrawText("rerun", (int)r.x + 12, y, 16, RAYWHITE);
    DrawText("run 4 more", (int)r.x + 96, y, 16, RAYWHITE);
}

static void BeastUI_RenderJobs(BeastAppState *app) {
    Rectangle r = Beast_ToRayRect(app->ui.jobs_rect);
    DrawRectangleRec(r, (Color){26, 26, 34, 255});
    DrawText("jobs", (int)r.x + 12, (int)r.y + 10, 16, RAYWHITE);

    if (app->jobs_running.count <= 0) {
        DrawText("no active jobs", (int)r.x + 12, (int)r.y + 34, 14, GRAY);
        return;
    }

    float x = r.x + 12.0f;
    for (int i = 0; i < app->jobs_running.count; ++i) {
        const BeastJob *job = &app->jobs_running.items[i];
        Rectangle card = {x, r.y + 28.0f, 220.0f, r.height - 36.0f};
        DrawRectangleRounded(card, 0.15f, 6, (Color){34, 34, 44, 255});

        DrawText(job->name, (int)card.x + 10, (int)card.y + 8, 14, RAYWHITE);
        DrawRectangle((int)card.x + 10, (int)card.y + 30, 180, 10, (Color){24, 24, 30, 255});
        DrawRectangle((int)card.x + 10, (int)card.y + 30, (int)(180.0f * job->progress_0_to_1), 10, SKYBLUE);

        x += 232.0f;
        if (x > r.x + r.width - 220.0f) break;
    }
}

static void BeastUI_RenderCommandBar(BeastAppState *app) {
    Rectangle r = Beast_ToRayRect(app->ui.command_rect);
    DrawRectangleRec(r, (Color){20, 20, 26, 255});

    DrawText("command", (int)r.x + 12, (int)r.y + 10, 16, RAYWHITE);

    Rectangle input = {r.x + 12.0f, r.y + 36.0f, r.width - 24.0f, 36.0f};
    DrawRectangleRounded(input, 0.18f, 6,
        app->selection.command_input_focused ? (Color){42, 42, 52, 255} : (Color){30, 30, 38, 255});

    DrawText(app->command.input[0] ? app->command.input : "type command...",
             (int)input.x + 10, (int)input.y + 10, 16,
             app->command.input[0] ? RAYWHITE : GRAY);

    DrawText(app->command.status_text[0] ? app->command.status_text : "ready",
             (int)r.x + 12, (int)r.y + 80, 14, GRAY);

    if (app->command.parse_status == BEAST_PARSE_VALID && app->command.parse_preview[0]) {
        DrawText(app->command.parse_preview, (int)r.x + 220, (int)r.y + 80, 14, LIGHTGRAY);
    }
}

/* ---------------- helpers ---------------- */

static Rectangle Beast_ToRayRect(BeastRect r) {
    Rectangle out = { r.x, r.y, r.w, r.h };
    return out;
}

static bool Beast_PointInRect(Vector2 p, BeastRect r) {
    return p.x >= r.x && p.x <= (r.x + r.w) &&
           p.y >= r.y && p.y <= (r.y + r.h);
}


⸻

file: src/ui/beast_ui_app_main_example.c

This gives agents a minimal executable shape.

#include "raylib.h"

#include "../beast_ui_types.h"
#include "beast_ui_app.h"

static BeastArtifact MakeMockArtifact(const char *id, const char *name, const char *model, const char *prompt) {
    BeastArtifact a = {0};
    snprintf(a.id, sizeof(a.id), "%s", id);
    snprintf(a.name, sizeof(a.name), "%s", name);
    snprintf(a.model_name, sizeof(a.model_name), "%s", model);
    snprintf(a.prompt, sizeof(a.prompt), "%s", prompt);
    a.type = BEAST_ARTIFACT_IMAGE;
    a.rating = BEAST_RATING_OK;
    a.width = 1024;
    a.height = 1024;
    a.seed = 42;
    a.runtime_seconds = 9.7f;
    a.loaded = true;
    return a;
}

static BeastJob MakeMockJob(const char *id, const char *name) {
    BeastJob j = {0};
    snprintf(j.id, sizeof(j.id), "%s", id);
    snprintf(j.name, sizeof(j.name), "%s", name);
    j.type = BEAST_JOB_IMAGE_GEN;
    j.status = BEAST_JOB_RUNNING;
    j.progress_0_to_1 = 0.15f;
    j.elapsed_seconds = 0.0f;
    j.cancellable = true;
    return j;
}

int main(void) {
    const int width = 1600;
    const int height = 900;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "beast");
    SetTargetFPS(60);

    BeastAppState app;
    BeastUIRuntime rt;

    BeastUI_AppInit(&app, &rt, width, height);

    BeastArtifact a0 = MakeMockArtifact("art_0001", "red armor study", "wan22", "humanoid facing camera red armor");
    BeastArtifact a1 = MakeMockArtifact("art_0002", "gold armor study", "wan22", "humanoid facing camera gold armor");

    BeastArtifacts_Add(&app.artifacts, &a0);
    BeastArtifacts_Add(&app.artifacts, &a1);
    BeastSelection_SelectArtifact(&app, 0);

    BeastJob job = MakeMockJob("job_0001", "generate variants");
    BeastJobs_Add(&app.jobs_running, &job);

    while (!WindowShouldClose() && !rt.should_quit) {
        BeastUI_AppUpdate(&app, &rt);
        BeastUI_AppRender(&app, &rt);
    }

    BeastUI_AppShutdown(&app, &rt);
    CloseWindow();
    return 0;
}


⸻

cursor handoff note

Drop this in as docs/cursor-ui-app-handoff-v0.1.md.

# cursor handoff: beast ui app skeleton v0.1

implement against:
- `beast_ui_types.h`
- `beast_ui_app.h`
- `beast_ui_app.c`

rules:
- do not block the main thread
- do not replace the app state model
- keep render and update paths separate
- keep ui simple and inspectable
- use mock data first
- wire real artifact loading later

first tasks:
1. compile the example
2. make panel resizing work
3. make artifact shelf selection robust
4. add command parsing for minimal command set
5. add real image preview loading
6. add rerun button stub
7. connect jobs strip to real background jobs

definition of done:
- app launches
- panels render
- artifact selection works
- command input works
- jobs animate without freezing
- selected artifact appears in inspector and canvas

If you want, next I can draft the minimal command parser spec and C header for:
	•	run last gen
	•	make 4 image
	•	show last image
	•	save this art
	•	rate this diamond/ok/landfill


