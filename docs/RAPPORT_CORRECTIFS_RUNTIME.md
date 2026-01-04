# Correctifs runtime (boot/WDT/flash/debug)

## Flash size et driver BOYA
- Problème : image compilée pour 2 MB alors que la puce détectée fait 16 MB (`Detected size(16384k) larger than image header (2048k)`) et warning “Detected boya flash chip but using generic driver”.
- Action : `CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y` et `CONFIG_SPI_FLASH_SUPPORT_BOYA_CHIP=y` dans `sdkconfig.defaults` (bootloader + app).
- Résultat attendu : header image à 16 MB, disparition du warning “size larger than header”, driver BOYA actif ou plus de warning générique.

## Backtrace exploitable
- Problème : crash WDT sans frame pointer rend la trace partielle, l’outil recommande `CONFIG_ESP_SYSTEM_USE_FRAME_POINTER`.
- Action : activation `CONFIG_ESP_SYSTEM_USE_FRAME_POINTER=y` (documenté dans `docs/DEBUG_BACKTRACE.md`).
- Résultat attendu : traces complètes décodables via `idf.py monitor`/`addr2line`.

## WDT (IDLE0 bloqué)
- Symptôme : `task_wdt` sur IDLE0/CPU0, suspicion de blocage pendant l’initialisation DSI/JD9165.
- Actions ciblées :
  - Ajout de traces temporelles dans `display_jd9165_init` (DSI bus/IO/panel/reset/seq) et dans `app_main` (jalons boot) pour localiser le blocage exact.
  - Yield systématique (`vTaskDelay(1)`) entre chaque commande JD9165 pour éviter d’affamer IDLE pendant la séquence d’init.
- Résultat attendu : plus de WDT pendant l’init ; si un blocage persiste, les timestamps indiquent la phase exacte à inspecter.

## Commandes de validation
```bash
idf.py fullclean build
idf.py -p COM9 flash
idf.py -p COM9 monitor
```
Critère : 60 s de logs sans `task_wdt`, affichage de la taille flash 16 MB et absence de warning générique BOYA.

## Rollback rapide
Revenir sur `sdkconfig.defaults` (retirer les 3 options ajoutées) puis `idf.py fullclean build`.
