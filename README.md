# mon_projet_jc1060p470c

Firmware ESP-IDF pour module Guition JC1060P470C-I-W-Y (ESP32-P4) avec écran 7" MIPI-DSI JD9165BA et dalle tactile capacitive I2C.

## Structure
- `main/`: code applicatif (board, LCD JD9165, tactile GT911, portage LVGL, app main)
- `docs/`: notes matérielles et checklist bring-up
- `FLASHING.md`: procédures de flash (idf.py et Flash Download Tool)

## Construction rapide
```bash
idf.py set-target esp32p4
idf.py fullclean
idf.py build
```
