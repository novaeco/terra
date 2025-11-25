# components/display/AGENTS.md — Contraintes LCD RGB 1024×600

## Règles
- Ne pas changer timings/pins sans justification et logs.
- Stabilité: pas de flicker/tearing, init robuste.
- Toute modif doit préserver le chemin DMA/PSRAM si utilisé.

## Validation
- image stable à l’écran
- pas de reset loop
