# AGENTS.md — DATABASE (SQLite / Migrations / Transactions)

## Étape 0
Lire `docs/skill/god-mode-dev-herp/SKILL.md` (bloquant).

## Mission
- SQLite : prepared statements + bind
- Transactions pour opérations multi-étapes
- Toute évolution schéma => migration SQL

## Interdits
Pas de DROP destructif sans demande explicite + plan migration/rollback.
