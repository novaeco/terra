# components/AGENTS.md — Règles composants (drivers/services)

## API / erreurs
- API publique claire (headers), retours esp_err_t.
- Pas d’allocations fréquentes dans callbacks/ISR.
- Logs: niveau par défaut INFO, tag explicite par composant; pas de verbosité excessive en boucle.

## Concurrence
- Bus partagés (I2C/SPI): mutex si multi-clients.
- Pas d’appels bloquants en ISR.
- Prévoir timeouts/backoff raisonnables sur I/O pour éviter deadlocks.

## CMake / dépendances
- SRCS/INCLUDE_DIRS/REQUIRES corrects.
- Ne pas télécharger LVGL (déjà fourni par l’environnement du projet).

## DoD
- Build OK
- Pas de régression sur I2C/SPI/LCD/Touch/SD
- Tests unitaires/mocks: si driver, documenter/stubber I2C/SPI pour tests offline.
