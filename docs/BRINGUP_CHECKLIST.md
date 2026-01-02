# Checklist de bring-up

1. Alimentation & câblage
   - USB ou batterie, vérifier 5V stable.
   - BLK_EN (IO2) et BL_PWM (IO1) correctement routés vers driver LED.
2. Flash
   - `idf.py set-target esp32p4 && idf.py flash monitor`.
   - UART0 sur GPIO43/44, CHIP_PU tiré haut, BOOT_MODE bas pour mode normal.
3. Logs init
   - Rechercher : `lcd_dsi lanes=2`, `timings hs=24 hbp=136 hfp=160 vs=2 vbp=21 vfp=12 dotclk=51.2MHz`.
   - Touch : `i2c scan found ...`, puis `gt911 addr=0x..`.
   - LVGL : `lvgl tick ok`, `flush ok`.
4. Affichage
   - Écran doit allumer rétroéclairage, afficher démo LVGL (cercle/bouton animé).
5. Tactile
   - Toucher l’écran : logs d’événements, curseur/bouton réagit.
6. Si échec
   - Écran noir : vérifier BLK_EN/BL_PWM, RESET (IO14), STBYB tiré haut.
   - Mauvais timings/lanes : vérifier config DSI 2 lanes.
   - Tactile muet : re-scan I2C, vérifier RST/INT, adresse 0x5D/0x14.
   - PSRAM absente : réduire buffers LVGL.
7. Logs capture
   - Utiliser `idf.py monitor`, sauver trace incluant init DSI, I2C scan, premiers touch.
