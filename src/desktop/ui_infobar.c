/*
 * Info bar — file size from stat; duration/resolution placeholders for Phase 2+ FFmpeg.
 */
#include "ui_infobar.h"

#include "beast2/c_compat.h"

#include "gallery_model.h"
#include "theme.h"
#include "ui_layout.h"
#include "ui_selection.h"

#include <stdio.h>
#include <sys/stat.h>

void ui_infobar_draw(const Beast2UiRootLayout *layout) {
    const Rectangle *b = &layout->info_bar;
    size_t idx = ui_selection_first_selected();
    struct stat st;

    DrawRectangleRec(*b, BEAST2_UI_COLOR_TOOLBAR_BG);
    DrawRectangleLinesEx(*b, 1.0f, BEAST2_UI_COLOR_TOOLBAR_BORDER);

    if (idx == (size_t) -1 || gallery_model_file_count() == 0) {
        DrawText(
            "No selection",
            (int) (b->x + 8.0f),
            (int) (b->y + 7.0f),
            14,
            BEAST2_UI_COLOR_TEXT_MUTED
        );
        return;
    }

    if (idx >= gallery_model_file_count()) {
        return;
    }

    {
        const char *path = gallery_model_full_path(idx);
        const char *base = gallery_model_basename(idx);
        char line[512];

        if (path == NULL || base == NULL) {
            return;
        }

        if (stat(path, &st) != 0) {
            snprintf(line, sizeof line, "%s  |  (stat failed)", base);
        } else {
            snprintf(
                line,
                sizeof line,
                "%s  |  %lld bytes  |  Duration: —  |  Resolution: —",
                base,
                (long long) st.st_size
            );
        }

        DrawText(line, (int) (b->x + 8.0f), (int) (b->y + 7.0f), 14, BEAST2_UI_COLOR_TEXT_ON_CARD);
    }
}
