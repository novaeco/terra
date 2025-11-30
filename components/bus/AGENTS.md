# components/bus/AGENTS.md — Bus partagés (I2C/CAN/RS485)

## Règles
- I2C: centraliser via i2c_bus_shared, mutex obligatoire si multi-clients.
- CAN/RS485: ne pas changer paramètres par défaut sans demande.
- API: esp_err_t + logs modérés.
- Timeouts/backoff: définir des timeouts réalistes et backoff en cas d’erreur pour éviter blocages.
- Initialisation: conserver une init centralisée (fichier/fonction de référence) pour éviter la duplication de config bus.

## Validation
- build OK
- pas de deadlock
