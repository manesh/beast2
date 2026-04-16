/*
 * Title header (toolbar lives in ui_chrome).
 */
#include "ui_shell.h"

#include "gallery_model.h"
#include "theme.h"
#include "ui_draw.h"
#include "ui_fonts.h"
#include "ui_layout.h"
#include "ui_scale.h"

void beast2_ui_shell_draw_header(const Beast2UiRootLayout *layout) {
    float x = layout->header.x;
    float y = layout->header.y;
    const char *status = gallery_model_status_line();

    beast2_ui_draw_text("Beast2", x, y, BEAST2_UI_FONT_TITLE_PX, BEAST2_UI_COLOR_TEXT_PRIMARY);
    y += (float) ui_scale_font_px(BEAST2_UI_FONT_TITLE_PX) + ui_scale_layout_px(4.0f);
    beast2_ui_draw_text(
        "Phase 7 — DPI-aware UI, shortcuts, virtualized gallery, toasts",
        x,
        y,
        BEAST2_UI_FONT_SUBTITLE_PX,
        BEAST2_UI_COLOR_TEXT_MUTED
    );
    y += (float) ui_scale_font_px(BEAST2_UI_FONT_SUBTITLE_PX) + ui_scale_layout_px(2.0f);
    if (status != NULL && status[0] != '\0') {
        beast2_ui_draw_text(status, x, y, 12, BEAST2_UI_COLOR_TEXT_MUTED);
    }
}
