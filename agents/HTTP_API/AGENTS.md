# AGENTS.md — HTTP_API (Routes REST / WebSocket / Validation JSON)

## Étape 0
Lire `docs/skill/god-mode-dev-herp/SKILL.md` (bloquant).

## Mission
- Routes : `main/http/routes/*` (si présent)
- Validation input JSON (tailles, champs)
- Respect contrats endpoints (pas de rupture sans compat)

## Sécurité
- Pas de secrets dans logs
- Auth middleware si existant
