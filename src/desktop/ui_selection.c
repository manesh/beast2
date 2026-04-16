/*
 * Selection state for gallery grid indices.
 */
#include "ui_selection.h"

#include <stdlib.h>
#include <string.h>

#include <raylib.h>

static unsigned char *s_selected = NULL;
static size_t s_file_count = 0;
static size_t s_anchor = 0;

static void selection_fill_range(size_t lo, size_t hi) {
    size_t i;

    if (lo > hi) {
        size_t t = lo;
        lo = hi;
        hi = t;
    }

    for (i = lo; i <= hi && i < s_file_count; i++) {
        s_selected[i] = 1;
    }
}

void ui_selection_reset_for_count(size_t file_count) {
    free(s_selected);
    s_selected = NULL;
    s_file_count = file_count;
    s_anchor = 0;

    if (file_count == 0) {
        return;
    }

    s_selected = (unsigned char *) calloc(file_count, 1);
}

void ui_selection_shutdown(void) {
    free(s_selected);
    s_selected = NULL;
    s_file_count = 0;
    s_anchor = 0;
}

void ui_selection_clear(void) {
    if (s_selected != NULL && s_file_count > 0) {
        memset(s_selected, 0, s_file_count);
    }
}

void ui_selection_on_click(size_t file_index, int ctrl_held, int shift_held) {
    if (file_index >= s_file_count || s_selected == NULL) {
        return;
    }

    if (shift_held && s_file_count > 0) {
        ui_selection_clear();
        selection_fill_range(s_anchor, file_index);
        return;
    }

    if (ctrl_held) {
        s_selected[file_index] = (unsigned char) (s_selected[file_index] ? 0 : 1);
        s_anchor = file_index;
        return;
    }

    ui_selection_clear();
    s_selected[file_index] = 1;
    s_anchor = file_index;
}

int ui_selection_is_selected(size_t file_index) {
    if (file_index >= s_file_count || s_selected == NULL) {
        return 0;
    }

    return s_selected[file_index] != 0;
}

size_t ui_selection_first_selected(void) {
    size_t i;

    if (s_selected == NULL) {
        return (size_t) -1;
    }

    for (i = 0; i < s_file_count; i++) {
        if (s_selected[i]) {
            return i;
        }
    }

    return (size_t) -1;
}

size_t ui_selection_count(void) {
    size_t i;
    size_t n = 0;

    if (s_selected == NULL) {
        return 0;
    }

    for (i = 0; i < s_file_count; i++) {
        if (s_selected[i]) {
            n++;
        }
    }

    return n;
}

void ui_selection_for_each_selected(ui_selection_each_fn fn, void *user) {
    size_t i;

    if (fn == NULL || s_selected == NULL) {
        return;
    }

    for (i = 0; i < s_file_count; i++) {
        if (s_selected[i]) {
            fn(i, user);
        }
    }
}

void ui_selection_keyboard_nav_grid(int cols, size_t file_count) {
    size_t cur;

    if (file_count == 0 || cols < 1 || s_selected == NULL) {
        return;
    }

    cur = ui_selection_first_selected();
    if (cur == (size_t) -1) {
        ui_selection_on_click(0, 0, 0);
        cur = 0;
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        if (cur + 1 < file_count) {
            ui_selection_on_click(cur + 1, 0, 0);
        }
    } else if (IsKeyPressed(KEY_LEFT)) {
        if (cur > 0) {
            ui_selection_on_click(cur - 1, 0, 0);
        }
    } else if (IsKeyPressed(KEY_DOWN)) {
        size_t n = cur + (size_t) cols;
        if (n < file_count) {
            ui_selection_on_click(n, 0, 0);
        }
    } else if (IsKeyPressed(KEY_UP)) {
        if (cur >= (size_t) cols) {
            ui_selection_on_click(cur - (size_t) cols, 0, 0);
        }
    } else if (IsKeyPressed(KEY_HOME)) {
        ui_selection_on_click(0, 0, 0);
    } else if (IsKeyPressed(KEY_END)) {
        ui_selection_on_click(file_count - 1, 0, 0);
    }
}
