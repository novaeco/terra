# agents/ — Équipe d'agents (orchestrée par ADMIN racine: agnts.md)

## Règle 0
Avant toute action : lire `docs/skill/god-mode-dev-herp/SKILL.md`.

## Contrat commun
Tous les agents appliquent :
- `agents/_shared/CONTRACT.md`
- `agents/_shared/CHECKLIST.md`
- `agents/_shared/OUTPUT_FORMAT.md`
- Gate diff : `agents/_shared/DIFF_GATE.md` (ADMIN)

## Fonctionnement
- Les agents spécialisés rendent : plan + diff + validation.
- L’ADMIN (agnts.md) bloque suppression/hallucination et valide build/tests.

## Démarrage recommandé (workflow)
1) REPO_SCOUT : cartographie
2) ARCHITECT : boundaries
3) Agent owner du sous-système : patch minimal
4) SECURITY/COMPLIANCE si impact
5) TEST_RELEASE : build/tests
6) ADMIN : gates + consolidation
