/*
 * Title header (toolbar lives in ui_chrome).
 */
#include "ui_shell.h"

#include "gallery_model.h"
#include "theme.h"
#include "ui_fonts.h"
#include "ui_layout.h"

void beast2_ui_shell_draw_header(const Beast2UiRootLayout *layout) {
    const int x = (int) layout->header.x;
    const int y = (int) layout->header.y;
    const char *status = gallery_model_status_line();

    DrawText("Beast2", x, y, BEAST2_UI_FONT_TITLE_PX, BEAST2_UI_COLOR_TEXT_PRIMARY);
    DrawText(
        "Phase 3 — toolbar, browse path, refresh/columns/F toggle, multi-select, info bar",
        x,
        y + BEAST2_UI_FONT_TITLE_PX + 4,
        BEAST2_UI_FONT_SUBTITLE_PX,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
    if (status != NULL && status[0] != '\0') {
        DrawText(status, x, y + BEAST2_UI_FONT_TITLE_PX + 4 + BEAST2_UI_FONT_SUBTITLE_PX + 2, 12, BEAST2_UI_COLOR_TEXT_MUTED);
    }
}
