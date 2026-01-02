# Notes matérielles JC1060P470C-I-W-Y

- Résolution : 1024x600 (Specs-EN p.4), JD9165 driver.
- MIPI-DSI : 2 lanes (DTSI init 0x0B=0x11) ; horloge ~51.2 MHz ; timings : HS=24, HBP=136, HFP=160, VS=2, VBP=21, VFP=12 (dtsi lignes 1-8).
- Touch capacitif : bus I2C (voir schéma « Capacitive touch »), broches ESP32-P4 SDA=GPIO9, SCL=GPIO8, RST=GPIO10, INT=GPIO11 (schéma image JC1060_schematic-1, bloc Capacitive touch : GT911, libellé IO9/IO8/IO10/IO11).
- LCD contrôle : STBYB tiré haut (résistance pull-up, bloc DSI_LCD), RESET via GPIO14 (schéma DSI_LCD : LCD_RST sur IO14), BL_EN via GPIO2 (schéma Blacklighting : BLK_EN IO2), BL PWM via GPIO1 (BL_PWM IO1). Alim LCD par DC/DC (bloc LCD_DC-DC), rétroéclairage 12V/LED driver TPS61178 config (VLED ~12V, courant fixé par Rset 20k).
- CSI caméra : connecteur CSI (bloc CSI_Camera), signaux CCLK/MCLK, PVDD/DVDD, SCL/SDA, données D0..D7/HSYNC/VSYNC/PCLK (brochés vers ESP32-P4 GPIO40..47/48.. etc.). Non implémenté dans firmware.
- UART/BOOT : ESP32-P4 UART0 sur GPIO43 (TX) / GPIO44 (RX) (schéma bloc JC-ESP32P4-M3). BOOT_MODE (GPIO9 sur en-tête? Attention : GPIO9 utilisé par I2C touch) ; CHIP_PU via bouton reset.
- ESP32-C6 compagnon : UART0 C6_U0TXD/U0RXD, strap IO9 pour download (schéma en-tête JP1). Par défaut non utilisé.
- Inconnues : adresse I2C GT911 (0x5D ou 0x14 possibles) – sera scannée au boot ; présence PSRAM 32M mentionnée (Specs p.3) mais à confirmer via log esp_psram_get_size().
