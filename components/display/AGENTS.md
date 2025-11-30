# components/display/AGENTS.md — Contraintes LCD RGB 1024×600

## Règles
- Ne pas changer timings/pins sans justification et logs.
- Stabilité: pas de flicker/tearing, init robuste.
- Toute modif doit préserver le chemin DMA/PSRAM si utilisé.
- Référencer le timing actuel (fichier de config/pinout) et vérifier le mode DMA/PSRAM actif avant toute modif.

## Validation
- image stable à l’écran
- pas de reset loop
- Critères chiffrés: fps ou latence de flush documentés; fournir capture photo/oscillo si timings changent.
