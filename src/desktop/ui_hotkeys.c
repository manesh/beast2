#include "ui_hotkeys.h"

#include "desktop_execution.h"
#include "media_bridge.h"
#include "ui_chrome.h"
#include "ui_context_menu.h"
#include "ui_gallery.h"
#include "ui_sidebar.h"
#include "ui_toast.h"

#include <raylib.h>

void ui_hotkeys_frame(
    const Beast2UiRootLayout *layout,
    const char *config_path,
    int use_cli_generator,
    bool block_for_modal_or_menu
) {
    (void) config_path;

    if (block_for_modal_or_menu) {
        return;
    }

    if (IsKeyPressed(KEY_F5) || ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_R))) {
        media_bridge_refresh_tag_names();
        ui_gallery_reload_files();
        return;
    }

    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_B)) {
        ui_sidebar_toggle_visible();
        return;
    }

    if (IsKeyPressed(KEY_F6) || ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_ENTER))) {
        (void) use_cli_generator;
        if (!desktop_execution_is_running()) {
            if (desktop_execution_try_start() != 0) {
                ui_toast_show(desktop_execution_status_line(), 4.5f);
            }
        }
        return;
    }

    ui_gallery_handle_keyboard(layout);
}
