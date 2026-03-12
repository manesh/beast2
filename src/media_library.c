#include "beast2/media_library.h"

#include <errno.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "beast2/filesystem.h"

static void beast2_media_set_error(
    char *error_message,
    size_t error_message_size,
    const char *message
) {
    if (error_message == NULL || error_message_size == 0) {
        return;
    }

    snprintf(error_message, error_message_size, "%s", message);
}

static uint64_t beast2_media_hash_append(uint64_t hash, const char *value) {
    const unsigned char *cursor = (const unsigned char *) value;

    while (*cursor != '\0') {
        hash ^= (uint64_t) *cursor;
        hash *= 1099511628211ULL;
        cursor++;
    }

    return hash;
}

static void beast2_media_current_timestamp(char *destination, size_t destination_size) {
    time_t now = time(NULL);
    struct tm utc_time;
    struct tm *resolved = NULL;

    memset(&utc_time, 0, sizeof(utc_time));
#if defined(_WIN32)
    gmtime_s(&utc_time, &now);
#else
    resolved = gmtime(&now);
    if (resolved != NULL) {
        utc_time = *resolved;
    }
#endif
    strftime(destination, destination_size, "%Y-%m-%dT%H:%M:%SZ", &utc_time);
}

static int beast2_media_exec_sql(
    sqlite3 *db,
    const char *sql,
    char *error_message,
    size_t error_message_size
) {
    char *sqlite_error = NULL;

    if (sqlite3_exec(db, sql, NULL, NULL, &sqlite_error) != SQLITE_OK) {
        snprintf(
            error_message,
            error_message_size,
            "sqlite error: %s",
            sqlite_error != NULL ? sqlite_error : "unknown"
        );
        sqlite3_free(sqlite_error);
        return -1;
    }

    return 0;
}

static int beast2_media_prepare_statement(
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

static int beast2_media_step_done(
    sqlite3 *db,
    sqlite3_stmt *statement,
    char *error_message,
    size_t error_message_size
) {
    int step_code = sqlite3_step(statement);

    if (step_code != SQLITE_DONE) {
        snprintf(error_message, error_message_size, "sqlite step error: %s", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

static long long beast2_media_upsert_generator(
    sqlite3 *db,
    const beast2_media_record *record,
    char *error_message,
    size_t error_message_size
) {
    static const char *sql =
        "INSERT INTO generators (name, description, creation_date, source_path, engine, checkpoint) "
        "VALUES (?, ?, ?, ?, ?, ?) "
        "ON CONFLICT(name) DO UPDATE SET "
        "source_path=excluded.source_path, "
        "engine=excluded.engine, "
        "checkpoint=excluded.checkpoint;";
    sqlite3_stmt *statement = NULL;
    sqlite3_stmt *query_statement = NULL;
    char timestamp[64];
    long long generator_id = 0;

    beast2_media_current_timestamp(timestamp, sizeof(timestamp));

    if (beast2_media_prepare_statement(db, &statement, sql, error_message, error_message_size) != 0) {
        return 0;
    }

    sqlite3_bind_text(statement, 1, record->generator_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, "", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, timestamp, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, record->generator_source_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, record->engine, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 6, record->checkpoint, -1, SQLITE_TRANSIENT);

    if (beast2_media_step_done(db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return 0;
    }

    sqlite3_finalize(statement);

    if (
        beast2_media_prepare_statement(
            db,
            &query_statement,
            "SELECT generator_id FROM generators WHERE name = ?;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return 0;
    }

    sqlite3_bind_text(query_statement, 1, record->generator_name, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(query_statement) == SQLITE_ROW) {
        generator_id = sqlite3_column_int64(query_statement, 0);
    } else {
        snprintf(error_message, error_message_size, "failed to query generator id");
    }

    sqlite3_finalize(query_statement);
    return generator_id;
}

static int beast2_media_generate_thumbnail(
    beast2_media_library_context *context,
    const beast2_media_record *record,
    beast2_media_record_result *result,
    char *error_message,
    size_t error_message_size
) {
    const char *relative_path = record->output_relative_path;
    const char *slash = strrchr(relative_path, '/');
    const char *shard = NULL;
    char directory[BEAST2_MAX_PATH_LENGTH];
    char base_name[BEAST2_MAX_PATH_LENGTH];
    char stem[BEAST2_MAX_PATH_LENGTH];
    char contents[4096];
    size_t stem_length = 0;
    uint64_t hash = 1469598103934665603ULL;
    FILE *file = NULL;

    if (slash == NULL) {
        beast2_media_set_error(error_message, error_message_size, "invalid media path for thumbnail generation");
        return -1;
    }

    snprintf(directory, sizeof(directory), "%.*s", (int) (slash - relative_path), relative_path);
    snprintf(base_name, sizeof(base_name), "%s", slash + 1);
    shard = strrchr(directory, '/');
    shard = shard != NULL ? shard + 1 : directory;

    stem_length = strcspn(base_name, ".");
    snprintf(stem, sizeof(stem), "%.*s", (int) stem_length, base_name);

    if (strcmp(record->output_kind, "image") == 0) {
        if (
            snprintf(
                result->thumbnail_relative_path,
                sizeof(result->thumbnail_relative_path),
                "thumbs/images/%s/%s.ppm",
                shard,
                stem
            ) >= (int) sizeof(result->thumbnail_relative_path)
        ) {
            beast2_media_set_error(error_message, error_message_size, "thumbnail path exceeds supported length");
            return -1;
        }
    } else {
        if (
            snprintf(
                result->thumbnail_relative_path,
                sizeof(result->thumbnail_relative_path),
                "thumbs/%s/%s/%s.txt",
                strcmp(record->output_kind, "video_manifest") == 0 ? "videos" : "text",
                shard,
                stem
            ) >= (int) sizeof(result->thumbnail_relative_path)
        ) {
            beast2_media_set_error(error_message, error_message_size, "thumbnail path exceeds supported length");
            return -1;
        }
    }

    if (
        beast2_fs_join_path(
            result->thumbnail_path,
            sizeof(result->thumbnail_path),
            context->workspace_root,
            result->thumbnail_relative_path
        ) != 0
    ) {
        beast2_media_set_error(error_message, error_message_size, "failed to build thumbnail path");
        return -1;
    }

    if (strcmp(record->output_kind, "image") == 0) {
        hash = beast2_media_hash_append(hash, record->prompt_hash);
        hash = beast2_media_hash_append(hash, record->checkpoint);
        if (
            snprintf(
                contents,
                sizeof(contents),
                "P3\n"
                "# beast2_thumbnail\n"
                "# source %s\n"
                "2 2\n"
                "255\n"
                "%u %u %u\n"
                "%u %u %u\n"
                "%u %u %u\n"
                "%u %u %u\n",
                record->output_relative_path,
                (unsigned int) ((hash >> 8) & 0xff),
                (unsigned int) ((hash >> 16) & 0xff),
                (unsigned int) ((hash >> 24) & 0xff),
                (unsigned int) ((hash >> 12) & 0xff),
                (unsigned int) ((hash >> 20) & 0xff),
                (unsigned int) ((hash >> 28) & 0xff),
                (unsigned int) ((hash >> 4) & 0xff),
                (unsigned int) ((hash >> 10) & 0xff),
                (unsigned int) ((hash >> 18) & 0xff),
                (unsigned int) ((hash >> 2) & 0xff),
                (unsigned int) ((hash >> 14) & 0xff),
                (unsigned int) ((hash >> 22) & 0xff)
            ) >= (int) sizeof(contents)
        ) {
            beast2_media_set_error(error_message, error_message_size, "thumbnail contents exceeded supported length");
            return -1;
        }
    } else {
        if (
            snprintf(
                contents,
                sizeof(contents),
                "BEAST2_PREVIEW\n"
                "kind: %s\n"
                "source: %s\n"
                "generator: %s\n"
                "checkpoint: %s\n",
                record->output_kind,
                record->output_relative_path,
                record->generator_name,
                record->checkpoint
            ) >= (int) sizeof(contents)
        ) {
            beast2_media_set_error(error_message, error_message_size, "thumbnail contents exceeded supported length");
            return -1;
        }
    }

    {
        char parent_path[BEAST2_MAX_PATH_LENGTH];

        if (beast2_fs_parent_directory(result->thumbnail_path, parent_path, sizeof(parent_path)) != 0) {
            beast2_media_set_error(error_message, error_message_size, "failed to derive thumbnail directory");
            return -1;
        }

        if (beast2_fs_mkdirs(parent_path, error_message, error_message_size) != 0) {
            return -1;
        }
    }

    file = fopen(result->thumbnail_path, "w");

    if (file == NULL) {
        snprintf(error_message, error_message_size, "failed to open thumbnail file: %s", result->thumbnail_path);
        return -1;
    }

    if (fputs(contents, file) == EOF) {
        fclose(file);
        snprintf(error_message, error_message_size, "failed to write thumbnail file: %s", result->thumbnail_path);
        return -1;
    }

    if (fclose(file) != 0) {
        snprintf(error_message, error_message_size, "failed to close thumbnail file: %s", result->thumbnail_path);
        return -1;
    }

    return 0;
}

static long long beast2_media_lookup_media_id(
    sqlite3 *db,
    const char *path,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    long long media_id = 0;

    if (
        beast2_media_prepare_statement(
            db,
            &statement,
            "SELECT media_id FROM media WHERE path = ?;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return 0;
    }

    sqlite3_bind_text(statement, 1, path, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) == SQLITE_ROW) {
        media_id = sqlite3_column_int64(statement, 0);
    } else {
        snprintf(error_message, error_message_size, "failed to lookup media id");
    }

    sqlite3_finalize(statement);
    return media_id;
}

static int beast2_media_upsert_tag(
    sqlite3 *db,
    const char *tag_name,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    int result = 0;

    if (
        beast2_media_prepare_statement(
            db,
            &statement,
            "INSERT INTO tags (tag_name, tag_type) VALUES (?, ?) ON CONFLICT(tag_name) DO NOTHING;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_text(statement, 1, tag_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, "user", -1, SQLITE_TRANSIENT);

    result = beast2_media_step_done(db, statement, error_message, error_message_size);
    sqlite3_finalize(statement);
    return result;
}

static long long beast2_media_lookup_tag_id(
    sqlite3 *db,
    const char *tag_name,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    long long tag_id = 0;

    if (
        beast2_media_prepare_statement(
            db,
            &statement,
            "SELECT tag_id FROM tags WHERE tag_name = ?;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return 0;
    }

    sqlite3_bind_text(statement, 1, tag_name, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) == SQLITE_ROW) {
        tag_id = sqlite3_column_int64(statement, 0);
    } else {
        snprintf(error_message, error_message_size, "failed to lookup tag id");
    }

    sqlite3_finalize(statement);
    return tag_id;
}

static int beast2_media_insert_media_tags(
    sqlite3 *db,
    long long media_id,
    const char *tags_csv,
    char *error_message,
    size_t error_message_size
) {
    char buffer[2048];
    char *token = NULL;
    char *cursor = NULL;

    if (tags_csv == NULL || *tags_csv == '\0') {
        return 0;
    }

    if (strlen(tags_csv) >= sizeof(buffer)) {
        beast2_media_set_error(error_message, error_message_size, "tag list exceeds supported length");
        return -1;
    }

    snprintf(buffer, sizeof(buffer), "%s", tags_csv);
    cursor = buffer;

    while ((token = strtok(cursor, ",")) != NULL) {
        sqlite3_stmt *statement = NULL;
        char *trimmed = token;
        long long tag_id = 0;

        cursor = NULL;

        while (*trimmed == ' ') {
            trimmed++;
        }

        if (*trimmed == '\0') {
            continue;
        }

        if (beast2_media_upsert_tag(db, trimmed, error_message, error_message_size) != 0) {
            return -1;
        }

        tag_id = beast2_media_lookup_tag_id(db, trimmed, error_message, error_message_size);
        if (tag_id == 0) {
            return -1;
        }

        if (
            beast2_media_prepare_statement(
                db,
                &statement,
                "INSERT INTO media_tags (media_id, tag_id, confidence) VALUES (?, ?, ?) "
                "ON CONFLICT(media_id, tag_id) DO UPDATE SET confidence = excluded.confidence;",
                error_message,
                error_message_size
            ) != 0
        ) {
            return -1;
        }

        sqlite3_bind_int64(statement, 1, media_id);
        sqlite3_bind_int64(statement, 2, tag_id);
        sqlite3_bind_double(statement, 3, 1.0);

        if (beast2_media_step_done(db, statement, error_message, error_message_size) != 0) {
            sqlite3_finalize(statement);
            return -1;
        }

        sqlite3_finalize(statement);
    }

    return 0;
}

static int beast2_media_insert_generator_history(
    sqlite3 *db,
    long long generator_id,
    long long media_id,
    const char *artifact_relative_path,
    char *error_message,
    size_t error_message_size
) {
    sqlite3_stmt *statement = NULL;
    char timestamp[64];

    beast2_media_current_timestamp(timestamp, sizeof(timestamp));

    if (
        beast2_media_prepare_statement(
            db,
            &statement,
            "INSERT INTO generator_history (generator_id, media_id, artifact_path, created_time) VALUES (?, ?, ?, ?);",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_int64(statement, 1, generator_id);
    sqlite3_bind_int64(statement, 2, media_id);
    sqlite3_bind_text(statement, 3, artifact_relative_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, timestamp, -1, SQLITE_TRANSIENT);

    if (beast2_media_step_done(db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return -1;
    }

    sqlite3_finalize(statement);
    return 0;
}

int beast2_media_library_init(
    beast2_media_library_context *context,
    const char *workspace_root,
    char *error_message,
    size_t error_message_size
) {
    char db_directory[BEAST2_MAX_PATH_LENGTH];
    static const char *schema_sql =
        "PRAGMA journal_mode=WAL;"
        "CREATE TABLE IF NOT EXISTS generators ("
        "  generator_id INTEGER PRIMARY KEY,"
        "  name TEXT NOT NULL UNIQUE,"
        "  description TEXT,"
        "  creation_date TEXT NOT NULL,"
        "  source_path TEXT,"
        "  engine TEXT,"
        "  checkpoint TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS media ("
        "  media_id INTEGER PRIMARY KEY,"
        "  path TEXT NOT NULL UNIQUE,"
        "  thumbnail_path TEXT,"
        "  type TEXT NOT NULL,"
        "  resolution TEXT,"
        "  duration REAL DEFAULT 0,"
        "  size INTEGER NOT NULL,"
        "  creation_time TEXT NOT NULL,"
        "  generator_id INTEGER,"
        "  checkpoint TEXT,"
        "  seed TEXT,"
        "  prompt_hash TEXT,"
        "  prompt TEXT,"
        "  backend TEXT,"
        "  precision TEXT,"
        "  FOREIGN KEY(generator_id) REFERENCES generators(generator_id)"
        ");"
        "CREATE TABLE IF NOT EXISTS tags ("
        "  tag_id INTEGER PRIMARY KEY,"
        "  tag_name TEXT NOT NULL UNIQUE,"
        "  tag_type TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS media_tags ("
        "  media_id INTEGER NOT NULL,"
        "  tag_id INTEGER NOT NULL,"
        "  confidence REAL NOT NULL DEFAULT 1.0,"
        "  PRIMARY KEY(media_id, tag_id),"
        "  FOREIGN KEY(media_id) REFERENCES media(media_id),"
        "  FOREIGN KEY(tag_id) REFERENCES tags(tag_id)"
        ");"
        "CREATE TABLE IF NOT EXISTS generator_history ("
        "  history_id INTEGER PRIMARY KEY,"
        "  generator_id INTEGER NOT NULL,"
        "  media_id INTEGER NOT NULL,"
        "  artifact_path TEXT NOT NULL,"
        "  created_time TEXT NOT NULL,"
        "  FOREIGN KEY(generator_id) REFERENCES generators(generator_id),"
        "  FOREIGN KEY(media_id) REFERENCES media(media_id)"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_media_generator_id ON media(generator_id);"
        "CREATE INDEX IF NOT EXISTS idx_media_type ON media(type);"
        "CREATE INDEX IF NOT EXISTS idx_media_creation_time ON media(creation_time);"
        "CREATE INDEX IF NOT EXISTS idx_tags_name ON tags(tag_name);"
        "CREATE INDEX IF NOT EXISTS idx_media_tags_tag_id ON media_tags(tag_id);";

    memset(context, 0, sizeof(*context));
    snprintf(context->workspace_root, sizeof(context->workspace_root), "%s", workspace_root);

    if (
        beast2_fs_join_path(
            context->db_path,
            sizeof(context->db_path),
            workspace_root,
            "db/beast2.sqlite"
        ) != 0
    ) {
        beast2_media_set_error(error_message, error_message_size, "failed to build media database path");
        return -1;
    }

    if (beast2_fs_parent_directory(context->db_path, db_directory, sizeof(db_directory)) != 0) {
        beast2_media_set_error(error_message, error_message_size, "failed to derive media database directory");
        return -1;
    }

    if (beast2_fs_mkdirs(db_directory, error_message, error_message_size) != 0) {
        return -1;
    }

    if (sqlite3_open(context->db_path, &context->db) != SQLITE_OK) {
        snprintf(error_message, error_message_size, "failed to open sqlite database: %s", sqlite3_errmsg(context->db));
        sqlite3_close(context->db);
        context->db = NULL;
        return -1;
    }

    if (beast2_media_exec_sql(context->db, schema_sql, error_message, error_message_size) != 0) {
        sqlite3_close(context->db);
        context->db = NULL;
        return -1;
    }

    return 0;
}

void beast2_media_library_shutdown(beast2_media_library_context *context) {
    if (context->db != NULL) {
        sqlite3_close(context->db);
        context->db = NULL;
    }
}

int beast2_media_library_record_output(
    beast2_media_library_context *context,
    const beast2_media_record *record,
    beast2_media_record_result *result,
    char *error_message,
    size_t error_message_size
) {
    struct stat path_stat;
    sqlite3_stmt *statement = NULL;
    char timestamp[64];
    long long generator_id = 0;

    memset(result, 0, sizeof(*result));

    if (context == NULL || context->db == NULL || record == NULL) {
        beast2_media_set_error(error_message, error_message_size, "media library context is invalid");
        return -1;
    }

    if (stat(record->output_path, &path_stat) != 0) {
        snprintf(error_message, error_message_size, "failed to stat output path: %s", record->output_path);
        return -1;
    }

    if (beast2_media_generate_thumbnail(context, record, result, error_message, error_message_size) != 0) {
        return -1;
    }

    generator_id = beast2_media_upsert_generator(context->db, record, error_message, error_message_size);
    if (generator_id == 0) {
        return -1;
    }

    beast2_media_current_timestamp(timestamp, sizeof(timestamp));

    if (
        beast2_media_prepare_statement(
            context->db,
            &statement,
            "INSERT INTO media (path, thumbnail_path, type, resolution, duration, size, creation_time, generator_id, checkpoint, seed, prompt_hash, prompt, backend, precision) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
            "ON CONFLICT(path) DO UPDATE SET "
            "thumbnail_path=excluded.thumbnail_path, "
            "type=excluded.type, "
            "resolution=excluded.resolution, "
            "duration=excluded.duration, "
            "size=excluded.size, "
            "creation_time=excluded.creation_time, "
            "generator_id=excluded.generator_id, "
            "checkpoint=excluded.checkpoint, "
            "seed=excluded.seed, "
            "prompt_hash=excluded.prompt_hash, "
            "prompt=excluded.prompt, "
            "backend=excluded.backend, "
            "precision=excluded.precision;",
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    sqlite3_bind_text(statement, 1, record->output_relative_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, result->thumbnail_relative_path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, record->output_kind, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, record->resolution, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(statement, 5, 0.0);
    sqlite3_bind_int64(statement, 6, (sqlite3_int64) path_stat.st_size);
    sqlite3_bind_text(statement, 7, timestamp, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(statement, 8, generator_id);
    sqlite3_bind_text(statement, 9, record->checkpoint, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 10, record->seed, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 11, record->prompt_hash, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 12, record->prompt, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 13, record->backend, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 14, record->precision, -1, SQLITE_TRANSIENT);

    if (beast2_media_step_done(context->db, statement, error_message, error_message_size) != 0) {
        sqlite3_finalize(statement);
        return -1;
    }

    sqlite3_finalize(statement);
    result->generator_id = generator_id;
    result->media_id = beast2_media_lookup_media_id(
        context->db,
        record->output_relative_path,
        error_message,
        error_message_size
    );

    if (result->media_id == 0) {
        return -1;
    }

    if (
        beast2_media_insert_media_tags(
            context->db,
            result->media_id,
            record->tags_csv,
            error_message,
            error_message_size
        ) != 0 ||
        beast2_media_insert_generator_history(
            context->db,
            generator_id,
            result->media_id,
            record->generator_artifact_relative_path,
            error_message,
            error_message_size
        ) != 0
    ) {
        return -1;
    }

    return 0;
}
