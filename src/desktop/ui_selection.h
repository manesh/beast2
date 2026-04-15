/*
 * Multi-select: single click, Ctrl toggle, Shift range from anchor.
 */
#ifndef BEAST2_UI_SELECTION_H
#define BEAST2_UI_SELECTION_H

#include <stddef.h>

void ui_selection_reset_for_count(size_t file_count);
void ui_selection_shutdown(void);
void ui_selection_clear(void);
void ui_selection_on_click(size_t file_index, int ctrl_held, int shift_held);
int ui_selection_is_selected(size_t file_index);
size_t ui_selection_first_selected(void);
size_t ui_selection_count(void);

#endif
