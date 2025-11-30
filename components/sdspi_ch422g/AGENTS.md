# components/sdspi_ch422g/AGENTS.md — Contraintes microSD SPI via CH422G

## Règles
- Source de vérité: implémentation actuelle (pins/CS/EXIO).
- Absence carte SD: aucun crash, retour d’erreur propre.
- Ne pas bloquer l’UI pendant mount/IO (tâche dédiée).
- Budget temporel/stack: définir stack/taille et priorité FreeRTOS de la tâche SD; s’assurer qu’elle ne bloque pas l’affichage.

## Validation
- build OK
- avec carte: mount + lecture OK
- sans carte: fallback OK
- Procédure de test: exécuter avec carte puis sans carte; vérifier logs attendus et absence de blocage UI.
