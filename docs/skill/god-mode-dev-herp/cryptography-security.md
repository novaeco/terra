# Cryptographie & Sécurité Avancée

## 1. Fondamentaux Cryptographiques

### Principes de base
- **Confidentialité** : seul le destinataire lit le message
- **Intégrité** : message non modifié
- **Authentification** : identité vérifiée
- **Non-répudiation** : preuve d'envoi/réception

### Types de cryptographie
- **Symétrique** : même clé pour chiffrer/déchiffrer
- **Asymétrique** : clé publique/clé privée
- **Hybride** : combinaison des deux

## 2. Cryptographie Symétrique

### Algorithmes de bloc
| Algorithme | Taille bloc | Taille clé | Status |
|------------|-------------|------------|--------|
| DES | 64 bits | 56 bits | Obsolète |
| 3DES | 64 bits | 168 bits | Déprécié |
| AES | 128 bits | 128/192/256 | Standard actuel |
| Blowfish | 64 bits | 32-448 | Remplacé |
| Twofish | 128 bits | 128-256 | Alternatif |
| ChaCha20 | 512 bits | 256 | Moderne, rapide |

### Modes d'opération
- **ECB** : chaque bloc indépendant (à éviter)
- **CBC** : chaînage, IV nécessaire
- **CTR** : compteur, parallélisable
- **GCM** : authentifié, standard recommandé
- **CCM** : authentifié, IoT

### AES (Advanced Encryption Standard)
- Substitution-Permutation Network
- 10/12/14 rounds selon taille clé
- SubBytes, ShiftRows, MixColumns, AddRoundKey
- Résistant aux attaques connues

### Algorithmes de flux
- RC4 : obsolète, vulnérabilités
- ChaCha20 : moderne, rapide (software)
- Salsa20 : prédécesseur ChaCha

## 3. Cryptographie Asymétrique

### RSA
- Basé sur factorisation grands nombres
- Clés : 2048-4096 bits recommandé
- Lent, utilisé pour échange de clés, signatures
- Opérations : c = m^e mod n, m = c^d mod n

### Diffie-Hellman
- Échange de clés sur canal non sécurisé
- Basé sur logarithme discret
- ECDH : version courbes elliptiques

### Courbes Elliptiques (ECC)
- Clés plus courtes (256 bits ≈ RSA 3072)
- Plus rapide que RSA
- Courbes : P-256, P-384, Curve25519, secp256k1

### Algorithmes ECC
- ECDSA : signatures
- ECDH : échange de clés
- EdDSA (Ed25519) : signatures modernes

## 4. Fonctions de Hachage

### Propriétés
- Déterministe
- Rapide à calculer
- Résistance pré-image
- Résistance seconde pré-image
- Résistance aux collisions

### Algorithmes
| Algorithme | Taille sortie | Status |
|------------|---------------|--------|
| MD5 | 128 bits | Cassé |
| SHA-1 | 160 bits | Déprécié |
| SHA-256 | 256 bits | Standard |
| SHA-3 | 224-512 bits | Alternatif |
| BLAKE2 | 256-512 bits | Rapide |
| BLAKE3 | 256+ bits | Très rapide |

### Hachage de mots de passe
- **Bcrypt** : coût ajustable, standard
- **Scrypt** : mémoire-dur
- **Argon2** : gagnant PHC, recommandé
- **PBKDF2** : NIST approuvé

### HMAC
- Hash-based Message Authentication Code
- HMAC(K, m) = H((K ⊕ opad) || H((K ⊕ ipad) || m))
- Authentification et intégrité

## 5. Signatures Numériques

### Principes
- Signer avec clé privée
- Vérifier avec clé publique
- Prouve authenticité et intégrité

### Algorithmes
- **RSA-PSS** : RSA avec padding probabiliste
- **ECDSA** : courbes elliptiques
- **EdDSA** : Ed25519, déterministe, rapide
- **DSA** : ancien standard

### Certificats X.509
- Identité + clé publique signée par CA
- Chaîne de confiance
- Révocation : CRL, OCSP

## 6. Protocoles Cryptographiques

### TLS 1.3
- Handshake en 1-RTT (0-RTT possible)
- Cipher suites simplifiées
- Forward secrecy obligatoire
- Suppression algorithmes obsolètes

### Signal Protocol
- Double Ratchet
- X3DH key agreement
- Forward secrecy, post-compromise security
- Utilisé par Signal, WhatsApp, etc.

### Noise Protocol Framework
- Handshakes flexibles
- Authentification optionnelle
- Utilisé par WireGuard

### SSH
- Authentification par clé
- Tunneling, port forwarding
- Algorithmes : Ed25519 recommandé

## 7. Cryptographie Post-Quantique

### Menace quantique
- Algorithme de Shor : casse RSA, ECC
- Algorithme de Grover : affaiblit symétrique (doubler clés)

### Familles post-quantiques
- **Lattices** : CRYSTALS-Kyber, CRYSTALS-Dilithium
- **Hash-based** : SPHINCS+
- **Code-based** : Classic McEliece
- **Isogenies** : SIKE (cassé)

### Standards NIST (2024)
- **ML-KEM** (Kyber) : encapsulation de clés
- **ML-DSA** (Dilithium) : signatures
- **SLH-DSA** (SPHINCS+) : signatures alternatives

## 8. Attaques Cryptographiques

### Attaques sur algorithmes
- **Brute force** : essai exhaustif
- **Cryptanalyse différentielle** : analyse des différences
- **Cryptanalyse linéaire** : approximations linéaires
- **Birthday attack** : collisions hash

### Attaques par canaux auxiliaires
- **Timing** : mesure du temps d'exécution
- **Power analysis** : consommation électrique
- **Cache timing** : accès mémoire
- **Acoustic** : émissions sonores

### Contre-mesures
- Temps constant
- Masquage (blinding)
- Bruit aléatoire
- Isolation physique

### Attaques protocolaires
- **Man-in-the-Middle** : interception
- **Replay** : rejeu de messages
- **Padding oracle** : fuite d'information
- **Downgrade** : forcer algorithme faible

## 9. Applications Pratiques

### Gestion des clés
- Génération : source d'aléa cryptographique
- Stockage : HSM, TPM, secure enclave
- Rotation : changement régulier
- Destruction : effacement sécurisé

### Chiffrement de fichiers
- GPG/PGP : standard, hybride
- Age : moderne, simple
- VeraCrypt : volumes chiffrés

### Chiffrement disque
- LUKS (Linux)
- BitLocker (Windows)
- FileVault (macOS)
- VeraCrypt (cross-platform)

### VPN
- WireGuard : moderne, simple, rapide
- OpenVPN : mature, flexible
- IPSec : standard entreprise

### Messagerie sécurisée
- Signal : référence
- Matrix/Element : décentralisé
- Wire : entreprise

## 10. Implémentation Sécurisée

### Règles d'or
1. Ne JAMAIS implémenter sa propre crypto
2. Utiliser des bibliothèques éprouvées
3. Temps constant pour opérations sensibles
4. Aléa cryptographiquement sûr uniquement
5. Zéroïser la mémoire après usage

### Bibliothèques recommandées
- **libsodium** : high-level, sûre par défaut
- **OpenSSL/BoringSSL** : complète, bas niveau
- **mbedTLS** : embarqué
- **crypto (Go)** : standard library
- **cryptography (Python)** : high-level

### Aléa cryptographique
- /dev/urandom (Linux)
- CryptGenRandom (Windows)
- getentropy() / arc4random()
- CSPRNG : générateurs déterministes seedés

### Erreurs courantes
- ECB mode
- IV/nonce réutilisé
- Padding non validé
- Comparaison timing-unsafe
- Aléa non cryptographique
- Clés hardcodées