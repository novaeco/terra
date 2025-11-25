# components/bus/AGENTS.md — Bus partagés (I2C/CAN/RS485)

## Règles
- I2C: centraliser via i2c_bus_shared, mutex obligatoire si multi-clients.
- CAN/RS485: ne pas changer paramètres par défaut sans demande.
- API: esp_err_t + logs modérés.

## Validation
- build OK
- pas de deadlock
