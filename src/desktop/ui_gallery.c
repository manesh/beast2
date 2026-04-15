/*
 * Scrollable gallery: real files, LRU thumbs, Phase 3 selection + column count.
 */
#include "ui_gallery.h"

#include "gallery_model.h"
#include "theme.h"
#include "ui_chrome.h"
#include "ui_fonts.h"
#include "ui_selection.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

enum {
    kGalleryGap = 12,
    kGalleryPad = 16,
    kCardW = 140,
    kCardH = 120,
    kThumbCacheSlots = 48,
    kScrollbarReserve = 14,
};

typedef struct {
    int in_use;
    size_t file_index;
    Texture2D tex;
    unsigned last_touch;
} ThumbSlot;

static Texture2D s_placeholder;
static float s_scroll_y;
static unsigned s_frame_counter;
static ThumbSlot s_thumb_cache[kThumbCacheSlots];
static int s_cols = 3;

static float gallery_content_height(size_t item_count) {
    if (item_count == 0) {
        return (float) kGalleryPad * 2.0f + 40.0f;
    }

    const int rows = (int) ((item_count + (size_t) s_cols - 1) / (size_t) s_cols);
    return (float) (kGalleryPad * 2 + rows * (kCardH + kGalleryGap) - kGalleryGap);
}

static void thumb_cache_evict_all(void) {
    size_t i;

    for (i = 0; i < kThumbCacheSlots; i++) {
        if (s_thumb_cache[i].in_use) {
            UnloadTexture(s_thumb_cache[i].tex);
            s_thumb_cache[i].in_use = 0;
        }
    }
}

static void thumb_cache_touch(size_t file_index, const char *full_path, int is_image, Texture2D *out_tex) {
    size_t i;
    size_t victim = 0;
    unsigned oldest = 0xffffffffu;
    Texture2D loaded;

    *out_tex = s_placeholder;

    if (full_path == NULL) {
        return;
    }

    if (!is_image) {
        return;
    }

    for (i = 0; i < kThumbCacheSlots; i++) {
        if (s_thumb_cache[i].in_use && s_thumb_cache[i].file_index == file_index) {
            s_thumb_cache[i].last_touch = s_frame_counter;
            *out_tex = s_thumb_cache[i].tex;
            return;
        }
    }

    for (i = 0; i < kThumbCacheSlots; i++) {
        if (!s_thumb_cache[i].in_use) {
            victim = i;
            oldest = 0;
            break;
        }
        if (s_thumb_cache[i].last_touch < oldest) {
            oldest = s_thumb_cache[i].last_touch;
            victim = i;
        }
    }

    if (s_thumb_cache[victim].in_use) {
        UnloadTexture(s_thumb_cache[victim].tex);
        s_thumb_cache[victim].in_use = 0;
    }

    loaded = LoadTexture(full_path);
    if (loaded.width <= 0 || loaded.height <= 0) {
        return;
    }

    SetTextureFilter(loaded, TEXTURE_FILTER_BILINEAR);
    s_thumb_cache[victim].tex = loaded;
    s_thumb_cache[victim].in_use = 1;
    s_thumb_cache[victim].file_index = file_index;
    s_thumb_cache[victim].last_touch = s_frame_counter;
    *out_tex = loaded;
}

void ui_gallery_set_column_count(int cols) {
    if (cols < 2) {
        cols = 2;
    }
    if (cols > 8) {
        cols = 8;
    }
    s_cols = cols;
}

void ui_gallery_reload_files(void) {
    gallery_model_reload(ui_chrome_is_folder_view(), ui_chrome_get_active_tag());
    thumb_cache_evict_all();
    ui_selection_reset_for_count(gallery_model_file_count());
    s_scroll_y = 0.0f;
}

void ui_gallery_init(const char *config_path) {
    Image img = GenImageChecked(64, 64, 8, 8, (Color){90, 70, 120, 255}, BEAST2_UI_COLOR_CARD);
    memset(s_thumb_cache, 0, sizeof s_thumb_cache);
    s_placeholder = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(s_placeholder, TEXTURE_FILTER_BILINEAR);
    s_scroll_y = 0.0f;
    s_frame_counter = 0;
    s_cols = ui_chrome_get_columns();
    gallery_model_init(config_path);
    ui_selection_reset_for_count(gallery_model_file_count());
}

void ui_gallery_shutdown(void) {
    thumb_cache_evict_all();
    UnloadTexture(s_placeholder);
    ui_selection_shutdown();
    gallery_model_shutdown();
}

void ui_gallery_update(float wheel_move) {
    s_frame_counter++;
    s_scroll_y -= wheel_move * 48.0f;
}

int ui_gallery_pick_file_index(Vector2 mouse, Rectangle panel_bounds) {
    const size_t item_count = gallery_model_file_count();
    float lx;
    float ly;
    int col;
    int row;
    float ox;
    float oy;
    size_t idx;

    if (item_count == 0) {
        return -1;
    }

    if (!CheckCollisionPointRec(mouse, panel_bounds)) {
        return -1;
    }

    if (mouse.x >= panel_bounds.x + panel_bounds.width - (float) kScrollbarReserve) {
        return -1;
    }

    lx = mouse.x - panel_bounds.x - (float) kGalleryPad;
    ly = mouse.y - panel_bounds.y - (float) kGalleryPad + s_scroll_y;
    if (lx < 0.0f || ly < 0.0f) {
        return -1;
    }

    col = (int) (lx / (float) (kCardW + kGalleryGap));
    row = (int) (ly / (float) (kCardH + kGalleryGap));
    if (col < 0 || col >= s_cols) {
        return -1;
    }

    ox = lx - (float) col * (float) (kCardW + kGalleryGap);
    oy = ly - (float) row * (float) (kCardH + kGalleryGap);
    if (ox > (float) kCardW || oy > (float) kCardH) {
        return -1;
    }

    idx = (size_t) row * (size_t) s_cols + (size_t) col;
    if (idx >= item_count) {
        return -1;
    }

    return (int) idx;
}

void ui_gallery_draw(Rectangle panel_bounds) {
    const size_t item_count = gallery_model_file_count();
    const float inner_h = panel_bounds.height - (float) kGalleryPad * 2.0f;
    const float content_h = gallery_content_height(item_count) - (float) kGalleryPad * 2.0f;
    const float max_scroll = (content_h > inner_h) ? (content_h - inner_h) : 0.0f;

    if (s_scroll_y < 0.0f) {
        s_scroll_y = 0.0f;
    }
    if (s_scroll_y > max_scroll) {
        s_scroll_y = max_scroll;
    }

    DrawRectangleRec(panel_bounds, BEAST2_UI_COLOR_PANEL);
    DrawRectangleLinesEx(panel_bounds, 1.0f, BEAST2_UI_COLOR_PANEL_BORDER);

    if (item_count == 0) {
        DrawText(
            "No media files here (png/jpg/webp/… or video extensions).",
            (int) (panel_bounds.x + (float) kGalleryPad),
            (int) (panel_bounds.y + (float) kGalleryPad),
            BEAST2_UI_FONT_BODY_PX,
            BEAST2_UI_COLOR_TEXT_MUTED
        );
        return;
    }

    {
        const int sx = (int) floorf(panel_bounds.x);
        const int sy = (int) floorf(panel_bounds.y);
        const int sw = (int) floorf(panel_bounds.width);
        const int sh = (int) floorf(panel_bounds.height);
        size_t i;

        BeginScissorMode(sx, sy, sw, sh);

        for (i = 0; i < item_count; i++) {
            const int col = (int) (i % (size_t) s_cols);
            const int row = (int) (i / (size_t) s_cols);
            const float x = panel_bounds.x + (float) kGalleryPad + (float) col * (float) (kCardW + kGalleryGap);
            const float y =
                panel_bounds.y + (float) kGalleryPad + (float) row * (float) (kCardH + kGalleryGap) - s_scroll_y;
            const Rectangle card = {x, y, (float) kCardW, (float) kCardH};
            const char *full = gallery_model_full_path(i);
            const char *base = gallery_model_basename(i);
            Texture2D thumb = s_placeholder;

            DrawRectangleRounded(card, 6.0f, 8, BEAST2_UI_COLOR_CARD);
            DrawRectangleRoundedLinesEx(card, 6.0f, 8, 1.0f, BEAST2_UI_COLOR_CARD_BORDER);

            if (ui_selection_is_selected(i)) {
                DrawRectangleRoundedLinesEx(card, 6.0f, 8, 2.5f, BEAST2_UI_COLOR_SELECTION_RING);
            }

            thumb_cache_touch(i, full, gallery_model_file_is_image(i), &thumb);

            {
                const float scale = fminf(
                    ((float) kCardW - 16.0f) / (float) thumb.width,
                    ((float) kCardH - 36.0f) / (float) thumb.height
                );
                const float tw = (float) thumb.width * scale;
                const float tx = x + (float) kCardW * 0.5f - tw * 0.5f;
                const float ty = y + 6.0f;
                DrawTextureEx(thumb, (Vector2){tx, ty}, 0.0f, scale, WHITE);
            }

            if (base != NULL && ui_chrome_show_filename()) {
                DrawText(
                    base,
                    (int) (x + 4),
                    (int) (y + (float) kCardH - 22.0f),
                    12,
                    BEAST2_UI_COLOR_TEXT_ON_CARD
                );
            }
        }

        EndScissorMode();
    }

    if (max_scroll > 1.0f && item_count > 0) {
        const float track_h = panel_bounds.height - 24.0f;
        const float thumb_h = fmaxf(24.0f, track_h * (inner_h / content_h));
        const float t = (max_scroll > 0.0f) ? (s_scroll_y / max_scroll) : 0.0f;
        const float thumb_y = panel_bounds.y + 12.0f + (track_h - thumb_h) * t;
        const float bar_x = panel_bounds.x + panel_bounds.width - 10.0f;
        DrawRectangle((int) bar_x, (int) panel_bounds.y + 12, 6, (int) track_h, BEAST2_UI_COLOR_SCROLL_TRACK);
        DrawRectangle((int) bar_x, (int) thumb_y, 6, (int) thumb_h, BEAST2_UI_COLOR_SCROLL_THUMB);
    }
}
