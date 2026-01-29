# CHECKLIST — Gates (Shared)

## Gate 0 — SKILL.md lu
- [ ] `docs/skill/god-mode-dev-herp/SKILL.md` ouvert et appliqué
- [ ] Si absent : STOP + rapport

## Gate 1 — Architecture (couches/modules)
- [ ] Changements confinés au bon module
- [ ] Pas de couplage illégitime
- [ ] main.c reste orchestration
- [ ] Pas de “god file”

## Gate 2 — Contrats API/DB
- [ ] Endpoints non cassés sans compat/migration
- [ ] DB modifiée => migration SQL fournie
- [ ] JSON validé (tailles + champs)

## Gate 3 — Sécurité
- [ ] Pas de secrets hardcodés
- [ ] Input validation
- [ ] SQL injection impossible (prepared/bind)
- [ ] Logs non sensibles

## Gate 4 — Build/Test
- [ ] `idf.py build` OK
- [ ] Tests pertinents (si présents) OK
