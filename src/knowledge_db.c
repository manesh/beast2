#include "beast2/knowledge_db.h"

#include <ctype.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void beast2_knowledge_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static int beast2_knowledge_prepare_statement(
    sqlite3 *db,
    sqlite3_stmt **statement,
    const char *sql,
    char *error_message,
    size_t error_message_size
) {
    if (sqlite3_prepare_v2(db, sql, -1, statement, NULL) != SQLITE_OK) {
        snprintf(error_message, error_message_size, "sqlite prepare error: %s", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

static int beast2_knowledge_step_done(
    sqlite3 *db,
    sqlite3_stmt *statement,
    char *error_message,
    size_t error_message_size
) {
    int code = sqlite3_step(statement);

    if (code != SQLITE_DONE) {
        snprintf(error_message, error_message_size, "sqlite step error: %s", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

static int beast2_knowledge_contains_ignore_case(const char *haystack, const char *needle) {
    size_t start = 0;
    size_t needle_length = 0;

    if (haystack == NULL || needle == NULL) {
        return 0;
    }

    needle_length = strlen(needle);
    if (needle_length == 0) {
        return 1;
    }

    for (start = 0; haystack[start] != '\0'; start++) {
        size_t index = 0;

        while (
            haystack[start + index] != '\0' &&
            needle[index] != '\0' &&
            tolower((unsigned char) haystack[start + index]) == tolower((unsigned char) needle[index])
        ) {
            index++;
        }

        if (index == needle_length) {
            return 1;
        }
    }

    return 0;
}

static int beast2_knowledge_upsert_source(
    sqlite3 *db,
    const char *source_name,
    const char *source_kind,
    const char *description,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;

    if (
        beast2_knowledge_prepare_statement(
            db,
            &statement,
            "INSERT INTO knowledge_sources (source_name, source_kind, description) VALUES (?, ?, ?) "
            "ON CONFLICT(source_name) DO UPDATE SET source_kind=excluded.source_kind, description=excluded.description;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_text(statement, 1, source_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, source_kind, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, description, -1, SQLITE_TRANSIENT);

    if (beast2_knowledge_step_done(db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return -1;
    }

    sqlite3_finalize(statement);
    return 0;
}

static long long beast2_knowledge_lookup_source_id(
    sqlite3 *db,
    const char *source_name,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    long long source_id = 0;

    if (
        beast2_knowledge_prepare_statement(
            db,
            &statement,
            "SELECT source_id FROM knowledge_sources WHERE source_name = ?;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return 0;
    }

    sqlite3_bind_text(statement, 1, source_name, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) == SQLITE_ROW) {
        source_id = sqlite3_column_int64(statement, 0);
    } else {
        snprintf(error_message, error_message_size, "knowledge source not found: %s", source_name);
    }

    sqlite3_finalize(statement);
    return source_id;
}

static int beast2_knowledge_upsert_entry(
    sqlite3 *db,
    long long source_id,
    const char *entry_key,
    const char *title,
    const char *body,
    const char *keywords,
    const char *tags,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;

    if (
        beast2_knowledge_prepare_statement(
            db,
            &statement,
            "INSERT INTO knowledge_entries (source_id, entry_key, title, body, keywords, tags) "
            "VALUES (?, ?, ?, ?, ?, ?) "
            "ON CONFLICT(source_id, entry_key) DO UPDATE SET "
            "title=excluded.title, body=excluded.body, keywords=excluded.keywords, tags=excluded.tags;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_int64(statement, 1, source_id);
    sqlite3_bind_text(statement, 2, entry_key, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, title, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, body, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, keywords, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 6, tags, -1, SQLITE_TRANSIENT);

    if (beast2_knowledge_step_done(db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return -1;
    }

    sqlite3_finalize(statement);
    return 0;
}

static int beast2_knowledge_seed_defaults(
    beast2_media_library_context *context,
    char *error_message,
    size_t error_message_size
) {
    long long encyclopedia_id = 0;
    long long belief_id = 0;
    long long prompt_library_id = 0;

    if (
        beast2_knowledge_upsert_source(
            context->db,
            "local_encyclopedia",
            "encyclopedia",
            "Offline encyclopedia articles for LLM context.",
            error_message,
            error_message_size
        ) != 0 ||
        beast2_knowledge_upsert_source(
            context->db,
            "belief_db",
            "belief_db",
            "Belief conditioning rules for LLM responses.",
            error_message,
            error_message_size
        ) != 0 ||
        beast2_knowledge_upsert_source(
            context->db,
            "prompt_library",
            "prompt_library",
            "Reusable prompt snippets and styles.",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    encyclopedia_id = beast2_knowledge_lookup_source_id(context->db, "local_encyclopedia", error_message, error_message_size);
    belief_id = beast2_knowledge_lookup_source_id(context->db, "belief_db", error_message, error_message_size);
    prompt_library_id = beast2_knowledge_lookup_source_id(context->db, "prompt_library", error_message, error_message_size);

    if (encyclopedia_id == 0 || belief_id == 0 || prompt_library_id == 0) {
        return -1;
    }

    if (
        beast2_knowledge_upsert_entry(
            context->db,
            encyclopedia_id,
            "moonlit-fortress",
            "Moonlit Fortress",
            "A fortress on a sea cliff is often associated with moonlit stone walls, storm winds, and cold blue rim lighting.",
            "fortress moonlit cliff lighting storm",
            "fortress,moonlit,cliff",
            error_message,
            error_message_size
        ) != 0 ||
        beast2_knowledge_upsert_entry(
            context->db,
            encyclopedia_id,
            "cyberpunk-city",
            "Cyberpunk City",
            "Cyberpunk skylines emphasize neon signage, reflective wet streets, layered towers, and magenta-cyan contrast.",
            "cyberpunk city neon skyline wet streets",
            "cyberpunk,city,neon",
            error_message,
            error_message_size
        ) != 0 ||
        beast2_knowledge_upsert_entry(
            context->db,
            encyclopedia_id,
            "latent-space",
            "Latent Space",
            "Latent space interpolation allows smooth transitions between visual concepts and stylistic anchors.",
            "latent space interpolation style anchors",
            "latent,interpolation,style",
            error_message,
            error_message_size
        ) != 0 ||
        beast2_knowledge_upsert_entry(
            context->db,
            belief_id,
            "offline-first",
            "Offline First",
            "Prefer private local execution, reproducibility, and no cloud dependency.",
            "offline private local reproducibility",
            "offline,local,privacy",
            error_message,
            error_message_size
        ) != 0 ||
        beast2_knowledge_upsert_entry(
            context->db,
            belief_id,
            "cinematic-realism",
            "Cinematic Realism",
            "Favor believable lighting, grounded materials, and dramatic but plausible camera language.",
            "cinematic realism lighting materials camera",
            "cinematic,realism,lighting",
            error_message,
            error_message_size
        ) != 0 ||
        beast2_knowledge_upsert_entry(
            context->db,
            prompt_library_id,
            "painterly-concept",
            "Painterly Concept Art",
            "Style: painterly concept art, brushwork emphasis, layered atmosphere",
            "painterly concept art atmosphere",
            "painterly,concept-art,atmosphere",
            error_message,
            error_message_size
        ) != 0 ||
        beast2_knowledge_upsert_entry(
            context->db,
            prompt_library_id,
            "neon-dusk",
            "Neon Dusk",
            "Style: neon dusk haze, cinematic reflections, moody ambient glow",
            "neon dusk cinematic glow",
            "neon,dusk,cinematic",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    return 0;
}

int beast2_knowledge_db_init(
    beast2_media_library_context *context,
    char *error_message,
    size_t error_message_size
) {
    static const char *schema_sql =
        "CREATE TABLE IF NOT EXISTS knowledge_sources ("
        "  source_id INTEGER PRIMARY KEY,"
        "  source_name TEXT NOT NULL UNIQUE,"
        "  source_kind TEXT NOT NULL,"
        "  description TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS knowledge_entries ("
        "  entry_id INTEGER PRIMARY KEY,"
        "  source_id INTEGER NOT NULL,"
        "  entry_key TEXT NOT NULL,"
        "  title TEXT NOT NULL,"
        "  body TEXT NOT NULL,"
        "  keywords TEXT,"
        "  tags TEXT,"
        "  UNIQUE(source_id, entry_key),"
        "  FOREIGN KEY(source_id) REFERENCES knowledge_sources(source_id)"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_knowledge_entries_source_id ON knowledge_entries(source_id);"
        "CREATE INDEX IF NOT EXISTS idx_knowledge_entries_title ON knowledge_entries(title);";
    char *sqlite_error = NULL;

    if (sqlite3_exec(context->db, schema_sql, NULL, NULL, &sqlite_error) != SQLITE_OK) {
        snprintf(error_message, error_message_size, "sqlite error: %s", sqlite_error != NULL ? sqlite_error : "unknown");
        sqlite3_free(sqlite_error);
        return -1;
    }

    return beast2_knowledge_seed_defaults(context, error_message, error_message_size);
}

int beast2_knowledge_db_fetch_context(
    beast2_media_library_context *context,
    const char *source_name,
    const char *terms,
    const char *mode,
    char *buffer,
    size_t buffer_size,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    long long source_id = 0;
    size_t length = 0;
    int matched_rows = 0;
    char terms_buffer[4096];
    char *tokens[16];
    size_t token_count = 0;
    char *token = NULL;
    char *cursor = NULL;

    buffer[0] = '\0';

    if (context == NULL || context->db == NULL || source_name == NULL || *source_name == '\0') {
        beast2_knowledge_set_error(error_message, error_message_size, "knowledge source is required");
        return -1;
    }

    source_id = beast2_knowledge_lookup_source_id(context->db, source_name, error_message, error_message_size);
    if (source_id == 0) {
        return -1;
    }

    if (terms != NULL && *terms != '\0') {
        snprintf(terms_buffer, sizeof(terms_buffer), "%s", terms);
        cursor = terms_buffer;

        while ((token = strtok(cursor, " \t\r\n,.:;!?()[]{}<>/\\\"'")) != NULL && token_count < 16) {
            tokens[token_count++] = token;
            cursor = NULL;
        }
    }

    if (
        beast2_knowledge_prepare_statement(
            context->db,
            &statement,
            "SELECT title, body, keywords FROM knowledge_entries WHERE source_id = ? ORDER BY entry_id;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_int64(statement, 1, source_id);
    length += (size_t) snprintf(buffer + length, buffer_size - length, "knowledge_source: %s\n", source_name);

    while (sqlite3_step(statement) == SQLITE_ROW && matched_rows < 4) {
        const char *title = (const char *) sqlite3_column_text(statement, 0);
        const char *body = (const char *) sqlite3_column_text(statement, 1);
        const char *keywords = (const char *) sqlite3_column_text(statement, 2);
        int matches = token_count == 0 ? 1 : 0;
        size_t index = 0;

        for (index = 0; index < token_count && !matches; index++) {
            if (
                beast2_knowledge_contains_ignore_case(title, tokens[index]) ||
                beast2_knowledge_contains_ignore_case(body, tokens[index]) ||
                beast2_knowledge_contains_ignore_case(keywords, tokens[index])
            ) {
                matches = 1;
            }
        }

        if (!matches) {
            continue;
        }

        if (mode != NULL && strcmp(mode, "prompt_library_query") == 0) {
            length += (size_t) snprintf(
                buffer + length,
                buffer_size - length,
                "prompt_snippet_%d: %s\n",
                matched_rows + 1,
                body != NULL ? body : ""
            );
        } else if (mode != NULL && strcmp(mode, "belief_conditioning") == 0) {
            length += (size_t) snprintf(
                buffer + length,
                buffer_size - length,
                "belief_%d_title: %s\nbelief_%d_body: %s\n",
                matched_rows + 1,
                title != NULL ? title : "",
                matched_rows + 1,
                body != NULL ? body : ""
            );
        } else {
            length += (size_t) snprintf(
                buffer + length,
                buffer_size - length,
                "entry_%d_title: %s\nentry_%d_body: %s\n",
                matched_rows + 1,
                title != NULL ? title : "",
                matched_rows + 1,
                body != NULL ? body : ""
            );
        }

        if (length >= buffer_size) {
            sqlite3_finalize(statement);
            beast2_knowledge_set_error(error_message, error_message_size, "knowledge context exceeded supported length");
            return -1;
        }

        matched_rows++;
    }

    sqlite3_finalize(statement);

    if (matched_rows == 0) {
        snprintf(buffer, buffer_size, "knowledge_source: %s\nno_matches: true\n", source_name);
    }

    return 0;
}
