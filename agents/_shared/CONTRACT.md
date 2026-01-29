# CONTRACT — Anti-hallucination / Anti-destruction (Shared)

## Anti-hallucination
- Toute affirmation doit être prouvée par :
  - code (chemin + symbole) OU
  - log (commande + extrait) OU
  - doc (SKILL/ARCHITECTURE/PROJET_COMPLET/SPEC)
- Si info manque : Hypothèse A/B/C + test concret.

## Anti-destruction
- Toute suppression (fichier, API, route, table, champ, feature, sécurité) = BLOCKER
  sauf demande explicite + plan migration/compat/rollback.

## Patch minimal
- 1 patch = 1 intention. Pas de refactor global / reformat opportuniste.

## Boundaries
- Respect des responsabilités de modules. Accès cross-module uniquement via interfaces.

## Observabilité
- Logs ciblés, pas de secrets/PII.
