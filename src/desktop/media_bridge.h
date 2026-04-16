/*
 * SQLite media library handle for the desktop shell (tags, tag view).
 */
#ifndef BEAST2_MEDIA_BRIDGE_H
#define BEAST2_MEDIA_BRIDGE_H

#include "beast2/media_library.h"

#include <stddef.h>

int media_bridge_init(const char *workspace_root);
void media_bridge_shutdown(void);
int media_bridge_ready(void);

beast2_media_library_context *media_bridge_context(void);

void media_bridge_refresh_tag_names(void);
size_t media_bridge_tag_count(void);
const char *media_bridge_tag_name(size_t index);

int media_bridge_add_tag_to_absolute_file(
    const char *absolute_file_path,
    const char *tag_name,
    char *error_message,
    size_t error_message_size
);

#endif
