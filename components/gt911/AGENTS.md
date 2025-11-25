# components/gt911/AGENTS.md — Contraintes tactile GT911

## Règles
- I2C partagé: respecter mutex/bus shared.
- Gestion IRQ / polling selon code actuel; ne pas dupliquer.
- Robustesse: gérer erreurs I2C sans panic.

## Validation
- init OK
- événements tactiles cohérents
