# components/storage/AGENTS.md — Stockage (sdcard, mounts, paths)

## Règles
- Filtrage d’erreurs: jamais de crash si média absent.
- Paths: cohérence (pas de chemins magiques).
- Convention de chemin racine (ex: /sdcard vs /spiflash) explicite; gérer permissions/droits si applicables.
- Budget mémoire pour buffers IO (alignement/taille bloc compatibles carte utilisée).

## Validation
- mount OK si média présent
- fallback OK sinon
