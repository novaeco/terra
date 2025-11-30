# components/ch422g/AGENTS.md — Contraintes expandeur CH422G

## Règles
- I2C partagé: pas de collisions (mutex).
- Ne pas casser le mapping EXIO utilisé par LCD/SD/USB/CAN etc.
- Toute modif doit être rétrocompatible avec les usages existants.
- Maintenir un tableau récapitulatif des GPIO/EXIO utilisés (LCD, SD, USB, CAN) pour limiter les régressions.

## Validation
- init OK
- fonctions EXIO utilisées par l’app OK
- Procédure: test lecture/écriture EXIO après init avec logs attendus; vérifier qu’aucun mapping critique ne change.
