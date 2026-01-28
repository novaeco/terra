#ifndef API_ANIMALS_H
#define API_ANIMALS_H

/*
 * API handlers for the `/api/v1/animals` endpoints.
 * Each function corresponds to a REST verb on either the collection
 * or an individual animal.  Replace these stubs with your own
 * handlers that parse requests, interact with the database and
 * serialise responses to JSON.
 */

int api_animals_get_all(void);
int api_animals_create(void);
int api_animals_get(const char *id);
int api_animals_update(const char *id);
int api_animals_delete(const char *id);

#endif /* API_ANIMALS_H */