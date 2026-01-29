# AGENTS.md — FIRMWARE_CORE (ESP-IDF / FreeRTOS / perf)

## Étape 0
Lire `docs/skill/god-mode-dev-herp/SKILL.md` (bloquant).

## Mission
- Orchestration app_main (sans monolithe)
- Tasks/queues/event groups
- Perf/mémoire (DRAM/PSRAM), buffers
- Glue modules (wifi/http/db/mqtt/ble/security/ota)

## Validation
`idf.py build` obligatoire + logs ciblés si besoin.
