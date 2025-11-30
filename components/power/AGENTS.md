# components/power/AGENTS.md — Gestion power (CS8501)

## Règles
- Ne pas modifier la séquence d’alim/charge sans logs et justification.
- Les lectures doivent être robustes (pas de crash si capteur indispo).
- Matrice des sources d’alimentation (USB/batterie): clarifier priorités et transitions.
- Télémétrie: définir période de mesure, unités et précision attendue.
- En cas d’erreur capteur: valeurs de repli sûres et logs limités (throttle).

## Validation
- boot OK
- mesures/logs cohérents
