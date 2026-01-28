#ifndef DB_ANIMALS_H
#define DB_ANIMALS_H

int db_animal_create(void);
int db_animal_get(void);
int db_animal_update(void);
int db_animal_delete(void);
int db_animal_search(const char *query);

#endif /* DB_ANIMALS_H */