#ifndef BEAST2_KNOWLEDGE_DB_H
#define BEAST2_KNOWLEDGE_DB_H

#include <stddef.h>

#include "beast2/media_library.h"

int beast2_knowledge_db_init(
    beast2_media_library_context *context,
    char *error_message,
    size_t error_message_size
);

int beast2_knowledge_db_fetch_context(
    beast2_media_library_context *context,
    const char *source_name,
    const char *terms,
    const char *mode,
    char *buffer,
    size_t buffer_size,
    char *error_message,
    size_t error_message_size
);

#endif
