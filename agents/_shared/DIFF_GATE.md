# DIFF_GATE — Scan anti-suppression / anti-régression (ADMIN)

## Objectif
Détecter automatiquement les changements destructeurs / risqués avant acceptation.

## Signaux BLOCKER (par défaut)
1) Suppressions fichiers (`deleted file mode`, `/dev/null`, `git rm`)
2) Modifs DB destructrices (`DROP TABLE`, `DROP COLUMN`, suppression migrations)
3) Suppressions d’API/endpoints (routes retirées, handlers supprimés)
4) Downgrade sécurité (TLS/JWT/auth/encryption désactivés, validations retirées)
5) Refactor massif non demandé (renames, reformat, déplacement massif)

## Utilisation
- `python agents/_shared/diff_gate.py <DIFF_FILE>`
- Exit 0 = OK, Exit 2 = BLOCKER.

Le script est volontairement conservateur.
