/*
 * Compute panel rectangles from window size.
 */
#include "ui_layout.h"

#include "theme.h"

void beast2_ui_layout_root(int render_w, int render_h, int show_tag_strip, Beast2UiRootLayout *out) {
    const float rw = (float) render_w;
    const float rh = (float) render_h;
    const float inner_w = rw - BEAST2_UI_MARGIN_H * 2.0f;
    const float tag_h = show_tag_strip ? BEAST2_UI_TAG_STRIP_H : 0.0f;
    float y = BEAST2_UI_MARGIN_V;

    out->header.x = BEAST2_UI_MARGIN_H;
    out->header.y = y;
    out->header.width = inner_w;
    out->header.height = BEAST2_UI_HEADER_BLOCK_H;
    y += BEAST2_UI_HEADER_BLOCK_H;

    out->toolbar.x = BEAST2_UI_MARGIN_H;
    out->toolbar.y = y;
    out->toolbar.width = inner_w;
    out->toolbar.height = BEAST2_UI_TOOLBAR_H;
    y += BEAST2_UI_TOOLBAR_H;

    out->tag_strip.x = BEAST2_UI_MARGIN_H;
    out->tag_strip.y = y;
    out->tag_strip.width = inner_w;
    out->tag_strip.height = tag_h;
    y += tag_h;

    out->info_bar.x = BEAST2_UI_MARGIN_H;
    out->info_bar.width = inner_w;
    out->info_bar.height = BEAST2_UI_INFO_BAR_H;
    out->info_bar.y = rh - BEAST2_UI_GALLERY_BOTTOM_PAD - BEAST2_UI_INFO_BAR_H;

    out->gallery.x = BEAST2_UI_MARGIN_H;
    out->gallery.y = y;
    out->gallery.width = inner_w;
    out->gallery.height = out->info_bar.y - y;
    if (out->gallery.height < 60.0f) {
        out->gallery.height = 60.0f;
    }
}
