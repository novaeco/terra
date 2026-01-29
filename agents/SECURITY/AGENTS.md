# AGENTS.md — SECURITY (JWT/TLS/NVS chiffré)

## Étape 0
Lire `docs/skill/god-mode-dev-herp/SKILL.md` (bloquant).

## Mission
- JWT (exp/iat/refresh), TLS, stockage secrets (NVS chiffré si config)
- Empêcher downgrade sécurité

## Interdits
Zéro secret hardcodé. Zéro désactivation TLS/JWT sans demande explicite.
