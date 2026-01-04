# FLASHING

## Via ESP-IDF
```bash
idf.py set-target esp32p4
idf.py flash monitor
```

> Note : la carte cible est un ESP32-P4 révision v1.3. Le bootloader/app sont configurés avec `REV_MIN=1` pour éviter le refus de flash (« chip revision mismatch ») observé lorsque la révision minimale est fixée à v3. Pensez à forcer une reconstruction propre avant un flash sur cette révision :
> ```bash
> idf.py fullclean
> idf.py build
> idf.py -p COM9 flash
> ```

## Via Flash Download Tool
1. Construire une fois : `idf.py build` puis lire `build/flasher_args.json` pour les offsets.
2. Ouvrir Flash Download Tool (ESP32-P4), sélectionner les binaires/offsets de `flasher_args.json`.
3. Mode download : BOOT_MODE bas + CHIP_PU haut (ESP32-P4). Si jamais ESP32-C6 requis : IO9 bas + CHIP_PU bas/haut selon séquence.
