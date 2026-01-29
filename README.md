# ESP32 Reptile Manager Server

This repository provides a **skeleton implementation** of the ESP32‑S3 based
*Reptile Manager Server* described in the accompanying specification and
architecture documents.  It mirrors the directory layout and major
components of the full project without including the full production
code.  The goal of this skeleton is to give developers a clear
starting point from which to build the complete firmware.

The original project is designed to run on an ESP32‑S3‑DevKitC‑1‑N32R16V
board (32 MB of flash and 16 MB of PSRAM).  It exposes a RESTful API,
WebSocket interface, and MQTT topics, stores data in an embedded
SQLite database, reads sensors like DHT22 and DS18B20, supports
OTA updates, and enforces security via JWT, TLS and encrypted NVS.

This skeleton does **not** implement the full functionality but
includes all modules as stub files with the appropriate function
signatures.  Each source file contains TODO comments describing
the responsibilities outlined in the architecture.  You are welcome
to replace these stubs with real implementations based on the
documentation in the `docs` folder.

## PSRAM Troubleshooting (ESP32-S3)

If the monitor shows an error similar to:

```
E (xxx) quad_psram: PSRAM chip is not connected, or wrong PSRAM line mode
E cpu_start: Failed to init external RAM!
```

It means the firmware is configured to use external PSRAM, but the
module either **does not have PSRAM** or the PSRAM **mode/size** does not
match the hardware.

**Hypothèses (et vérifications rapides) :**

* **A — Module sans PSRAM :** la carte ne dispose pas de PSRAM.
  * Vérif : consultez la référence exacte du module (marquage, BOM) et
    comparez avec les variantes PSRAM/non‑PSRAM.
* **B — Mauvais mode de ligne PSRAM :** le firmware est en mode quad alors
  que le module exige un autre mode.
  * Vérif : dans `idf.py menuconfig`, vérifiez la configuration
    *Component config → ESP32S3‑specific → SPI RAM config* et ajustez le
    mode/ligne selon la documentation matérielle.
* **C — Mauvaise taille/config PSRAM :** taille ou timings incorrects.
  * Vérif : idem menuconfig, vérifiez la taille configurée et les timings
    recommandés pour votre module.

**Option A — Board without PSRAM (quick fix):**

Use the no‑PSRAM defaults when configuring the project:

```bash
idf.py fullclean
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.no_psram" reconfigure
idf.py build
```

Then confirm the generated `sdkconfig` contains the line
`# CONFIG_ESP32S3_SPIRAM_SUPPORT is not set` before flashing.

**Option B — Board with PSRAM (correct config):**

Keep PSRAM enabled and make sure the settings match your module
(line mode and size). Use:

```bash
idf.py menuconfig
```

Then check **Component config → ESP32S3-specific → Support for external
SPIRAM** and set the correct mode/size for your board.

## Getting Started

To build the full project you will need the Espressif ESP‑IDF toolchain.
This skeleton does not compile on its own because many of the
dependencies (ESP‑IDF headers, drivers, third‑party components) are
omitted for brevity.  Refer to the original project’s `PROJET_COMPLET.md`
for detailed build instructions when integrating this skeleton into a
fully configured ESP‑IDF environment.

## Directory Layout

The layout of this repository follows the structure described in
`PROJET_COMPLET.md`.  At a high level it contains:

* `main/` – the top‑level application entry point and modules
  implementing Wi‑Fi provisioning, HTTP and WebSocket servers, the
  SQLite database manager, sensor drivers, MQTT client, BLE services,
  security helpers, OTA update manager, and various utilities.
* `components/` – third‑party components (placeholders for SQLite,
  cJSON, and OneWire drivers).
* `docs/` – technical documentation copied from the files provided
  by the user.  See `ARCHITECTURE.md`, `PROJET_COMPLET.md` and
  `SPEC_GESTIONNAIRE_ELEVAGE_REPTILES.md` for full details.
* `tests/` – placeholder for unit, integration and load tests.
* `tools/` – helper scripts such as flashing and OTA upload.

## License

The original project is released under the MIT license.  This
skeleton includes only stub code and documentation; you should ensure
that your final implementation complies with the licensing terms of
any third‑party libraries you choose to integrate.
