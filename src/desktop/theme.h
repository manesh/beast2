/*
 * Single source of truth for desktop shell colors and spacing (raylib UI).
 */
#ifndef BEAST2_THEME_H
#define BEAST2_THEME_H

#include <raylib.h>

/* Backgrounds */
#define BEAST2_UI_COLOR_BG ((Color){18, 18, 24, 255})
#define BEAST2_UI_COLOR_PANEL ((Color){28, 28, 36, 255})
#define BEAST2_UI_COLOR_PANEL_BORDER ((Color){55, 58, 72, 255})
#define BEAST2_UI_COLOR_CARD ((Color){40, 42, 52, 255})
#define BEAST2_UI_COLOR_CARD_BORDER ((Color){70, 74, 90, 255})
#define BEAST2_UI_COLOR_TOOLBAR_BG ((Color){32, 34, 42, 255})
#define BEAST2_UI_COLOR_TOOLBAR_BORDER ((Color){55, 58, 72, 255})
#define BEAST2_UI_COLOR_TAB_ACTIVE ((Color){70, 120, 200, 255})
#define BEAST2_UI_COLOR_TAB_INACTIVE ((Color){50, 52, 62, 255})
#define BEAST2_UI_COLOR_SELECTION_RING ((Color){120, 180, 255, 255})

/* Text */
#define BEAST2_UI_COLOR_TEXT_PRIMARY RAYWHITE
#define BEAST2_UI_COLOR_TEXT_MUTED ((Color){160, 160, 175, 255})
#define BEAST2_UI_COLOR_TEXT_ON_CARD ((Color){200, 200, 210, 255})

/* Scrollbar */
#define BEAST2_UI_COLOR_SCROLL_TRACK ((Color){45, 45, 55, 255})
#define BEAST2_UI_COLOR_SCROLL_THUMB ((Color){120, 125, 150, 255})

/* Layout (screen space, pixels) */
#define BEAST2_UI_MARGIN_H 24.0f
#define BEAST2_UI_MARGIN_V 18.0f
#define BEAST2_UI_HEADER_BLOCK_H 72.0f
#define BEAST2_UI_TOOLBAR_H 42.0f
#define BEAST2_UI_INFO_BAR_H 30.0f
#define BEAST2_UI_GALLERY_BOTTOM_PAD 20.0f

#endif
