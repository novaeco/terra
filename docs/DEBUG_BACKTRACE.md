# Backtrace exploitable

## Objectif
Les plantages actuels demandent un diagnostic “CONFIG_ESP_SYSTEM_USE_FRAME_POINTER”. Sans frame pointer, les traces peuvent être tronquées ou inexploitables avec l’IDF 5.x.

## Correctif appliqué
- `CONFIG_ESP_SYSTEM_USE_FRAME_POINTER=y` ajouté dans `sdkconfig.defaults` pour conserver les pointeurs de pile dans toutes les builds (bootloader + app).

## Comment utiliser
```bash
idf.py -p COM9 monitor
```
Lors d’un crash, la trace doit inclure des adresses résolues (fonctions/sources) et être décodable par `addr2line` via `idf.py monitor` (ou `xtensa-esp32p4-elf-addr2line`). Aucune option de performance n’a été dégradée en dehors de cette conservation de frame pointer (surcoût minime, indispensable pour l’analyse WDT).
