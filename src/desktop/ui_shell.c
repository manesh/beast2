/*
 * Header strip — Phase 3 will add breadcrumbs, tabs, and toolbar controls.
 */
#include "ui_shell.h"

#include "theme.h"
#include "ui_fonts.h"
#include "ui_layout.h"

void beast2_ui_shell_draw_header(const Beast2UiRootLayout *layout) {
    const int x = (int)layout->header.x;
    const int y = (int)layout->header.y;
    DrawText("Beast2", x, y, BEAST2_UI_FONT_TITLE_PX, BEAST2_UI_COLOR_TEXT_PRIMARY);
    DrawText(
        "Gallery (scroll wheel over grid) — Phase 1 scaffolding",
        x,
        y + BEAST2_UI_FONT_TITLE_PX + 4,
        BEAST2_UI_FONT_SUBTITLE_PX,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
}
