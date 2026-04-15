/*
 * Phase 2: gallery file list from beast2 config workspace (sorted media files).
 */
#ifndef BEAST2_GALLERY_MODEL_H
#define BEAST2_GALLERY_MODEL_H

#include <stddef.h>

void gallery_model_init(const char *config_path);
/*
 * is_folder_view: list files from the browse directory. Otherwise list files tagged with active_tag_name.
 */
void gallery_model_reload(int is_folder_view, const char *active_tag_name);
void gallery_model_shutdown(void);

size_t gallery_model_file_count(void);
const char *gallery_model_full_path(size_t index);
const char *gallery_model_basename(size_t index);
const char *gallery_model_browse_root(void);
/*
 * Workspace root from the last successful config load (else ".").
 */
const char *gallery_model_workspace_root(void);
const char *gallery_model_status_line(void);
int gallery_model_file_is_image(size_t index);

#endif
