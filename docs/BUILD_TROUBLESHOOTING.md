# Dépannage compilation ESP-IDF / Windows

## Échecs de configuration CMake après mise à jour d’ESP-IDF

Lors d’une mise à jour ou d’un nettoyage partiel, certains binaires ou dépendances peuvent manquer.

### Vérification rapide

```powershell
cd UIperso
python tools/validate_idf_env.py
```

Le script contrôle que `IDF_PATH` est défini, pointe vers une arborescence valide et indique la version détectée.

### Remédiation recommandée

1. **S’assurer que l’environnement est chargé** (`export.bat`/`export.ps1` ou `idf.py --version`).
2. **Récupérer les sous-modules et dépendances** :
   ```powershell
   git -C %IDF_PATH% submodule update --init --recursive
   ```
3. **Nettoyer et relancer la configuration** :
   ```powershell
   idf.py fullclean
   idf.py set-target esp32s3
   ```
4. Si l’erreur persiste, lancer `idf.py doctor` puis réinstaller ESP-IDF via `idf-env.exe install` (corrige les installations corrompues ou partielles).

Après correction, relancer `idf.py build`.
