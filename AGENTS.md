# AGENTS – Projet UI professionnelle ESP32-S3 Waveshare Touch LCD 7B (Codex Agent)

Ce fichier définit les règles, le contexte et les attentes pour tout agent (Codex, GPT, etc.) travaillant sur ce dépôt.
Il sert de **spécification maîtresse** pour la conception d’une interface utilisateur embarquée professionnelle basée sur ESP-IDF 6.1 et LVGL 9.4, ciblant la carte Waveshare ESP32-S3-Touch-LCD-7B (1024×600) avec module ESP32-S3-WROOM-1-N16R8.

---

## 1. Objectif du dépôt

Fournir une **base de projet de production** pour une UI LVGL 9.4 sur ESP32-S3, prête à être étendue pour des applications de type :
- Dashboard de système de contrôle (terrarium, automation, domotique, etc.).
- Interface HMI professionnelle modulaire, maintenable, testable.
- Démonstrateur hardware complet de la carte Waveshare ESP32-S3-Touch-LCD-7B.

Le dépôt doit contenir un projet ESP-IDF 6.1 complet, compilable, avec :
- Intégration LVGL 9.4.
- Drivers display RGB 1024×600, tactile GT911, expandeur CH422G, µSD, bus CAN/RS485, gestion d’alimentation (stubs CS8501).
- UI structurée en panneaux (dashboard, system_panel, logs_panel) + thème LVGL custom.

Ce projet est une **base générique extensible**, pas un produit final figé.

---

## 2. Contexte matériel (référence)

Cible matérielle principale (non modifiable sans accord explicite) :

- **Carte** : Waveshare ESP32-S3-Touch-LCD-7B 1024×600.
- **MCU** : ESP32-S3-WROOM-1-N16R8 (16 Mo Flash, 8 Mo PSRAM).
- **Écran** : LCD 7" 1024×600, interface RGB parallèle, contrôleur type ST7262 / AXS15231B, pilotable via `esp_lcd_rgb_panel`.
- **Tactile** : GT911 (I²C + IRQ + RST).
- **Expandeur I/O** : CH422G (I²C) – contrôle du backlight, VCOM, sélection USB/CAN, CS µSD, reset touch, etc.
- **µSD** : Slot microSD en SPI.
- **CAN** : Transceiver TJA1051 (TWAI).
- **RS485** : ISL83485 (UART + DE/RE).
- **Power** : CS8501 (gestion batterie/charge – implémenté au minimum en stubs propres et extensibles).
- **PSRAM** : 8 Mo, utilisée en priorité pour les buffers graphiques LVGL (double buffer, framebuffers).

> IMPORTANT : le mapping GPIO exact de la carte **ne doit pas être inventé**. Il doit respecter la documentation officielle Waveshare pour ce modèle spécifique.  
> Si l’agent ne peut pas accéder à Internet, il doit :
> - Soit utiliser un mapping fourni dans ce dépôt (fichiers `docs/` ou commentaires dans le code).
> - Soit marquer clairement toute hypothèse de mapping dans des commentaires `/* TODO: vérifier mapping GPIO avec doc Waveshare */` **sans bloquer la compilation**.

---

## 3. Contexte logiciel et environnement

- **Framework** : ESP-IDF v6.1 (toolchain `xtensa-esp32s3-elf`).  
- **RTOS** : FreeRTOS (inclus dans ESP-IDF).  
- **UI** : LVGL v9.4 (déjà intégrée dans l’ESP-IDF du développeur – composant local).  
- **Ciblage** : `IDF_TARGET = esp32s3`.  
- **OS dev** : Windows (PowerShell / CMD) – mais le projet doit rester portable (Linux possible).

Règles pour l’agent :

1. **Ne pas télécharger LVGL** depuis Internet ni ajouter de sous-modules Git.  
   - Utiliser exclusivement le composant LVGL déjà présent dans le projet (par ex. `components/lvgl` ou composant `lvgl__lvgl`, selon la configuration explicitement documentée dans le dépôt).

2. **Pas d’ajout de bibliothèques tierces** non prévues (pas de nouveaux sous-modules, pas de dépendances réseau).  
   - Utiliser seulement : ESP-IDF, LVGL, et les composants déjà présents.

3. Si l’agent n’a **pas accès à Internet** :  
   - Il doit s’appuyer sur les spécifications de ce fichier et sur la structure existante du dépôt,  
   - Marquer clairement toute hypothèse matérielle non confirmée dans des commentaires, sans empêcher la compilation.

---

## 4. Règles de travail pour Codex / agents

### 4.1. Mode de travail par phases (OBLIGATOIRE)

Même si la spécification maître (ci-dessous) mentionne parfois “tout générer en une seule fois”, **ces instructions sont supersédées par AGENTS.md**.

Pour ce dépôt, **tu ne dois jamais essayer de tout générer/modifier en une seule réponse**.  
Travaille **par phases**, de manière incrémentale, par exemple :

1. **Phase 1 – Bootstrapping projet ESP-IDF**
   - Créer/ajuster :
     - `CMakeLists.txt` racine,
     - `main/CMakeLists.txt`,
     - `sdkconfig.defaults` minimal,
     - `main.c` minimal (init logs, SPIRAM, message de démarrage).
   - Objectif : `idf.py set-target esp32s3` + `idf.py build` doivent passer sans erreur.

2. **Phase 2 – Intégration LVGL + driver display RGB**
   - Créer/ajuster `components/display/` :
     - `rgb_lcd.c`, `rgb_lcd.h`, `CMakeLists.txt`,
     - `components/lvgl/lv_conf.h` (si géré localement ici).
   - Objectif : écran initialisé, fond visible, affichage d’un label simple.

3. **Phase 3 – Tactile GT911 + CH422G**
   - Créer/ajuster :
     - `components/gt911/` (driver tactile + indev LVGL),
     - `components/ch422g/` (backlight, VCOM, resets, sélecteurs).

4. **Phase 4 – µSD / stockage**
   - Créer/ajuster :
     - `components/storage/` (SPI µSD + FATFS + tests).

5. **Phase 5 – Bus CAN / RS485**
   - Créer/ajuster :
     - `components/bus/` (`can_driver.c/.h`, `rs485_driver.c/.h`), avec configuration de base.

6. **Phase 6 – UI manager + panneaux + thèmes**
   - Créer/ajuster :
     - `main/ui_manager.c/.h`, `dashboard.c/.h`, `system_panel.c/.h`, `logs_panel.c/.h`,
     - `main/lv_theme_custom.c/.h`,
     - Navigation et intégration des drivers précédents.

À chaque phase :  
- Ne modifier que les fichiers nécessaires.  
- Garder le projet **compilable** (aucun fichier manquant).  
- Mettre à jour les CMakeLists de façon cohérente.

### 4.2. Build & commandes standard

L’agent doit supposer les commandes standard ESP-IDF :

```bash
# Configuration cible (une seule fois)
idf.py set-target esp32s3

# Build
idf.py build

# Flash (adapter le port série)
idf.py -p COMx flash

# Monitor
idf.py -p COMx monitor
```

Sous Windows, l’agent peut mentionner PowerShell ou CMD, mais **ne doit pas supposer WSL obligatoirement**.

### 4.3. Style et qualité de code

- Langage : C pour les composants ESP-IDF / LVGL.
- Style :
  - Indentation 4 espaces, pas de tabulations.
  - Un `static const char *TAG = "MODULE"` par fichier `.c` pour les logs (`ESP_LOGI/W/E`).
  - Gestion d’erreurs explicite : utiliser `ESP_ERROR_CHECK()` pour les erreurs fatales (ex: init LCD), et tester/retourner des `esp_err_t` pour les autres.
- Commentaires : de préférence en **anglais** dans le code, les spécifications peuvent rester en français.

**Règle importante :**  
> Le code doit **toujours compiler** (à config correcte près) – aucun TODO ne doit bloquer la compilation.  
> Pour les fonctionnalités non encore implémentées mais prévues, utiliser des stubs documentés, par ex. :  
> `float cs8501_get_battery_voltage(void) { return 4.0f; /* TODO: implement real reading */ }`

---

## 5. Arborescence cible (référence fonctionnelle)

L’agent doit viser l’arborescence suivante (il peut l’enrichir, mais pas supprimer ces éléments sans raison sérieuse) :

```text
esp32s3_ui_project/
├── CMakeLists.txt
├── Makefile (optionnel)
├── sdkconfig.defaults
├── main/
│   ├── main.c
│   ├── ui_manager.c
│   ├── ui_manager.h
│   ├── dashboard.c
│   ├── dashboard.h
│   ├── system_panel.c
│   ├── system_panel.h
│   ├── logs_panel.c
│   ├── logs_panel.h
│   ├── events.c
│   ├── events.h
│   ├── lv_theme_custom.c
│   ├── lv_theme_custom.h
├── components/
│   ├── lvgl/
│   │   └── lv_conf.h
│   ├── display/
│   │   ├── CMakeLists.txt
│   │   ├── rgb_lcd.c
│   │   ├── rgb_lcd.h
│   ├── gt911/
│   │   ├── CMakeLists.txt
│   │   ├── gt911_touch.c
│   │   ├── gt911_touch.h
│   ├── ch422g/
│   │   ├── CMakeLists.txt
│   │   ├── ch422g.c
│   │   ├── ch422g.h
│   ├── storage/
│   │   ├── CMakeLists.txt
│   │   ├── sdcard.c
│   │   ├── sdcard.h
│   ├── bus/
│   │   ├── CMakeLists.txt
│   │   ├── can_driver.c
│   │   ├── can_driver.h
│   │   ├── rs485_driver.c
│   │   ├── rs485_driver.h
│   ├── power/
│   │   ├── CMakeLists.txt
│   │   ├── cs8501.c
│   │   ├── cs8501.h
├── ui_assets/
│   ├── fonts/
│   ├── icons/
│   └── themes/
```

---

## 6. Spécification maître (prompt utilisateur d’origine)

La section suivante est une **copie textuelle** du prompt maître fourni par le développeur.  
Elle définit les exigences fonctionnelles détaillées.  
En cas de **contradiction** entre ce bloc et les règles d’AGENTS.md (par ex. “tout générer en une fois”), **AGENTS.md prévaut**.
Pour le reste, l’agent doit suivre ces instructions avec la plus grande fidélité possible.

```text
Agis en tant qu’ingénieur systèmes embarqués senior, spécialiste mondial de l’ESP32-S3, d’ESP-IDF 6.1 (CMake, FreeRTOS, PSRAM, DMA, pilotes bas niveau), et d’interfaces graphiques professionnelles avec LVGL 9.4 sur écrans RGB parallèles haute résolution. Tu maîtrises également les bonnes pratiques de conception d’UI HMI (structure modulaire, séparation logique/UI, réactivité, cohérence graphique) et les pipelines graphiques optimisés pour microcontrôleurs (double buffering, PSRAM, DMA, réduction de la charge CPU).

Ta mission : à partir des éléments d’architecture et d’extraits de code ci-dessous, générer en UNE SEULE FOIS un projet COMPLET ESP-IDF 6.1 + LVGL 9.4, immédiatement compilable, sans fichiers manquants, sans TODO bloquant, structuré pour la PRODUCTION, ciblant la carte Waveshare ESP32-S3-Touch-LCD-7B 1024×600 basée sur un ESP32-S3-WROOM-1-N16R8 (16 Mo Flash, 8 Mo PSRAM).

Le projet doit fournir :

Une arborescence propre ESP-IDF avec main/ et components/ (display, touch, expandeur I/O, stockage, bus CAN/RS485, power).

Une intégration complète LVGL 9.4 (déjà présente dans l’ESP-IDF 6.1 local, tu NE télécharges PAS LVGL depuis Internet, tu utilises le composant LVGL fourni par l’IDF).

Des drivers fonctionnels pour l’écran LCD RGB 1024×600 (esp_lcd_rgb_panel), le tactile GT911 (I²C + IRQ), l’expandeur CH422G (I²C), la carte µSD (SPI), le CAN (TJA1051 / TWAI driver), le RS485 (UART + DE/RE), et la gestion d’alimentation CS8501 (au minimum stubs propres orientés production).

Une UI professionnelle complète basée sur LVGL 9.4, avec un gestionnaire d’UI modulaire (ui_manager) et plusieurs panneaux (dashboard, panneau système, logs), le tout stylé via un thème custom.

Une configuration FreeRTOS/LVGL optimisée (tick 1 ms, double buffer, buffers en PSRAM, pas d’allocation dynamique après initialisation de l’UI, flush DMA aligné).

Avant d’écrire la moindre ligne de code, commence par afficher une courte liste de questions de clarification pour moi (l’utilisateur), sous forme de liste numérotée, pour préciser :

La langue principale de l’UI (FR uniquement, EN uniquement, ou FR/EN).

Le style graphique (clair/sombre, couleurs dominantes, style “industriel/HMI”, “domotique moderne”, etc.).

Le niveau de détail souhaité pour les panneaux dashboard, system_panel et logs_panel (par exemple : simple placeholders vs. widgets concrets : indicateurs système (CPU, heap, PSRAM, FPS), état bus CAN/RS485, état µSD, tension batterie, etc.).

Le format exact de la sortie souhaitée (un seul gros bloc avec tout le code, ou sections par fichiers, avec arborescence détaillée, etc.).

Les contraintes éventuelles de mémoire (limitation de hauteur de buffer LVGL, taille maximale de double buffer, etc.) ou de performances (FPS cible, consommation maximum, etc.).

Une fois ces réponses obtenues, tu les intègres dans la conception et tu produis directement la version finale du projet complet.

────────────────────────────────────────────────────────
CONTEXTE ET CONTRAINTES
────────────────────────────────────────────────────────

Contexte matériel :

Cible : Waveshare ESP32-S3-Touch-LCD-7B 1024×600.

SoC : ESP32-S3-WROOM-1-N16R8 (16 Mo Flash, 8 Mo PSRAM).

Écran : LCD 7" 1024×600, interface RGB parallèle, contrôleur type ST7262/AXS15231B, pilotable via esp_lcd_rgb_panel.

Tactile : GT911 (I²C, IRQ, RST).

Expandeur I/O : CH422G (I²C) pour backlight, VCOM, sélection USB/CAN, CS µSD, reset touch, etc.

µSD : slot microSD en SPI.

CAN : transceiver TJA1051 (TWAI).

RS485 : ISL83485 (UART + DE/RE).

Power : CS8501 (gestion batterie/charge – tu peux fournir des fonctions stubs propres mais extensibles).

PSRAM : 8 Mo, à utiliser pour les buffers graphiques LVGL.

IMPORTANT :

Tu peux et DOIS utiliser Internet pour retrouver la documentation officielle Waveshare de cette carte, la datasheet du contrôleur LCD, le mapping GPIO complet, la doc GT911, CH422G, TJA1051, ISL83485, CS8501, et les meilleures pratiques LVGL 9.4 pour ESP32-S3.

Tu dois t’assurer que le mapping des broches (esp_lcd_rgb_panel_config_t + GPIO assignés) est cohérent avec la documentation Waveshare ESP32-S3-Touch-LCD-7B 1024×600 (ne pas inventer de pins aléatoires).

La fréquence PCLK, les timings HSYNC/VSYNC, porches, etc. doivent être cohérents avec le panneau 1024×600 (pclk typiquement > 30 MHz ; évite le 16 MHz trop bas si tu peux).

Contexte logiciel :

ESP-IDF : v6.1 (toolchain xtensa-esp32s3-elf).

LVGL : v9.4 (composant lvgl déjà présent dans l’IDF, NE PAS cloner ni télécharger LVGL).

FreeRTOS utilisé comme RTOS (tâche principale + éventuelles tâches de fond).

Tu peux utiliser les APIs modernes de l’IDF (esp_lcd, esp_timer, twai, etc.).

Contexte UI :

L’UI est une interface “générique” professionnelle pour cette carte, qui sert de base à un futur projet (dashboard de système de contrôle terrarium/automation, par exemple). Elle doit être propre, moderne, minimaliste mais extensible.

Panneaux de base :

dashboard : vue principale avec titre, quelques widgets (placeholders ou vrais widgets : chart, indicateurs, labels).

system_panel : affichage et éventuellement contrôle de paramètres système simples (brightness backlight, état µSD, info mémoire, toggle simulation bus, etc.).

logs_panel : zone de logs texte (type console) scrollable, alimentée par un simple wrapper de log interne.

Un thème LVGL personnalisé doit être appliqué globalement (style d’écran, couleurs par défaut, radius, etc.).

Style de réponse attendu :

Tu t’adresses à moi en utilisant “tu”.

Rédaction au masculin.

Style ultra-technique, dense, clair, structuré, orienté performance.

Pas de phrases superflues : contenu exploitable directement.

────────────────────────────────────────────────────────
ARBORESCENCE À PRODUIRE
────────────────────────────────────────────────────────

Tu dois produire l’arborescence suivante (tu peux l’enrichir si nécessaire, mais sans supprimer ces éléments) :

esp32s3_ui_project/
├── CMakeLists.txt
├── Makefile (optionnel, tu peux le fournir pour commodité, mais non obligatoire)
├── sdkconfig.defaults
├── main/
│ ├── main.c
│ ├── ui_manager.c
│ ├── ui_manager.h
│ ├── dashboard.c
│ ├── dashboard.h
│ ├── system_panel.c
│ ├── system_panel.h
│ ├── logs_panel.c
│ ├── logs_panel.h
│ ├── events.c
│ ├── events.h
│ ├── lv_theme_custom.c
│ ├── lv_theme_custom.h
├── components/
│ ├── lvgl/
│ │ └── lv_conf.h (fichier de conf LVGL 9.4 adapté à 1024×600, 16 bpp, PSRAM, tick custom)
│ ├── display/
│ │ ├── CMakeLists.txt
│ │ ├── rgb_lcd.c
│ │ ├── rgb_lcd.h
│ ├── gt911/
│ │ ├── CMakeLists.txt
│ │ ├── gt911_touch.c
│ │ ├── gt911_touch.h
│ ├── ch422g/
│ │ ├── CMakeLists.txt
│ │ ├── ch422g.c
│ │ ├── ch422g.h
│ ├── storage/
│ │ ├── CMakeLists.txt
│ │ ├── sdcard.c
│ │ ├── sdcard.h
│ ├── bus/
│ │ ├── CMakeLists.txt
│ │ ├── can_driver.c
│ │ ├── can_driver.h
│ │ ├── rs485_driver.c
│ │ ├── rs485_driver.h
│ ├── power/
│ │ ├── CMakeLists.txt
│ │ ├── cs8501.c
│ │ ├── cs8501.h
├── ui_assets/
│ ├── fonts/ (définir au moins une fonte LVGL intégrée)
│ ├── icons/ (tu peux créer quelques icônes LVGL symboliques)
│ └── themes/ (si tu veux séparer des styles spécifiques)

────────────────────────────────────────────────────────
CONTRAINTES SUR LE CODE À PRODUIRE
────────────────────────────────────────────────────────

[CETTE SECTION DÉTAILLÉE CONTINUE DANS LE PROMPT D’ORIGINE ET DOIT ÊTRE INTERPRÉTÉE PAR L’AGENT COMME DES EXIGENCES FONCTIONNELLES ET TECHNIQUES.  
AGENTS.md PRÉVAUT UNIQUEMENT SUR LES PARTIES PROCÉDURALES (MODE “UNE SEULE FOIS” VS TRAVAIL PAR PHASES).]
```

---

## 7. Priorisation en cas de limite de taille de réponse

Si l’agent ne peut pas fournir tout le code demandé dans une seule réponse (limite de tokens), il doit **prioriser** dans l’ordre suivant :

1. **Arborescence + fichiers CMake** (racine + `main/` + `components/*`).  
2. `main.c`, drivers `display/` (RGB) et intégration LVGL (incluant `lv_conf.h`).  
3. Drivers `gt911/`, `ch422g/`, `storage/` (µSD).  
4. Drivers `bus/` (CAN/RS485) + `power/` (CS8501 stubs).  
5. UI complète : `ui_manager`, panneaux (dashboard, system_panel, logs_panel), thème custom.  
6. Makefile optionnel, documentation additionnelle, commentaires plus détaillés.

À la fin de chaque réponse partielle, l’agent doit lister clairement :  
> « Fichiers non encore fournis (à générer dans une étape suivante) : … »

---

## 8. Règle d’or

> **Toujours laisser le projet dans un état compilable.**  
> Pas de fichiers manquants, pas de prototypes orphelins, pas de TODO bloquant la compilation.  
> Les fonctionnalités non implémentées doivent être stubées proprement.

Fin du fichier AGENTS.md.
