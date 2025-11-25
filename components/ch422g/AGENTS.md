# components/ch422g/AGENTS.md — Contraintes expandeur CH422G

## Règles
- I2C partagé: pas de collisions (mutex).
- Ne pas casser le mapping EXIO utilisé par LCD/SD/USB/CAN etc.
- Toute modif doit être rétrocompatible avec les usages existants.

## Validation
- init OK
- fonctions EXIO utilisées par l’app OK
