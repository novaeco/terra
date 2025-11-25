# components/sdspi_ch422g/AGENTS.md — Contraintes microSD SPI via CH422G

## Règles
- Source de vérité: implémentation actuelle (pins/CS/EXIO).
- Absence carte SD: aucun crash, retour d’erreur propre.
- Ne pas bloquer l’UI pendant mount/IO (tâche dédiée).

## Validation
- build OK
- avec carte: mount + lecture OK
- sans carte: fallback OK
