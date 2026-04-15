/*
 * Scrollable gallery strip — layout prototype for the Beast2 desktop shell.
 */
#include "ui_gallery.h"

#include "theme.h"
#include "ui_fonts.h"

#include <math.h>
#include <stdio.h>

enum {
    kGalleryCols = 3,
    kGalleryGap = 12,
    kGalleryPad = 16,
    kGalleryItems = 24,
    kCardW = 140,
    kCardH = 120,
};

static Texture2D s_placeholder;
static float s_scroll_y;

static float gallery_content_height(void) {
    const int rows = (kGalleryItems + kGalleryCols - 1) / kGalleryCols;
    return (float)(kGalleryPad * 2 + rows * (kCardH + kGalleryGap) - kGalleryGap);
}

void ui_gallery_init(void) {
    Image img = GenImageChecked(64, 64, 8, 8, (Color){90, 70, 120, 255}, BEAST2_UI_COLOR_CARD);
    s_placeholder = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(s_placeholder, TEXTURE_FILTER_BILINEAR);
    s_scroll_y = 0.0f;
}

void ui_gallery_shutdown(void) {
    UnloadTexture(s_placeholder);
}

void ui_gallery_update(float wheel_move) {
    /* wheel_move is typically ±1 per notch from GetMouseWheelMove() */
    s_scroll_y -= wheel_move * 48.0f;
}

void ui_gallery_draw(Rectangle panel_bounds) {
    const float inner_h = panel_bounds.height - (float)kGalleryPad * 2.0f;
    const float content_h = gallery_content_height() - (float)kGalleryPad * 2.0f;
    const float max_scroll = (content_h > inner_h) ? (content_h - inner_h) : 0.0f;
    if (s_scroll_y < 0.0f) {
        s_scroll_y = 0.0f;
    }
    if (s_scroll_y > max_scroll) {
        s_scroll_y = max_scroll;
    }

    DrawRectangleRec(panel_bounds, BEAST2_UI_COLOR_PANEL);
    DrawRectangleLinesEx(panel_bounds, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);

    const int sx = (int)floorf(panel_bounds.x);
    const int sy = (int)floorf(panel_bounds.y);
    const int sw = (int)floorf(panel_bounds.width);
    const int sh = (int)floorf(panel_bounds.height);
    BeginScissorMode(sx, sy, sw, sh);

    for (int i = 0; i < kGalleryItems; i++) {
        const int col = i % kGalleryCols;
        const int row = i / kGalleryCols;
        const float x = panel_bounds.x + (float)kGalleryPad + (float)col * (float)(kCardW + kGalleryGap);
        const float y = panel_bounds.y + (float)kGalleryPad + (float)row * (float)(kCardH + kGalleryGap) - s_scroll_y;
        const Rectangle card = {x, y, (float)kCardW, (float)kCardH};

        DrawRectangleRounded(card, 6.0f, 8, BEAST2_UI_COLOR_CARD);
        DrawRectangleRoundedLinesEx(card, 6.0f, 8, 1.0f, BEAST2_UI_COLOR_CARD_BORDER);

        if (i == 0) {
            const float tw = (float)s_placeholder.width * 2.0f;
            const float th = (float)s_placeholder.height * 2.0f;
            const float tx = x + (float)kCardW * 0.5f - tw * 0.5f;
            const float ty = y + (float)kCardH * 0.5f - th * 0.5f;
            DrawTextureEx(s_placeholder, (Vector2){tx, ty}, 0.0f, 2.0f, WHITE);
        } else {
            char label[24];
            (void)snprintf(label, sizeof label, "%d", i);
            DrawText(label, (int)(x + 10), (int)(y + 10), BEAST2_UI_FONT_BODY_PX, BEAST2_UI_COLOR_TEXT_ON_CARD);
        }
    }

    EndScissorMode();

    if (max_scroll > 1.0f) {
        const float track_h = panel_bounds.height - 24.0f;
        const float thumb_h = fmaxf(24.0f, track_h * (inner_h / content_h));
        const float t = (max_scroll > 0.0f) ? (s_scroll_y / max_scroll) : 0.0f;
        const float thumb_y = panel_bounds.y + 12.0f + (track_h - thumb_h) * t;
        const float bar_x = panel_bounds.x + panel_bounds.width - 10.0f;
        DrawRectangle((int)bar_x, (int)panel_bounds.y + 12, 6, (int)track_h, BEAST2_UI_COLOR_SCROLL_TRACK);
        DrawRectangle((int)bar_x, (int)thumb_y, 6, (int)thumb_h, BEAST2_UI_COLOR_SCROLL_THUMB);
    }
}
