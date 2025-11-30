# ui_assets/AGENTS.md — Discipline assets (icons/fonts/themes)

## Règles
- Éviter renommages/déplacements sans MAJ des références.
- Toute nouvelle image: taille/poids compatibles RAM/PSRAM et pipeline LVGL du repo.
- Un seul pipeline d’assets (ne pas en introduire un 2e sans demande).
- Budget mémoire/PSRAM par type d’asset (icône, font); formats recommandés (PNG/RAW/mono) documentés.
- Pipeline: préciser l’outil de conversion LVGL et options utilisées; conserver l’historique de versioning/noms.

## SD vs Compilé
- Si SD: absence SD => fallback, jamais de crash.
- Si compilé: vérifier intégration via CMake.
