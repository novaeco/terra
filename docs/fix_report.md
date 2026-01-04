## Résumé correctifs JC1060P470C

### Causes racines
- LVGL 9.4 : `lv_anim_set_exec_cb` refusait le cast direct vers `lv_obj_set_style_opa` (warning `-Wcast-function-type` promu en erreur).
- Timer LVGL : accès à `timer->user_data` interdit avec le type opaque.
- Police UI : `lv_font_montserrat_20` non activée dans la configuration LVGL, symbole manquant à l’édition de liens.
- Portage LVGL : collision de nom `lvgl_port_init` avec l’API officielle `esp_lvgl_port`.
- Flash ESP32-P4 : configuration par défaut limitait l’image à la révision min v3.0, incompatible avec le silicium v1.3 (flash refusé).

### Correctifs appliqués
- Ajout d’un wrapper d’animation conforme (`anim_set_obj_opa`) et d’un callback de fin typé pour supprimer le toast.
- Utilisation de `lv_timer_get_user_data` pour récupérer le contexte UI.
- Activation de `CONFIG_LV_FONT_MONTSERRAT_20` dans `sdkconfig.defaults`.
- Renommage du portage local en `app_lvgl_port` (header dédié) pour éviter tout conflit avec `esp_lvgl_port`.
- Configuration de compatibilité ESP32-P4 : `CONFIG_ESP32P4_SELECTS_REV_LESS_V3=y` et `CONFIG_ESP32P4_REV_MIN_100=y` persistés.

### Validation attendue
Exécuter depuis PowerShell (répertoire du projet) :
```
idf.py fullclean
idf.py build
idf.py -p COM9 flash
```
Critères : aucune erreur/avertissement bloquant, image flashée sans message de révision (bootloader marqué compatible v1.0+), interface LVGL compilée avec la police 20 disponible.
