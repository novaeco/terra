#ifndef API_BREEDING_H
#define API_BREEDING_H

int api_breeding_get_cycles(void);
int api_breeding_create_cycle(void);
int api_breeding_get_cycle(const char *id);
int api_breeding_record_mating(const char *id);
int api_breeding_record_clutch(const char *id);
int api_breeding_record_hatching(const char *id);

#endif /* API_BREEDING_H */