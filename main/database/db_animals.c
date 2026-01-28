#include <stdio.h>
#include "db_animals.h"

/*
 * Animal database accessors.
 *
 * These functions call into the generic db_manager to execute SQL
 * statements against the animals table.  For brevity the
 * statements are simplified and hard‑coded.  A real
 * implementation would accept parameters and use prepared
 * statements to prevent SQL injection.
 */

#include "database/db_manager.h"
#include "utils/logger.h"

int db_animal_create(void)
{
    const char *sql =
        "INSERT INTO animals (id, species_name, common_name, date_acquisition, status, created_at, updated_at) "
        "VALUES ('animal-1', 'Python regius', 'Ball python', strftime('%s','now'), 'ACTIVE', strftime('%s','now'), strftime('%s','now'));";
    if (db_execute(sql) == 0) {
        log_info("db/animals", "Inserted new animal record");
        return 0;
    }
    return -1;
}

int db_animal_get(void)
{
    const char *sql = "SELECT id, species_name, common_name FROM animals LIMIT 1;";
    return db_execute(sql);
}

int db_animal_update(void)
{
    const char *sql = "UPDATE animals SET status='SOLD', updated_at=strftime('%s','now') WHERE id='animal-1';";
    return db_execute(sql);
}

int db_animal_delete(void)
{
    const char *sql = "DELETE FROM animals WHERE id='animal-1';";
    return db_execute(sql);
}

int db_animal_search(const char *query)
{
    if (!query) {
        return -1;
    }
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT id, species_name FROM animals WHERE species_name LIKE '%%%s%%';", query);
    return db_execute(sql);
}