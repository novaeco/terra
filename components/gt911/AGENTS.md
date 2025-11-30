# components/gt911/AGENTS.md — Contraintes tactile GT911

## Règles
- I2C partagé: respecter mutex/bus shared.
- Gestion IRQ / polling selon code actuel; ne pas dupliquer.
- Robustesse: gérer erreurs I2C sans panic.
- Déglitch/debounce: filtrer les événements tactiles (seuils, temps) selon besoins UI.
- Fréquence: respecter la fréquence de polling/IRQ actuelle; documenter toute variation.
- Events: uniformiser le format des events LVGL/callback (ex: coordonnées, état) pour intégration UI cohérente.

## Validation
- init OK
- événements tactiles cohérents
