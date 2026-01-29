# AGENTS.md (Racine) — Règles globales (Codex + Agents)

⚠️ Le fichier d'autorité opérationnelle est **`agnts.md`** (racine).
Ce fichier-ci sert de *référence globale* (non destructrice) et de pointeur vers l'ADMIN.

## Étape 0 (bloquante)
Avant toute action : lire `docs/skill/god-mode-dev-herp/SKILL.md`.
Si absent/illisible : STOP, rapporter l'erreur sans inventer.

## Sources de vérité (ordre de priorité)
1) `docs/skill/god-mode-dev-herp/SKILL.md`
2) `docs/ARCHITECTURE.md`
3) `docs/PROJET_COMPLET.md`
4) `SPEC_GESTIONNAIRE_ELEVAGE_REPTILES.md`

## Validation minimale
- `idf.py build` obligatoire pour tout patch.

## Gouvernance
- Tout agent spécialisé rend : plan + diff + commandes + logs.
- `agnts.md` (ADMIN) tranche (PASS/FAIL) et bloque toute suppression non demandée.
