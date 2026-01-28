#include <stdio.h>
#include <string.h>
#include "db_manager.h"

/*
 * SQLite database manager implementation.
 *
 * This module initialises the embedded SQLite engine, opens a
 * database file on the SPIFFS/LittleFS filesystem and runs basic
 * migration scripts to create tables if they do not exist.  The
 * db_execute() helper executes arbitrary SQL statements.  A simple
 * backup function copies the database file to a .bak file in the
 * same directory.  See the architecture document for the schema【808169448218282†L587-L669】.
 */

#include "sqlite3.h"
#include "storage/file_manager.h"
#include "utils/logger.h"

static sqlite3 *s_db = NULL;

int db_init(void)
{
    if (s_db) {
        return 0;
    }
    // Open or create the database file in SPIFFS/LittleFS
    const char *db_path = "/spiffs/reptiles.db";
    int rc = sqlite3_open(db_path, &s_db);
    if (rc != SQLITE_OK) {
        log_error("db", "Failed to open database: %s", sqlite3_errmsg(s_db));
        return -1;
    }
    // Enable foreign keys
    sqlite3_exec(s_db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    // Create tables if they do not exist (simplified schema)
    const char *sql =
        "CREATE TABLE IF NOT EXISTS animals ("
        "id TEXT PRIMARY KEY,"
        "species_name TEXT NOT NULL,"
        "common_name TEXT,"
        "sex TEXT,"
        "date_birth INTEGER,"
        "date_acquisition INTEGER NOT NULL,"
        "status TEXT,"
        "provenance_type TEXT,"
        "provenance_vendor TEXT,"
        "metadata_json TEXT,"
        "created_at INTEGER NOT NULL,"
        "updated_at INTEGER NOT NULL);";
    rc = sqlite3_exec(s_db, sql, NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        log_error("db", "Failed to create tables: %s", sqlite3_errmsg(s_db));
        return -1;
    }
    log_info("db", "Database initialised at %s", db_path);
    return 0;
}

int db_execute(const char *sql)
{
    if (!s_db || !sql) {
        return -1;
    }
    char *errmsg = NULL;
    int rc = sqlite3_exec(s_db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        log_error("db", "SQL error: %s", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    return 0;
}

int db_backup(void)
{
    if (!s_db) {
        return -1;
    }
    // Flush SQLite buffers
    sqlite3_exec(s_db, "VACUUM;", NULL, NULL, NULL);
    // Copy the DB file to a backup file
    const char *src = "/spiffs/reptiles.db";
    const char *dst = "/spiffs/reptiles.db.bak";
    FILE *fsrc = fopen(src, "rb");
    if (!fsrc) {
        log_error("db", "Failed to open source DB for backup");
        return -1;
    }
    FILE *fdst = fopen(dst, "wb");
    if (!fdst) {
        fclose(fsrc);
        log_error("db", "Failed to open destination DB for backup");
        return -1;
    }
    char buf[256];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fsrc)) > 0) {
        fwrite(buf, 1, n, fdst);
    }
    fclose(fsrc);
    fclose(fdst);
    log_info("db", "Database backup created at %s", dst);
    return 0;
}