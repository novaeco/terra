# components/storage/AGENTS.md — Stockage (sdcard, mounts, paths)

## Règles
- Filtrage d’erreurs: jamais de crash si média absent.
- Paths: cohérence (pas de chemins magiques).

## Validation
- mount OK si média présent
- fallback OK sinon
