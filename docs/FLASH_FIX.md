# Correction taille flash ESP32-P4

## Symptôme observé
Lors du boot, le bootloader affiche `SPI Flash Size: 2MB` puis le moniteur avertit `Detected size(16384k) larger than image header (2048k)`. Cela signifie que l’image est compilée pour 2 MB alors que la puce détectée fait 16 MB, ce qui peut provoquer des partitions tronquées ou un plantage au démarrage.

## Correctif appliqué
- Configuration forcée à 16 MB via Kconfig : `CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y` dans `sdkconfig.defaults` (l’option `…_DETECT` seule ne suffisait pas, l’image restait en 2 MB).
- Support Boya activé (voir `docs/FLASH_BOYA.md`) pour aligner le driver sur la puce détectée par le bootloader.

## Procédure de build/flash
```bash
idf.py fullclean build
idf.py -p COM9 flash
idf.py -p COM9 monitor
```
Log attendu : la ligne `Flash Size` doit indiquer 16 MB et l’avertissement “Detected size larger than header” doit disparaître.

## Rollback
Rééditer `sdkconfig.defaults` en réactivant `CONFIG_ESPTOOLPY_FLASHSIZE_DETECT=y` (ou une autre taille), puis `idf.py fullclean build`.
