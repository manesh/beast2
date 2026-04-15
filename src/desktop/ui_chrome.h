/*
 * Phase 3 toolbar: view tabs, breadcrumb, refresh, columns, filename toggle.
 */
#ifndef BEAST2_UI_CHROME_H
#define BEAST2_UI_CHROME_H

#include <raylib.h>
#include <stdbool.h>

struct Beast2UiRootLayout;

void ui_chrome_init(void);
void ui_chrome_draw(const struct Beast2UiRootLayout *layout);
/*
 * Returns true if a toolbar control handled the click (caller should not treat as gallery pick).
 */
bool ui_chrome_handle_click(Vector2 mouse, bool left_pressed, const struct Beast2UiRootLayout *layout);

int ui_chrome_get_columns(void);
int ui_chrome_show_filename(void);
int ui_chrome_is_folder_view(void);
const char *ui_chrome_get_active_tag(void);

#endif
