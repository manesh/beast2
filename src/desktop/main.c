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
#include "theme.h"

#include <raylib.h>
#include <stdint.h>
#include <stdio.h>

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

    const int screen_width = 960;
    const int screen_height = 540;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "Beast2");
    SetTargetFPS(60);

    beast2_ui_fonts_init();
    ui_chrome_init();
    ui_context_menu_init();
    desktop_execution_init();
    ui_gallery_init(config_path);
    if (argc > 2 && argv[2] != NULL && argv[2][0] != '\0') {
        desktop_execution_set_paths(config_path, argv[2]);
    } else {
        desktop_execution_set_paths(config_path, "examples/sdxl_character_concept.b2");
    }

    while (!WindowShouldClose()) {
        beast2_ui_input_begin_frame();

        Beast2UiRootLayout layout;
        beast2_ui_layout_root(GetRenderWidth(), GetRenderHeight(), !ui_chrome_is_folder_view(), &layout);

        Beast2UiInput ui_in;
        beast2_ui_input_end_frame(&layout, &ui_in);

        ui_context_menu_update_keys();

        if (desktop_execution_consume_completed_pulse()) {
            media_bridge_refresh_tag_names();
            ui_gallery_reload_files();
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
            if (ui_context_menu_handle_click(ui_in.mouse, true)) {
                /* menu consumed */
            } else if (!ui_chrome_handle_click(ui_in.mouse, true, &layout)) {
                if (CheckCollisionPointRec(ui_in.mouse, layout.gallery)) {
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

        ui_gallery_update(ui_context_menu_is_open() ? 0.0f : ui_in.wheel_for_gallery);

        BeginDrawing();
        ClearBackground(BEAST2_UI_COLOR_BG);
        beast2_ui_shell_draw_header(&layout);
        ui_chrome_draw(&layout);
        ui_gallery_draw(layout.gallery);
        ui_context_menu_draw();
        ui_infobar_draw(&layout);
        EndDrawing();
    }

    ui_gallery_shutdown();
    desktop_execution_shutdown();
    beast2_ui_fonts_shutdown();
    CloseWindow();
    return 0;
}
