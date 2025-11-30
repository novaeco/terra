# main/AGENTS.md — Règles applicatives (UI, tâches, tick, réseau)

## LVGL — règle critique (tick unique)
- Il doit y avoir UNE SEULE source de tick LVGL.
- Interdiction de double tick (ex: LV_TICK_CUSTOM actif + esp_timer qui appelle lv_tick_inc()).
- Source de vérité: le code actuel du repo (ne pas ajouter un 2e mécanisme).
- Définir la tâche LVGL: nom, priorité, stack et budget de rendu; éviter tout blocage (profilage si changement).

## Threading / RTOS
- Ne pas bloquer la tâche LVGL (FS/SD, Wi-Fi, HTTP, DNS, MQTT) -> utiliser tâche dédiée + queue/events.
- Toute init hardware potentiellement lente doit être hors thread LVGL.
- Watchdog: s’assurer que la tâche LVGL/GUI et les tâches réseau/FS respectent les délais; feed/reset adaptés.
- Partitionnement: séparer clairement les tâches réseau/FS vs UI (queues tailles explicites, limites de backlog).

## Assets UI
- Si assets chargés depuis SD: absence SD doit être gérée (fallback).
- Si assets compilés: vérifier CMake et symboles linkés.

## Wi-Fi / Heure (si activés)
- Pas de credentials en dur.
- Préférer NVS ou menuconfig pour config.
- Heure affichée: SNTP + TZ paramétrable.

## DoD
- UI visible et responsive > 60s.
- Aucune panique après init GT911/LCD.
- Checklist UI: latence input raisonnable, fps cible documentée; protocole de test avec et sans SD pour vérifier fallback.
