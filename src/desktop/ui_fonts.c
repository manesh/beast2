/*
 * Placeholder for LoadFontEx / custom UI font (Phase 2+). Default font is active now.
 */
#include "ui_fonts.h"

#include "ui_scale.h"

void beast2_ui_fonts_init(void) {}

void beast2_ui_fonts_shutdown(void) {}

void beast2_ui_fonts_begin_frame(void) {
    ui_scale_begin_frame();
}
