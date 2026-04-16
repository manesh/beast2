/*
 * Global keyboard shortcuts (Phase 7). Call after per-widget key handlers.
 */
#ifndef BEAST2_UI_HOTKEYS_H
#define BEAST2_UI_HOTKEYS_H

#include "ui_layout.h"

#include <stdbool.h>

void ui_hotkeys_frame(
    const Beast2UiRootLayout *layout,
    const char *config_path,
    int use_cli_generator,
    bool block_for_modal_or_menu
);

#endif
