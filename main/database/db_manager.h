#ifndef DB_MANAGER_H
#define DB_MANAGER_H

/*
 * SQLite database manager stub.  The real manager would initialise
 * the SQLite engine, run migrations, manage a connection pool and
 * provide thread‑safe access to the database.  Functions return 0
 * for success and non‑zero for failure.
 */

int db_init(void);
int db_execute(const char *sql);
int db_backup(void);

#endif /* DB_MANAGER_H */