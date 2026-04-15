/*
 * Root layout: title header + toolbar + gallery scissor + info bar.
 */
#ifndef BEAST2_UI_LAYOUT_H
#define BEAST2_UI_LAYOUT_H

#include <raylib.h>

typedef struct Beast2UiRootLayout {
    Rectangle header;
    Rectangle toolbar;
    Rectangle gallery;
    Rectangle info_bar;
} Beast2UiRootLayout;

void beast2_ui_layout_root(int render_w, int render_h, Beast2UiRootLayout *out);

#endif
