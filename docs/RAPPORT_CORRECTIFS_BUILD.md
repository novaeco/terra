# Rapport correctifs build/flash ESP32-P4 (rev v1.3)

## Causes racines observées
- Warnings Kconfig sur des CHOICE forçant des entrées `…=n` : flash size (2MB), révision minimale (rev1), méthode de backtrace (NO_BACKTRACE). Vérification dans l’IDF 5.3 : seuls les symboles positifs `CONFIG_ESPTOOLPY_FLASHSIZE_*` et `CONFIG_ESP32P4_REV_MIN_*` existent (voir `components/esptool_py/Kconfig.projbuild` et `components/esp_hw_support/port/esp32p4/Kconfig.hw_support`), pas de symbole `CONFIG_ESP_SYSTEM_NO_BACKTRACE` ni `CONFIG_ESP_SYSTEM_USE_FRAME_POINTER`.
- Bootloader trop gros : limite 0x6000 octets entre `BOOTLOADER_OFFSET_IN_FLASH=0x2000` et partition table `0x8000` (Kconfig bootloader). Le build précédent affichait `USING O3`, indiquant une optimisation -O3 qui gonfle le binaire.
- Absence de partition table personnalisée dans le dépôt (aucun `partitions.csv`), donc offsets par défaut IDF.

## Modifications apportées
- Flash : sélection explicite du choix `CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y` dans `sdkconfig.defaults` (aucun forçage `…=n`).
- Révision minimale : sélection explicite de `CONFIG_ESP32P4_REV_MIN_100=y` (rev v1.0, compatible silicium v1.3) avec les symboles existants IDF.
- Optimisation : passage en `CONFIG_COMPILER_OPTIMIZATION_SIZE=y` (app) et `CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_SIZE=y` (bootloader) pour sortir du mode O3.
- Taille bootloader : réduction de la verbosité à `CONFIG_BOOTLOADER_LOG_LEVEL_WARN=y`.
- Nettoyage : retrait des symboles non présents dans l’IDF 5.3 (`CONFIG_ESP32P4_SELECTS_REV_LESS_V3`, `CONFIG_ESP_SYSTEM_USE_FRAME_POINTER`) afin d’éviter les warnings Kconfig sur des symboles inexistants.

Fichiers modifiés : `sdkconfig.defaults`, `docs/RAPPORT_CORRECTIFS_BUILD.md` (présent fichier).

## Mesures bootloader
- Limite théorique : `0x8000 - 0x2000 = 0x6000` octets.
- Build local non exécuté : installation des toolchains ESP-IDF (`./install.sh esp32p4`) bloquée par l’erreur SSL `CERTIFICATE_VERIFY_FAILED` (téléchargement GitHub). Aucune `build/bootloader/bootloader.bin` disponible dans l’arbre actuel.
- Vérification à refaire côté poste de build :
  ```bash
  idf.py fullclean build
  stat -c '%s' build/bootloader/bootloader.bin   # doit être < 0x6000 (24576) octets
  ```
  Si >0x6000 : vérifier que `BOOTLOADER_COMPILER_OPTIMIZATION_SIZE` et `BOOTLOADER_LOG_LEVEL_WARN` sont bien appliqués, puis envisager un allégement supplémentaire avant tout changement d’offset.

## Commandes de validation recommandées
1) Préparer l’environnement ESP-IDF (v5.3) :
```bash
cd /workspace/terra
export IDF_PATH=/path/vers/esp-idf   # ajuster selon l’installation
source $IDF_PATH/export.sh
idf.py set-target esp32p4
```
2) Build complet :
```bash
idf.py fullclean build
```
Attendus : pas de warnings Kconfig « Trying to set symbol … to n » sur les CHOICE flash-size / backtrace / révision ; pas d’échec `bootloader_check_size`.
3) Taille bootloader :
```bash
stat -c '%s' build/bootloader/bootloader.bin
```
Attendu : valeur < 24576.
4) Flash + boot :
```bash
idf.py -p COMx flash
idf.py -p COMx monitor
```
Attendus : flash sans rejet de révision (rev min v1.0), log bootloader en WARN uniquement, absence de reset WDT après boot si l’application le permet.

## Plan B (si taille toujours >0x6000 après -Os/log WARN)
- Recompiler après avoir désactivé des options bootloader coûteuses (ex. EH frame si activé) avant toute modification d’offsets.
- En dernier recours seulement : déplacer `partition_table_offset` au-delà de 0x8000 et recalculer les offsets dans `partitions.csv`, puis revalider `idf.py build/flash`.
