/*
 * Beast2 desktop shell (raylib): Phase 1 — layout + input scaffolding + gallery strip.
 * Build with -DBEAST2_BUILD_RAYLIB_UI=ON (see root CMakeLists.txt).
 */
#include <beast2/app.h>

#include "ui_fonts.h"
#include "ui_gallery.h"
#include "ui_input.h"
#include "ui_layout.h"
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

int main(void) {
    printf("beast2_core: beast2_run @ %p\n", (void *)(uintptr_t)beast2_run);
    beast2_desktop_log_native_deps();

    const int screen_width = 960;
    const int screen_height = 540;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "Beast2");
    SetTargetFPS(60);

    beast2_ui_fonts_init();
    ui_gallery_init();

    while (!WindowShouldClose()) {
        beast2_ui_input_begin_frame();

        Beast2UiRootLayout layout;
        beast2_ui_layout_root(GetRenderWidth(), GetRenderHeight(), &layout);

        Beast2UiInput ui_in;
        beast2_ui_input_end_frame(&layout, &ui_in);

        ui_gallery_update(ui_in.wheel_for_gallery);

        BeginDrawing();
        ClearBackground(BEAST2_UI_COLOR_BG);
        beast2_ui_shell_draw_header(&layout);
        ui_gallery_draw(layout.gallery);
        EndDrawing();
    }

    ui_gallery_shutdown();
    beast2_ui_fonts_shutdown();
    CloseWindow();
    return 0;
}
