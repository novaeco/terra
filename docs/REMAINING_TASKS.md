# Tâches restantes

Toutes les actions prévues dans la liste initiale ont été implémentées :
- Modélisation d’écrans SquareLine/LVGL (création programmatique complète des écrans listés dans `docs/UI_SPEC.md`).
- Génération intégrée dans le squelette ESP-IDF (`ui_init.c`, callbacks `ui_event_*`, clavier AZERTY complet).
- Drivers matériels complétés : backlight LEDC, Wi-Fi STA, montage microSD, CAN/RS485, diagnostics mémoire/CPU, test tactile stub.
- Clavier AZERTY étendu et gestion d’apparition/masquage sur textareas SSID/PWD/PIN.
- Logique métier/sécurité : StatusBar mise à jour, écran PIN, placeholders OTA et exports.
- Performance et packaging : buffers LVGL double en PSRAM, sdkconfig orienté RGB/LVGL/Wi-Fi, table de partitions OTA.

Aucune tâche additionnelle n’est ouverte.
