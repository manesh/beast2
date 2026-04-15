/*
 * Root window layout: header band + main gallery panel.
 */
#ifndef BEAST2_UI_LAYOUT_H
#define BEAST2_UI_LAYOUT_H

#include <raylib.h>

typedef struct Beast2UiRootLayout {
    Rectangle header;
    Rectangle gallery;
} Beast2UiRootLayout;

void beast2_ui_layout_root(int render_w, int render_h, Beast2UiRootLayout *out);

#endif
