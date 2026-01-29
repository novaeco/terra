# agents/ — Équipe d'agents (orchestrée par ADMIN racine: AGENTS.md)

## Règle 0 (bloquante)
Avant toute action : lire `docs/skill/god-mode-dev-herp/SKILL.md`.
Si absent : STOP (rapport d'erreur), ne rien inventer.

## Contrat commun
Tous les agents appliquent :
- `agents/_shared/CONTRACT.md`
- `agents/_shared/CHECKLIST.md`
- `agents/_shared/OUTPUT_FORMAT.md`

## Gate diff (ADMIN)
- `agents/_shared/DIFF_GATE.md`
- `agents/_shared/diff_gate.py` (+ helpers .ps1/.sh)

## Fonctionnement
- Agents spécialisés => plan + diff + validation.
- ADMIN (AGENTS.md) => gates + consolidation + build final.
