# Réglementation Numérique - FR/EU/International

## 1. Protection des Données Personnelles

### RGPD - Règlement Général sur la Protection des Données (UE 2016/679)

#### Principes fondamentaux (Article 5)
1. **Licéité, loyauté, transparence** : traitement légal et transparent
2. **Limitation des finalités** : objectifs déterminés et légitimes
3. **Minimisation** : données adéquates, pertinentes, limitées
4. **Exactitude** : données exactes, mises à jour
5. **Limitation de conservation** : durée nécessaire uniquement
6. **Intégrité et confidentialité** : sécurité appropriée
7. **Responsabilité** : démontrer conformité

#### Bases légales (Article 6)
- Consentement
- Exécution contrat
- Obligation légale
- Intérêts vitaux
- Mission intérêt public
- Intérêts légitimes

#### Droits des personnes
- Information (Art. 13-14)
- Accès (Art. 15)
- Rectification (Art. 16)
- Effacement / "droit à l'oubli" (Art. 17)
- Limitation traitement (Art. 18)
- Portabilité (Art. 20)
- Opposition (Art. 21)
- Non-profilage automatisé (Art. 22)

#### Obligations développeurs/entreprises
- Privacy by Design & by Default (Art. 25)
- Registre des traitements (Art. 30)
- Analyse d'impact (AIPD/DPIA) si risque élevé (Art. 35)
- DPO obligatoire selon critères (Art. 37)
- Notification violations (72h) (Art. 33)
- Sous-traitants : contrat conforme (Art. 28)

#### Sanctions
- Jusqu'à 20M€ ou 4% CA mondial
- Avertissements, mises en demeure
- Interdictions de traitement

### CNIL - France

#### Rôle
- Autorité de contrôle française RGPD
- Pouvoir de sanction
- Accompagnement, recommandations
- Contrôles (sur place, en ligne)

#### Obligations spécifiques France
- Déclaration ancienne remplacée par registre
- Recommandations sectorielles (cookies, santé, etc.)
- Transferts hors-UE : clauses types

#### Ressources
- Guides pratiques CNIL
- MOOC RGPD
- Référentiels sectoriels

### Cookies et traceurs

#### Règles
- Consentement préalable (sauf cookies strictement nécessaires)
- Information claire
- Refus aussi simple qu'acceptation
- Conservation consentement 6 mois (recommandation CNIL)

#### Cookies exemptés
- Authentification
- Panier e-commerce
- Analytics audience agrégée (conditions strictes)

## 2. IA Act - Règlement Européen sur l'Intelligence Artificielle

### Classification par risque

#### Risque inacceptable (interdit)
- Manipulation subliminale
- Exploitation vulnérabilités
- Scoring social par autorités
- Identification biométrique temps réel (exceptions)

#### Risque élevé (obligations strictes)
- Systèmes de recrutement
- Éducation, notation
- Services essentiels (crédit, assurance)
- Maintien de l'ordre, justice
- Migration, contrôle frontières

**Obligations risque élevé** :
- Gestion risques
- Données d'entraînement de qualité
- Documentation technique
- Transparence utilisateurs
- Supervision humaine
- Précision, robustesse, cybersécurité
- Marquage CE

#### Risque limité
- Chatbots, deepfakes
- **Obligation transparence** : informer que c'est une IA

#### Risque minimal
- Filtres spam, jeux vidéo IA
- Pas d'obligations spécifiques

### GPAI - General Purpose AI (modèles fondation)
- Obligations transparence
- Documentation technique
- Modèles à risque systémique : obligations renforcées

### Calendrier
- Entrée en vigueur : août 2024
- Application progressive : 2025-2027

## 3. Cybersécurité

### NIS2 - Directive (UE) 2022/2555

#### Entités concernées
**Essentielles** : énergie, transports, santé, eau, infrastructures numériques, espace, administration
**Importantes** : services postaux, déchets, chimie, agroalimentaire, fabrication, fournisseurs numériques

#### Obligations
- Mesures gestion risques cybersécurité
- Notification incidents (24h alerte, 72h rapport)
- Gouvernance : responsabilité direction
- Supply chain security

#### Sanctions
- Essentielles : jusqu'à 10M€ ou 2% CA
- Importantes : jusqu'à 7M€ ou 1,4% CA

### DORA - Digital Operational Resilience Act
Secteur financier :
- Tests résilience
- Gestion risques TIC
- Incidents reporting
- Prestataires TIC critiques

### Cyber Resilience Act (CRA)
Produits avec éléments numériques :
- Sécurité by design
- Mises à jour sécurité
- Transparence vulnérabilités

### France - ANSSI & LPM

#### ANSSI
- Autorité nationale cybersécurité
- Certification (CSPN, CC)
- Recommandations techniques
- Veille menaces

#### Opérateurs d'Importance Vitale (OIV)
- Déclaration incidents
- Audits obligatoires
- Mesures sécurité renforcées

## 4. Commerce Électronique & Services Numériques

### DSA - Digital Services Act

#### Obligations selon taille
**Tous intermédiaires** :
- Point de contact
- Représentant légal UE si hors-UE
- Conditions générales transparentes

**Hébergeurs** :
- Notice and action (signalement contenus illicites)
- Motivation décisions modération

**Plateformes** :
- Système signalement
- Traçabilité professionnels (marketplaces)
- Publicité : transparence, pas de ciblage mineurs

**Très grandes plateformes (>45M utilisateurs)** :
- Évaluation risques systémiques
- Audits indépendants
- Accès données chercheurs
- Redevance supervision

### DMA - Digital Markets Act
**Gatekeepers** (plateformes dominantes) :
- Interopérabilité messagerie
- Pas d'auto-préférence
- Portabilité données
- Accès équitable

### Directive e-commerce (2000/31/CE)
- Responsabilité limitée hébergeurs
- Principe pays d'origine
- Information précontractuelle

## 5. Propriété Intellectuelle & Logiciel

### Droit d'auteur logiciel

#### Protection automatique
- Code source et objet
- Interfaces (selon jurisprudence)
- Documentation

#### Non protégé
- Idées, algorithmes, méthodes
- Fonctionnalités
- Langages de programmation

### Licences logicielles

#### Propriétaires
- Droits limités utilisateur
- Code source non accessible
- Restrictions redistribution

#### Open source
**Permissives** : MIT, BSD, Apache
- Redistribution libre
- Modifications libres
- Attribution requise

**Copyleft** : GPL, AGPL, LGPL
- Dérivés sous même licence
- Code source accessible

#### Compatibilité
- Vérifier compatibilité licences
- Obligations contamination (copyleft)
- SBOM (Software Bill of Materials)

### Brevets logiciels
- **Europe** : programmes "en tant que tels" non brevetables
- **USA** : plus permissif (invention technique)
- Prudence contributions open source

### Bases de données
- Protection sui generis (investissement substantiel)
- Droit d'auteur sur structure originale

## 6. Accessibilité Numérique

### Directive (UE) 2016/2102 - Secteur public
- Sites web et applications mobiles
- Conformité WCAG 2.1 niveau AA
- Déclaration d'accessibilité

### European Accessibility Act (2019/882)
- Produits et services privés
- E-commerce, banque, transports, télécoms
- Échéance : juin 2025

### France - RGAA
- Référentiel Général d'Amélioration de l'Accessibilité
- Obligations légales (250k€+ CA ou >250 employés)
- Déclaration, schéma pluriannuel

## 7. Communications Électroniques

### Code européen communications électroniques
- Neutralité du net
- Portabilité numéro
- Droits consommateurs

### ePrivacy (en cours)
- Remplacera directive 2002/58
- Cookies, métadonnées, confidentialité communications

## 8. Contrats & Responsabilité

### Garantie légale de conformité
- 2 ans minimum (produits, contenus numériques)
- Mises à jour sécurité
- Interopérabilité documentée

### Responsabilité produits défectueux
- Extension aux logiciels (révision directive)
- IA : régimes spécifiques en discussion

### Conditions générales
- Clauses abusives B2C
- Lisibilité, accessibilité
- Consentement éclairé

## 9. Normes & Standards

### ISO 27001 - Sécurité information
- SMSI (Système Management Sécurité Information)
- Certification volontaire
- Annexe A : mesures

### ISO 27701 - Privacy
- Extension ISO 27001
- Gestion informations personnelles

### SOC 2
- Standard audit américain
- Trust Services Criteria
- Rapports Type I et II

### PCI DSS
- Données cartes paiement
- Obligations selon volume transactions

## 10. Cloud & Hébergement

### Localisation données
- RGPD : transferts hors-UE encadrés
- Clauses contractuelles types
- Décisions adéquation

### Cloud souverain
- SecNumCloud (France)
- EUCS (schéma européen en cours)
- Niveaux de sécurité

### Réversibilité
- Portabilité données
- Formats standards
- Délais préavis

## 11. Checklist Développeur

### Nouveau projet
- [ ] Privacy by Design dès conception
- [ ] Registre des traitements si données personnelles
- [ ] Licences dépendances vérifiées (SBOM)
- [ ] Accessibilité planifiée
- [ ] Sécurité intégrée (OWASP)

### Mise en production
- [ ] Mentions légales, CGU, politique confidentialité
- [ ] Bandeau cookies conforme
- [ ] Processus exercice droits RGPD
- [ ] Hébergement conforme (localisation, sécurité)
- [ ] Logging, monitoring incidents

### Maintenance
- [ ] Mises à jour sécurité régulières
- [ ] Revue dépendances vulnérables
- [ ] Tests accessibilité
- [ ] Documentation à jour