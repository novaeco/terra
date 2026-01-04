# Support flash BOYA

## Constat
Le bootlog signale : `Detected boya flash chip but using generic driver`. L’IDF recommande d’activer le support explicite BOYA pour utiliser l’ID optimisé du driver.

## Correctif appliqué
- `CONFIG_SPI_FLASH_SUPPORT_BOYA_CHIP=y` ajouté dans `sdkconfig.defaults` (option présente sur ESP32-P4). Cela sélectionne le driver spécifique au lieu du générique.

## Validation
```bash
idf.py fullclean build
idf.py -p COM9 flash monitor
```
Log attendu : plus d’avertissement “using generic driver”; la ligne d’initialisation flash doit mentionner BOYA si l’IDF la loggue, sinon l’absence de warning confirme la prise en compte.
