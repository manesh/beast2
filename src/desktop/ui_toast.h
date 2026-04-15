/*
 * Non-blocking transient messages (Phase 7 error/success UX).
 */
#ifndef BEAST2_UI_TOAST_H
#define BEAST2_UI_TOAST_H

#include <raylib.h>

void ui_toast_init(void);
void ui_toast_begin_frame(float delta_seconds);
void ui_toast_show(const char *message, float duration_sec);
void ui_toast_draw(const Rectangle *above_info_bar, float screen_w);

#endif
