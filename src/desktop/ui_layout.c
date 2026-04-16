/*
 * Compute panel rectangles from window size.
 */
#include "ui_layout.h"

#include "theme.h"
#include "ui_scale.h"

void beast2_ui_layout_root(
    int render_w,
    int render_h,
    int show_tag_strip,
    int show_sidebar,
    float sidebar_w,
    Beast2UiRootLayout *out
) {
    const float rw = (float) render_w;
    const float rh = (float) render_h;
    const float mh = ui_scale_layout_px(BEAST2_UI_MARGIN_H);
    const float mv = ui_scale_layout_px(BEAST2_UI_MARGIN_V);
    const float header_h = ui_scale_layout_px(BEAST2_UI_HEADER_BLOCK_H);
    const float toolbar_h = ui_scale_layout_px(BEAST2_UI_TOOLBAR_H);
    const float tag_strip_h = ui_scale_layout_px(BEAST2_UI_TAG_STRIP_H);
    const float info_h = ui_scale_layout_px(BEAST2_UI_INFO_BAR_H);
    const float gallery_pad = ui_scale_layout_px(BEAST2_UI_GALLERY_BOTTOM_PAD);
    const float inner_w = rw - mh * 2.0f;
    const float tag_h = show_tag_strip ? tag_strip_h : 0.0f;
    const float gap = ui_scale_layout_px(8.0f);
    float sw = (show_sidebar && sidebar_w > 1.0f) ? sidebar_w : 0.0f;
    float gallery_w = inner_w;
    float y = mv;

    if (sw > 0.0f) {
        gallery_w = inner_w - sw - gap;
        if (gallery_w < 120.0f) {
            gallery_w = 120.0f;
            sw = inner_w - gallery_w - gap;
            if (sw < 1.0f) {
                sw = 0.0f;
                gallery_w = inner_w;
            }
        }
    }

    out->header.x = mh;
    out->header.y = y;
    out->header.width = inner_w;
    out->header.height = header_h;
    y += header_h;

    out->toolbar.x = mh;
    out->toolbar.y = y;
    out->toolbar.width = inner_w;
    out->toolbar.height = toolbar_h;
    y += toolbar_h;

    out->tag_strip.x = mh;
    out->tag_strip.y = y;
    out->tag_strip.width = inner_w;
    out->tag_strip.height = tag_h;
    y += tag_h;

    out->info_bar.x = mh;
    out->info_bar.width = inner_w;
    out->info_bar.height = info_h;
    out->info_bar.y = rh - gallery_pad - info_h;

    out->gallery.x = mh;
    out->gallery.y = y;
    out->gallery.width = gallery_w;
    out->gallery.height = out->info_bar.y - y;
    if (out->gallery.height < 60.0f) {
        out->gallery.height = 60.0f;
    }

    out->sidebar.width = sw;
    out->sidebar.height = out->gallery.height;
    out->sidebar.y = y;
    out->sidebar.x = mh + gallery_w + (sw > 0.0f ? gap : 0.0f);
}
