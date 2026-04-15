/*
 * Compute panel rectangles from window size (fixed margins; Phase 7 may add DPI scaling).
 */
#include "ui_layout.h"

#include "theme.h"

void beast2_ui_layout_root(int render_w, int render_h, Beast2UiRootLayout *out) {
    const float rw = (float)render_w;
    const float rh = (float)render_h;
    const float inner_w = rw - BEAST2_UI_MARGIN_H * 2.0f;

    out->header.x = BEAST2_UI_MARGIN_H;
    out->header.y = BEAST2_UI_MARGIN_V;
    out->header.width = inner_w;
    out->header.height = BEAST2_UI_HEADER_BLOCK_H;

    out->gallery.x = BEAST2_UI_MARGIN_H;
    out->gallery.y = BEAST2_UI_MARGIN_V + BEAST2_UI_HEADER_BLOCK_H;
    out->gallery.width = inner_w;
    out->gallery.height = rh - out->gallery.y - BEAST2_UI_GALLERY_BOTTOM_PAD;
    if (out->gallery.height < 80.0f) {
        out->gallery.height = 80.0f;
    }
}
