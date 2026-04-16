/*
 * Beast2 desktop shell (raylib): Phase 3 — gallery chrome + selection + info bar.
 * Build with -DBEAST2_BUILD_RAYLIB_UI=ON (see root CMakeLists.txt).
 */
#include <beast2/app.h>

#include "ui_chrome.h"
#include "ui_context_menu.h"
#include "ui_fonts.h"
#include "ui_gallery.h"
#include "ui_infobar.h"
#include "ui_input.h"
#include "desktop_execution.h"
#include "media_bridge.h"
#include "ui_layout.h"
#include "ui_selection.h"
#include "ui_shell.h"
#include "ui_sidebar.h"
#include "gallery_model.h"
#include "workflow_panel.h"
#include "ui_hotkeys.h"
#include "ui_toast.h"
#include "theme.h"

#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef BEAST2_HAS_ONNXRUNTIME
#include <onnxruntime_c_api.h>
#endif
#ifdef BEAST2_HAS_FFMPEG
#include <libavutil/avutil.h>
#endif

static void beast2_desktop_log_native_deps(void) {
#ifdef BEAST2_HAS_ONNXRUNTIME
    printf("ONNX Runtime: OrtGetApiBase @ %p\n", (void *)OrtGetApiBase());
#endif
#ifdef BEAST2_HAS_FFMPEG
    printf("FFmpeg libavutil: %s\n", av_version_info());
#endif
}

int main(int argc, char **argv) {
    const char *config_path = "config/beast2.conf";

    printf("beast2_core: beast2_run @ %p\n", (void *)(uintptr_t) beast2_run);
    beast2_desktop_log_native_deps();

    if (argc > 1 && argv[1] != NULL && argv[1][0] != '\0') {
        config_path = argv[1];
    }

    const int use_cli_generator = (argc > 2 && argv[2] != NULL && argv[2][0] != '\0');

    const int screen_width = 960;
    const int screen_height = 540;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "Beast2");
    SetTargetFPS(60);

    beast2_ui_fonts_init();
    ui_chrome_init();
    ui_sidebar_init();
    ui_context_menu_init();
    ui_toast_init();
    desktop_execution_init();
    ui_gallery_init(config_path);
    workflow_panel_init(gallery_model_workspace_root());
    if (use_cli_generator) {
        desktop_execution_set_paths(config_path, argv[2]);
    } else {
        workflow_panel_sync_desktop_generator(config_path, 0);
    }

    while (!WindowShouldClose()) {
        beast2_ui_input_begin_frame();
        beast2_ui_fonts_begin_frame();

        Beast2UiRootLayout layout;
        beast2_ui_layout_root(
            GetRenderWidth(),
            GetRenderHeight(),
            !ui_chrome_is_folder_view(),
            ui_sidebar_is_visible(),
            ui_sidebar_get_width(),
            &layout
        );

        Beast2UiInput ui_in;
        beast2_ui_input_end_frame(&layout, &ui_in);

        ui_toast_begin_frame(GetFrameTime());

        workflow_panel_sync_desktop_generator(config_path, use_cli_generator);

        ui_sidebar_update_interaction(&layout);

        if (workflow_panel_is_modal_open()) {
            workflow_panel_update_keys();
        } else {
            ui_context_menu_update_keys();
        }

        ui_hotkeys_frame(
            &layout,
            config_path,
            use_cli_generator,
            workflow_panel_is_modal_open() || ui_context_menu_is_open()
        );

        if (desktop_execution_consume_completed_pulse()) {
            const char *st = desktop_execution_status_line();

            media_bridge_refresh_tag_names();
            ui_gallery_reload_files();
            if (
                strstr(st, "failed") != NULL ||
                strstr(st, "Failed") != NULL ||
                strstr(st, "not found") != NULL
            ) {
                ui_toast_show(st, 6.0f);
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            if (
                !ui_context_menu_is_open() &&
                media_bridge_ready() &&
                CheckCollisionPointRec(ui_in.mouse, layout.gallery)
            ) {
                const int idx = ui_gallery_pick_file_index(ui_in.mouse, layout.gallery);

                if (idx >= 0) {
                    ui_context_menu_open(ui_in.mouse, idx);
                }
            }
        }

        if (ui_in.left_pressed) {
            if (workflow_panel_handle_click(ui_in.mouse, true, &layout, ui_sidebar_get_scroll_y())) {
                /* workflow panel or modal consumed */
            } else if (ui_context_menu_handle_click(ui_in.mouse, true)) {
                /* menu consumed */
            } else if (!ui_chrome_handle_click(ui_in.mouse, true, &layout)) {
                if (ui_sidebar_handle_click(ui_in.mouse, true, &layout)) {
                    /* sidebar resize grip consumed */
                } else if (CheckCollisionPointRec(ui_in.mouse, layout.gallery)) {
                    const int idx = ui_gallery_pick_file_index(ui_in.mouse, layout.gallery);
                    if (idx >= 0) {
                        const int ctrl =
                            IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
                        const int shift =
                            IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
                        ui_selection_on_click((size_t) idx, ctrl, shift);
                    }
                }
            }
        }

        if (ui_context_menu_is_open() || workflow_panel_is_modal_open()) {
            ui_sidebar_scroll(0.0f);
            ui_gallery_update(0.0f);
        } else {
            ui_sidebar_scroll(ui_in.wheel_for_sidebar);
            ui_gallery_update(ui_in.wheel_for_gallery);
        }

        BeginDrawing();
        ClearBackground(BEAST2_UI_COLOR_BG);
        beast2_ui_shell_draw_header(&layout);
        ui_chrome_draw(&layout);
        ui_gallery_draw(layout.gallery);
        ui_sidebar_draw(&layout);
        ui_context_menu_draw();
        workflow_panel_draw_modal_overlay();
        ui_infobar_draw(&layout);
        ui_toast_draw(&layout.info_bar, (float) GetRenderWidth());
        EndDrawing();
    }

    ui_gallery_shutdown();
    desktop_execution_shutdown();
    beast2_ui_fonts_shutdown();
    CloseWindow();
    return 0;
}
