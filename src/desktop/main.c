/*
 * Beast2 desktop shell (raylib): gallery MVP + optional ONNX/FFmpeg link probes.
 * Build with -DBEAST2_BUILD_RAYLIB_UI=ON (see root CMakeLists.txt).
 */
#include <beast2/app.h>

#include "ui_gallery.h"

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
    /* Reference a symbol from beast2_core so the static library is linked on all toolchains. */
    printf("beast2_core: beast2_run @ %p\n", (void *)(uintptr_t)beast2_run);
    beast2_desktop_log_native_deps();

    const int screen_width = 960;
    const int screen_height = 540;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "Beast2");
    SetTargetFPS(60);

    ui_gallery_init();

    while (!WindowShouldClose()) {
        const int rw = GetRenderWidth();
        const int rh = GetRenderHeight();
        const Rectangle gallery = {24.0f, 84.0f, (float)rw - 48.0f, (float)rh - 108.0f};

        float wheel = GetMouseWheelMove();
        if (!CheckCollisionPointRec(GetMousePosition(), gallery)) {
            wheel = 0.0f;
        }
        ui_gallery_update(wheel);

        BeginDrawing();
        ClearBackground((Color){18, 18, 24, 255});
        DrawText("Beast2", 24, 18, 26, RAYWHITE);
        DrawText("Gallery (scroll wheel)", 24, 48, 16, (Color){160, 160, 175, 255});

        ui_gallery_draw(gallery);

        EndDrawing();
    }

    ui_gallery_shutdown();
    CloseWindow();
    return 0;
}
