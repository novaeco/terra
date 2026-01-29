# DIFF_GATE — Scan anti-suppression / anti-régression (ADMIN)

## Objectif
Détecter automatiquement les changements destructeurs / risqués avant acceptation.

## Signaux BLOCKER (par défaut)
1) Suppressions fichiers
- `diff --git a/... b/...` + `deleted file mode`
- `git rm`
2) Suppressions d’API / endpoints
- Retrait de handlers routes, suppression de fonctions publiques exportées
3) Modifs DB destructrices
- `DROP TABLE`, `DROP COLUMN`, suppression migrations, downgrade version
4) Downgrade sécurité
- Désactivation TLS/JWT, suppression vérifs input, retrait chiffrement NVS
5) Refactor massif non demandé
- Renommages globaux, reformat, déplacement massif de fichiers

## Exceptions (uniquement si demandé explicitement)
- Suppression demandée : exiger plan migration + compat + rollback.

## Utilisation script
- Exécuter `python agents/_shared/diff_gate.py <DIFF_FILE>`
- Le script retourne exit code 0 (OK) ou 2 (BLOCKER détecté).

## Note
Le script est volontairement conservateur : mieux vaut faux positif que destruction silencieuse.
