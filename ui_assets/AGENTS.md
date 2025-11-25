# ui_assets/AGENTS.md — Discipline assets (icons/fonts/themes)

## Règles
- Éviter renommages/déplacements sans MAJ des références.
- Toute nouvelle image: taille/poids compatibles RAM/PSRAM et pipeline LVGL du repo.
- Un seul pipeline d’assets (ne pas en introduire un 2e sans demande).

## SD vs Compilé
- Si SD: absence SD => fallback, jamais de crash.
- Si compilé: vérifier intégration via CMake.
