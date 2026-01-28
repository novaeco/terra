#ifndef API_REGULATIONS_H
#define API_REGULATIONS_H

int api_regulations_get_species(const char *name);
int api_regulations_get_animal_status(const char *id);
int api_regulations_get_alerts(void);

#endif /* API_REGULATIONS_H */