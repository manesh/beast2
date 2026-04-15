/*
 * Typography scale for beast2_desktop (default raylib font until custom assets land).
 */
#ifndef BEAST2_UI_FONTS_H
#define BEAST2_UI_FONTS_H

enum {
    BEAST2_UI_FONT_TITLE_PX = 26,
    BEAST2_UI_FONT_SUBTITLE_PX = 16,
    BEAST2_UI_FONT_BODY_PX = 18,
};

void beast2_ui_fonts_init(void);
void beast2_ui_fonts_shutdown(void);

/* Call once per frame before layout/draw (DPI scale). */
void beast2_ui_fonts_begin_frame(void);

#endif
