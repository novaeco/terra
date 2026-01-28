# Spécification Technique - Gestionnaire d'Élevage de Reptiles

## 📋 TABLE DES MATIÈRES

1. [Vision & Objectifs](#1-vision--objectifs)
2. [Contraintes & Principes Architecturaux](#2-contraintes--principes-architecturaux)
3. [Modèle de Domaine](#3-modèle-de-domaine)
4. [Architecture Modulaire](#4-architecture-modulaire)
5. [Modules Fonctionnels](#5-modules-fonctionnels)
6. [Module Réglementaire (CORE)](#6-module-réglementaire-core)
7. [Intégration & APIs](#7-intégration--apis)
8. [Sécurité & Conformité](#8-sécurité--conformité)
9. [Qualité & Tests](#9-qualité--tests)
10. [Roadmap & Évolutivité](#10-roadmap--évolutivité)

---

## 1. VISION & OBJECTIFS

### 1.1 Vision Produit

Créer un système de gestion d'élevage de reptiles **modulaire**, **évolutif** et **conforme** qui permet aux éleveurs amateurs et professionnels de gérer leur collection en respectant toutes les obligations légales françaises, européennes et internationales.

### 1.2 Utilisateurs Cibles

- **Amateur débutant** : détention de 1-5 reptiles domestiques (Python regius, Pogona)
- **Amateur passionné** : 5-20 reptiles, espèces variées, début de reproduction
- **Éleveur amateur sérieux** : 20-50 reptiles, reproduction régulière, espèces CDC potentielles
- **Professionnel** : 50+ reptiles, vente, CDC/AOE obligatoires, activité commerciale

### 1.3 Objectifs Principaux

1. ✅ **Conformité réglementaire totale** : France, EU, CITES
2. 🧩 **Architecture modulaire** : pas de monolithe, services découplés
3. 📊 **Traçabilité complète** : registres, documents, historiques
4. 🚨 **Alertes intelligentes** : obligations, renouvellements, deadlines
5. 📱 **Multi-plateforme** : web, mobile, API
6. 🔒 **Sécurité & RGPD** : données sensibles protégées

### 1.4 Hors Périmètre (Phase 1)

❌ **Gestion climatique** : température, humidité, éclairage (module futur)
❌ Intégration IoT/capteurs (phase 2)
❌ Marketplace intégrée (phase 3)
❌ Réseau social/communauté (phase 4)

---

## 2. CONTRAINTES & PRINCIPES ARCHITECTURAUX

### 2.1 Contraintes Impératives

#### ⛔ ANTI-PATTERN : Architecture Monolithique

**CE QUE NOUS NE VOULONS PAS** :
```
❌ Application monolithique avec :
   - Tout le code dans un seul projet
   - Base de données unique centralisée
   - Couplage fort entre modules
   - Impossibilité de déployer indépendamment
   - Difficulté à faire évoluer
```

#### ✅ ARCHITECTURE CIBLE : Modulaire & Découplée

**CE QUE NOUS VOULONS** :
```
✅ Architecture modulaire avec :
   - Services indépendants découplés
   - Bounded contexts DDD clairs
   - APIs bien définies entre modules
   - Bases de données par contexte
   - Déploiement indépendant possible
   - Évolutivité par module
```

### 2.2 Principes Architecturaux

1. **Domain-Driven Design (DDD)**
   - Ubiquitous language : vocabulaire herpétologique précis
   - Bounded contexts : Animal, Réglementation, Élevage, Documents
   - Aggregates avec cohérence transactionnelle

2. **Hexagonal Architecture (Ports & Adapters)**
   - Core métier isolé des détails techniques
   - Ports : interfaces définies par le domaine
   - Adapters : implémentations techniques interchangeables

3. **Event-Driven Architecture**
   - Domain events pour communication inter-modules
   - Event sourcing pour traçabilité (optionnel par module)
   - Saga pattern pour transactions distribuées

4. **CQRS (Command Query Responsibility Segregation)**
   - Séparation lecture/écriture
   - Optimisation requêtes complexes
   - Audit trail automatique

### 2.3 Technologies Recommandées

#### Backend
- **Langages** : TypeScript/Node.js, Python (FastAPI), Go
- **APIs** : REST + GraphQL (requêtes complexes)
- **Events** : RabbitMQ, Apache Kafka, NATS
- **Bases de données** :
  - PostgreSQL (données structurées, JSONB pour flexibilité)
  - MongoDB (documents, historiques)
  - Redis (cache, sessions)

#### Frontend
- **Framework** : React + TypeScript, Next.js (SSR/SSG)
- **State Management** : Zustand, TanStack Query
- **UI** : Tailwind CSS, shadcn/ui
- **Mobile** : React Native, Capacitor

#### Infrastructure
- **Conteneurisation** : Docker, Docker Compose
- **Orchestration** : Kubernetes (production), Docker Swarm (dev)
- **CI/CD** : GitHub Actions, GitLab CI
- **Monitoring** : Prometheus, Grafana, Sentry

---

## 3. MODÈLE DE DOMAINE

### 3.1 Bounded Contexts (DDD)

```
┌─────────────────────────────────────────────────────┐
│                    SYSTÈME GLOBAL                   │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌──────────────────┐  ┌──────────────────┐       │
│  │  ANIMAL CONTEXT  │  │   REG CONTEXT    │       │
│  │                  │  │                  │       │
│  │  - Animal        │  │  - Législation   │       │
│  │  - Espèce        │  │  - Autorisation  │       │
│  │  - Identification│  │  - Document      │       │
│  │  - Santé         │  │  - Conformité    │       │
│  └──────────────────┘  └──────────────────┘       │
│           │                      │                 │
│           └──────────┬───────────┘                 │
│                      │                             │
│  ┌──────────────────┴──────────────────┐          │
│  │      ÉLEVAGE CONTEXT                │          │
│  │                                     │          │
│  │  - Collection                       │          │
│  │  - Reproduction                     │          │
│  │  - Alimentation                     │          │
│  │  - Registres                        │          │
│  └─────────────────────────────────────┘          │
│                      │                             │
│  ┌──────────────────┴──────────────────┐          │
│  │      DOCUMENT CONTEXT                │          │
│  │                                     │          │
│  │  - Génération                       │          │
│  │  - Stockage                         │          │
│  │  - Export                           │          │
│  │  - Signature                        │          │
│  └─────────────────────────────────────┘          │
│                                                     │
│  ┌─────────────────────────────────────┐          │
│  │       USER/AUTH CONTEXT              │          │
│  │                                     │          │
│  │  - Utilisateur                      │          │
│  │  - Profil (amateur/pro)             │          │
│  │  - Autorisations                    │          │
│  └─────────────────────────────────────┘          │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### 3.2 Entités Principales

#### Animal Aggregate

```typescript
interface Animal {
  id: UUID;
  espece: EspeceVO;
  identification: Identification;
  dateNaissance?: Date;
  dateAcquisition: Date;
  sexe: 'M' | 'F' | 'INCONNU';
  provenance: Provenance;
  statut: 'ACTIF' | 'VENDU' | 'DECEDE' | 'CEDE';
  marquage?: Marquage;
  historique: HistoriqueEvent[];
  metadonnees: Record<string, any>;
}

interface EspeceVO {
  nomScientifique: string;
  nomCommun: string;
  famille: string;
  genre: string;
  espece: string;
  sousEspece?: string;
  statutDomestique: boolean;
  categorieReglementaire: CategorieReglementaire;
}

interface Identification {
  numero: string; // Numéro interne
  puce?: string; // RFID
  photo?: URL;
  caracteristiquesUniques?: string;
}

interface Marquage {
  type: 'PUCE' | 'PHOTO' | 'ECAILLURE';
  numero?: string;
  localisation?: string;
  dateMarquage: Date;
  autorite?: string;
}

interface Provenance {
  type: 'ACHAT' | 'REPRODUCTION' | 'DON' | 'SAUVETAGE';
  vendeur?: string;
  eleveur?: string;
  numeroSIRET?: string;
  facture?: DocumentRef;
  certificatCession?: DocumentRef;
  parents?: { pere?: UUID; mere?: UUID };
}

type HistoriqueEvent = 
  | AcquisitionEvent
  | ReproductionEvent
  | SanteEvent
  | VenteEvent
  | DecesEvent
  | ChangementStatutEvent;
```

#### Réglementation Aggregate

```typescript
interface StatutReglementaire {
  animal: UUID;
  espece: EspeceVO;
  
  // Statut France
  categorieNational: 'DOMESTIQUE' | 'NON_DOMESTIQUE' | 'PROTEGE';
  niveauAutorisation: 'AUCUN' | 'DECLARATION' | 'CDC_AOE';
  colonne: 'a' | 'b' | 'c' | null;
  seuilDepasse: boolean;
  
  // Statut CITES/EU
  annexeCITES?: 'I' | 'II' | 'III';
  annexeUE?: 'A' | 'B' | 'C' | 'D';
  certificatRequis: boolean;
  
  // Documents associés
  documents: DocumentReglementaire[];
  
  // Conformité
  conforme: boolean;
  alertes: Alerte[];
  actionsRequises: ActionRequise[];
}

interface DocumentReglementaire {
  type: 'CDC' | 'AOE' | 'DECLARATION' | 'CIC' | 'FACTURE' | 'ATTESTATION_CESSION';
  numero?: string;
  dateEmission?: Date;
  dateExpiration?: Date;
  autorite?: string;
  fichier?: URL;
  statut: 'VALIDE' | 'EXPIRE' | 'EN_ATTENTE' | 'REFUSE';
}

interface Alerte {
  niveau: 'INFO' | 'AVERTISSEMENT' | 'CRITIQUE';
  type: string;
  message: string;
  dateEcheance?: Date;
  actionsRecommandees: string[];
}

interface ActionRequise {
  id: UUID;
  type: 'OBTENIR_CDC' | 'DECLARER' | 'RENOUVELER_CIC' | 'MARQUER_ANIMAL';
  priorite: 'BASSE' | 'MOYENNE' | 'HAUTE' | 'URGENTE';
  description: string;
  deadline?: Date;
  etapes: Etape[];
  statut: 'A_FAIRE' | 'EN_COURS' | 'TERMINEE';
}
```

#### Collection Aggregate

```typescript
interface Collection {
  id: UUID;
  proprietaire: UUID;
  type: 'AMATEUR' | 'PROFESSIONNEL';
  
  animaux: UUID[];
  
  // Statistiques
  statistiques: {
    total: number;
    parEspece: Map<string, number>;
    parStatut: Map<string, number>;
    parCategorieReglementaire: Map<string, number>;
  };
  
  // Registre légal
  registre: RegistreEntreesSorties;
  
  // Conformité globale
  conformite: ConformiteCollection;
}

interface RegistreEntreesSorties {
  entries: RegistreEntry[];
}

interface RegistreEntry {
  id: UUID;
  type: 'ENTREE' | 'SORTIE' | 'NAISSANCE' | 'DECES';
  date: Date;
  animal: UUID;
  espece: string;
  provenance?: string;
  destination?: string;
  numeroCITES?: string;
  documents: DocumentRef[];
  observations?: string;
}

interface ConformiteCollection {
  statut: 'CONFORME' | 'NON_CONFORME' | 'ATTENTION';
  dernierControle: Date;
  problemes: ProblemeConformite[];
  recommandations: string[];
}
```

#### Reproduction Aggregate

```typescript
interface CycleReproduction {
  id: UUID;
  saison: number; // Année
  
  couple: {
    male: UUID;
    femelle: UUID;
    espece: EspeceVO;
  };
  
  // Conditionnement
  conditionnement: {
    debut: Date;
    fin: Date;
    brumation: boolean;
    parametres?: Record<string, any>;
  };
  
  // Accouplement
  accouplements: Accouplement[];
  
  // Ponte/Gestation
  ponte?: Ponte;
  
  // Incubation
  incubation?: Incubation;
  
  // Résultats
  resultats: ResultatReproduction;
}

interface Ponte {
  date: Date;
  nombreOeufs: number;
  oeufsViables: number;
  oeufsInfertiles: number;
  observations?: string;
}

interface Incubation {
  debut: Date;
  temperatureMoyenne: number;
  substratsUtilise: string;
  incubateur?: string;
  eclosions: Eclosion[];
}

interface Eclosion {
  date: Date;
  oeufNumero: number;
  nouveauNe: UUID;
  poids?: number;
  taille?: number;
  observations?: string;
}

interface ResultatReproduction {
  oeufsTotal: number;
  oeufsViables: number;
  naissance: number;
  survival30j: number;
  survival1an: number;
  mortalite: number;
  anomalies: string[];
}
```

---

## 4. ARCHITECTURE MODULAIRE

### 4.1 Structure des Services

```
reptile-manager/
│
├── services/
│   ├── animal-service/          # Gestion des animaux
│   │   ├── src/
│   │   │   ├── domain/          # Entités, value objects, domain events
│   │   │   ├── application/     # Use cases, commands, queries
│   │   │   ├── infrastructure/  # Repositories, adapters
│   │   │   └── api/             # REST/GraphQL endpoints
│   │   ├── tests/
│   │   ├── Dockerfile
│   │   └── package.json
│   │
│   ├── regulation-service/      # Module réglementaire (CORE)
│   │   ├── src/
│   │   │   ├── domain/
│   │   │   │   ├── legislation/ # Base de données légale
│   │   │   │   ├── compliance/  # Moteur de conformité
│   │   │   │   └── alerts/      # Système d'alertes
│   │   │   ├── application/
│   │   │   ├── infrastructure/
│   │   │   └── api/
│   │   ├── data/
│   │   │   ├── species/         # Base espèces
│   │   │   ├── regulations/     # Textes légaux
│   │   │   └── thresholds/      # Seuils réglementaires
│   │   ├── tests/
│   │   ├── Dockerfile
│   │   └── package.json
│   │
│   ├── breeding-service/        # Gestion élevage
│   │   ├── src/
│   │   │   ├── domain/
│   │   │   ├── application/
│   │   │   ├── infrastructure/
│   │   │   └── api/
│   │   ├── tests/
│   │   ├── Dockerfile
│   │   └── package.json
│   │
│   ├── document-service/        # Génération documents
│   │   ├── src/
│   │   │   ├── domain/
│   │   │   ├── application/
│   │   │   ├── infrastructure/
│   │   │   └── api/
│   │   ├── templates/           # Templates PDF/Word
│   │   ├── tests/
│   │   ├── Dockerfile
│   │   └── package.json
│   │
│   ├── user-service/            # Authentification & profils
│   │   ├── src/
│   │   ├── tests/
│   │   ├── Dockerfile
│   │   └── package.json
│   │
│   └── notification-service/    # Alertes & notifications
│       ├── src/
│       ├── tests/
│       ├── Dockerfile
│       └── package.json
│
├── frontend/
│   ├── web/                     # Application web React
│   ├── mobile/                  # Application mobile React Native
│   └── shared/                  # Composants partagés
│
├── infrastructure/
│   ├── docker-compose.yml       # Orchestration locale
│   ├── k8s/                     # Manifests Kubernetes
│   ├── terraform/               # Infrastructure as Code
│   └── scripts/
│
├── shared/
│   ├── events/                  # Définitions domain events
│   ├── types/                   # Types TypeScript partagés
│   └── utils/
│
└── docs/
    ├── architecture/
    ├── api/
    └── regulations/
```

### 4.2 Communication Inter-Services

#### Event-Driven Communication

```typescript
// Domain Events
interface DomainEvent {
  id: UUID;
  aggregateId: UUID;
  aggregateType: string;
  eventType: string;
  version: number;
  timestamp: Date;
  data: Record<string, any>;
  metadata?: Record<string, any>;
}

// Exemples d'events
interface AnimalAcquisEvent extends DomainEvent {
  eventType: 'ANIMAL_ACQUIRED';
  data: {
    animalId: UUID;
    especeId: string;
    dateAcquisition: Date;
    provenance: Provenance;
  };
}

interface ReproductionSuccessEvent extends DomainEvent {
  eventType: 'REPRODUCTION_SUCCESS';
  data: {
    cycleId: UUID;
    parents: { male: UUID; female: UUID };
    nombreNouveauNes: number;
    dateNaissance: Date;
  };
}

interface ComplianceAlertEvent extends DomainEvent {
  eventType: 'COMPLIANCE_ALERT';
  data: {
    animalId: UUID;
    alertLevel: 'INFO' | 'WARNING' | 'CRITICAL';
    reason: string;
    actionRequired?: string;
    deadline?: Date;
  };
}
```

#### API Contracts

```typescript
// REST API Convention
// animal-service
GET    /api/v1/animals                 # Liste animaux
GET    /api/v1/animals/:id             # Détail animal
POST   /api/v1/animals                 # Créer animal
PUT    /api/v1/animals/:id             # Modifier animal
DELETE /api/v1/animals/:id             # Supprimer animal (soft delete)

GET    /api/v1/animals/:id/history     # Historique
GET    /api/v1/animals/:id/health      # Santé
POST   /api/v1/animals/:id/health      # Ajouter événement santé

// regulation-service
GET    /api/v1/regulations/species/:scientificName     # Statut espèce
GET    /api/v1/regulations/animals/:id/status          # Statut réglementaire
POST   /api/v1/regulations/animals/:id/check           # Vérifier conformité
GET    /api/v1/regulations/animals/:id/requirements    # Actions requises
GET    /api/v1/regulations/alerts                      # Alertes utilisateur

POST   /api/v1/regulations/documents                   # Upload document
GET    /api/v1/regulations/documents/:id               # Récupérer document
PUT    /api/v1/regulations/documents/:id/validate      # Valider document

// document-service
POST   /api/v1/documents/generate/registry             # Registre entrées-sorties
POST   /api/v1/documents/generate/certificate          # Attestation de cession
POST   /api/v1/documents/generate/declaration          # Déclaration préfectorale
GET    /api/v1/documents/:id/download                  # Télécharger
```

---

## 5. MODULES FONCTIONNELS

### 5.1 Module Animal

#### Responsabilités
- Gestion CRUD des animaux
- Identification et marquage
- Historique des événements
- Santé basique (sans paramètres climatiques)
- Photos et caractéristiques

#### Use Cases Principaux

```typescript
// Commands
class CreateAnimalCommand {
  espece: EspeceVO;
  dateAcquisition: Date;
  provenance: Provenance;
  sexe: 'M' | 'F' | 'INCONNU';
  identification: Identification;
  documents: DocumentRef[];
}

class UpdateAnimalHealthCommand {
  animalId: UUID;
  type: 'CONSULTATION' | 'TRAITEMENT' | 'VACCINATION' | 'MUE' | 'OBSERVATION';
  date: Date;
  description: string;
  veterinaire?: string;
  documents?: DocumentRef[];
}

class MarkAnimalCommand {
  animalId: UUID;
  typeMarquage: 'PUCE' | 'PHOTO' | 'ECAILLURE';
  numero?: string;
  localisation?: string;
  photos?: File[];
}

// Queries
class GetAnimalQuery {
  animalId: UUID;
}

class SearchAnimalsQuery {
  filters: {
    espece?: string;
    statut?: string;
    sexe?: string;
    dateAcquisitionMin?: Date;
    dateAcquisitionMax?: Date;
  };
  sort?: { field: string; order: 'asc' | 'desc' };
  pagination?: { page: number; limit: number };
}

class GetAnimalHistoryQuery {
  animalId: UUID;
  eventTypes?: string[];
  dateFrom?: Date;
  dateTo?: Date;
}
```

#### Base de Données (PostgreSQL)

```sql
-- Schema animal_service

CREATE TABLE animals (
  id UUID PRIMARY KEY,
  espece_nom_scientifique VARCHAR(255) NOT NULL,
  espece_nom_commun VARCHAR(255),
  espece_famille VARCHAR(100),
  identification_numero VARCHAR(50) UNIQUE NOT NULL,
  identification_puce VARCHAR(50) UNIQUE,
  date_naissance DATE,
  date_acquisition DATE NOT NULL,
  sexe VARCHAR(10) CHECK (sexe IN ('M', 'F', 'INCONNU')),
  statut VARCHAR(20) CHECK (statut IN ('ACTIF', 'VENDU', 'DECEDE', 'CEDE')),
  provenance_type VARCHAR(20) NOT NULL,
  provenance_vendeur VARCHAR(255),
  provenance_siret VARCHAR(14),
  metadonnees JSONB,
  created_at TIMESTAMP NOT NULL DEFAULT NOW(),
  updated_at TIMESTAMP NOT NULL DEFAULT NOW(),
  deleted_at TIMESTAMP
);

CREATE TABLE animal_events (
  id UUID PRIMARY KEY,
  animal_id UUID NOT NULL REFERENCES animals(id),
  event_type VARCHAR(50) NOT NULL,
  event_date TIMESTAMP NOT NULL,
  data JSONB NOT NULL,
  created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

CREATE TABLE animal_photos (
  id UUID PRIMARY KEY,
  animal_id UUID NOT NULL REFERENCES animals(id),
  url VARCHAR(500) NOT NULL,
  description TEXT,
  date_prise TIMESTAMP,
  created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

CREATE TABLE animal_health_records (
  id UUID PRIMARY KEY,
  animal_id UUID NOT NULL REFERENCES animals(id),
  record_type VARCHAR(50) NOT NULL,
  date TIMESTAMP NOT NULL,
  description TEXT,
  veterinaire VARCHAR(255),
  documents JSONB,
  created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_animals_espece ON animals(espece_nom_scientifique);
CREATE INDEX idx_animals_statut ON animals(statut);
CREATE INDEX idx_animal_events_animal ON animal_events(animal_id, event_date DESC);
CREATE INDEX idx_animal_health_animal ON animal_health_records(animal_id, date DESC);
```

### 5.2 Module Breeding (Élevage)

#### Responsabilités
- Gestion des cycles de reproduction
- Suivi conditionnement, accouplement, ponte
- Incubation et éclosions
- Généalogie
- Statistiques de reproduction

#### Use Cases Principaux

```typescript
// Commands
class StartBreedingCycleCommand {
  maleId: UUID;
  femaleId: UUID;
  saison: number;
  conditioningStart: Date;
  conditioningEnd: Date;
  brumation: boolean;
  notes?: string;
}

class RecordMatingCommand {
  cycleId: UUID;
  date: Date;
  duration?: number;
  observations?: string;
}

class RecordClutchCommand {
  cycleId: UUID;
  date: Date;
  nombreOeufs: number;
  oeufsViables: number;
  poids?: number;
  photos?: File[];
}

class StartIncubationCommand {
  cycleId: UUID;
  temperature: number;
  substrate: string;
  incubateur?: string;
}

class RecordHatchingCommand {
  cycleId: UUID;
  oeufNumero: number;
  date: Date;
  nouveauNeData: CreateAnimalCommand;
  poids?: number;
  taille?: number;
}

// Queries
class GetBreedingCyclesQuery {
  filters?: {
    saison?: number;
    espece?: string;
    statut?: 'EN_COURS' | 'TERMINE' | 'ABANDONNE';
  };
}

class GetOffspringQuery {
  cycleId: UUID;
}

class GetGenealogyQuery {
  animalId: UUID;
  depth?: number; // Nombre de générations
}
```

#### Calculs Génétiques (Module Optionnel)

```typescript
// Calculateur de morphs (exemple Python royal)
interface GeneticCalculator {
  calculateOffspring(
    male: GeneticProfile,
    female: GeneticProfile
  ): OffspringProbabilities;
}

interface GeneticProfile {
  morph: string;
  genes: Gene[];
}

interface Gene {
  locus: string;
  alleles: [string, string]; // Exemple: ['Normal', 'Pastel']
  dominance: 'DOMINANT' | 'RECESSIVE' | 'CO_DOMINANT';
}

interface OffspringProbabilities {
  combinations: {
    morph: string;
    probability: number; // 0.0 - 1.0
    genotype: string;
  }[];
}
```

### 5.3 Module Document

#### Responsabilités
- Génération de documents légaux
- Templates PDF/Word
- Stockage sécurisé
- Historique et versioning
- Export registres

#### Documents Générés

1. **Registre d'Entrées-Sorties**
   - Conforme arrêté 8/10/2018
   - Colonnes réglementaires obligatoires
   - Export PDF/Excel

2. **Attestation de Cession**
   - Entre particuliers ou vers professionnel
   - Informations vendeur/acheteur
   - Détails animal et documents

3. **Déclaration Préfectorale**
   - Formulaire CERFA pré-rempli
   - Liste des animaux détenus
   - Adresse installation

4. **Certificat Intra-Communautaire (préparation)**
   - Espèces Annexe A
   - Informations marquage
   - Preuves origine

5. **Facture de Vente**
   - Obligatoire pour professionnels
   - Mentions légales
   - TVA si applicable

#### Templates System

```typescript
interface DocumentTemplate {
  id: string;
  name: string;
  type: DocumentType;
  format: 'PDF' | 'DOCX' | 'XLSX';
  version: string;
  variables: TemplateVariable[];
  template: string; // HTML, LaTeX, or template file path
}

interface TemplateVariable {
  key: string;
  type: 'STRING' | 'DATE' | 'NUMBER' | 'BOOLEAN' | 'ARRAY' | 'OBJECT';
  required: boolean;
  description: string;
  example?: any;
}

// Générateur de documents
class DocumentGenerator {
  async generate(
    templateId: string,
    data: Record<string, any>
  ): Promise<GeneratedDocument> {
    // 1. Valider les données
    // 2. Charger le template
    // 3. Remplir les variables
    // 4. Générer le fichier
    // 5. Signer numériquement (optionnel)
    // 6. Stocker et retourner l'URL
  }
}
```

---

## 6. MODULE RÉGLEMENTAIRE (CORE)

### 6.1 Vue d'Ensemble

Le module réglementaire est le **cœur** du système. Il contient :
- Base de données complète des espèces et leur statut
- Moteur de conformité
- Système d'alertes et d'actions requises
- Intégration des textes légaux

### 6.2 Base de Données Légale

#### Structure des Données Espèces

```typescript
interface SpeciesRegulation {
  // Taxonomie
  taxonomy: {
    scientificName: string; // Nom complet (Genre espèce sous-espèce)
    commonName: string[];
    family: string;
    genus: string;
    species: string;
    subspecies?: string;
    synonyms: string[]; // Anciens noms scientifiques
  };
  
  // Statut France
  france: {
    domestic: boolean; // Liste arrêté domestiques 2006
    category: 'DOMESTIQUE' | 'NON_DOMESTIQUE' | 'PROTEGE';
    column: 'a' | 'b' | 'c' | null; // Arrêté 8/10/2018
    thresholds?: {
      a?: number; // Seuil détention libre
      b?: number; // Seuil déclaration
      c?: number; // CDC requis au-delà
    };
    dangerous: boolean; // Annexe 3 espèces dangereuses
    protected: boolean; // Espèces protégées françaises
    notes?: string;
  };
  
  // Statut CITES
  cites: {
    appendix?: 'I' | 'II' | 'III';
    population?: string; // Certaines populations seulement
    reservations: string[]; // Pays avec réserves
    annotations?: string;
    quotas?: Record<string, number>; // Par pays exportateur
  };
  
  // Statut EU
  eu: {
    annex?: 'A' | 'B' | 'C' | 'D';
    intraCommunityPermitRequired: boolean; // CIC
    proofOfOriginRequired: boolean;
    restrictions: string[];
  };
  
  // Espèces envahissantes
  invasive: {
    euList: boolean; // Règlement 1143/2014
    forbiddenActivities: string[]; // import, vente, reproduction, etc.
    restrictions: string;
  };
  
  // Réglementation autres pays
  international: Record<string, {
    status: string;
    restrictions: string[];
  }>;
  
  // Métadonnées
  sources: LegalReference[];
  lastUpdated: Date;
  version: string;
}

interface LegalReference {
  type: 'ARRETE' | 'DECRET' | 'LOI' | 'REGLEMENT_EU' | 'CONVENTION';
  name: string;
  date: Date;
  url?: string;
  article?: string;
}
```

#### Textes Légaux Stockés

```typescript
interface LegalText {
  id: UUID;
  type: 'LOI' | 'DECRET' | 'ARRETE' | 'CIRCULAIRE' | 'REGLEMENT_EU' | 'CONVENTION';
  title: string;
  reference: string; // Ex: "Arrêté du 8 octobre 2018"
  date: Date;
  effectiveDate: Date;
  jurisdiction: 'FRANCE' | 'EU' | 'INTERNATIONAL';
  
  // Contenu
  fullText: string;
  articles: Article[];
  annexes: Annexe[];
  
  // Relations
  amends: UUID[]; // Textes modifiés
  amendedBy: UUID[]; // Textes modificateurs
  supersedes: UUID[]; // Textes remplacés
  supersededBy: UUID[]; // Remplacé par
  
  // Métadonnées
  pdfUrl?: string;
  officialUrl?: string;
  keywords: string[];
  lastUpdated: Date;
}

interface Article {
  number: string;
  title?: string;
  content: string;
  subArticles?: SubArticle[];
}

interface Annexe {
  number: string;
  title: string;
  content: string; // Markdown ou HTML
  tables?: Table[];
}
```

#### Base de Données Espèces (Seed Data)

```typescript
// Exemples d'espèces pré-configurées
const speciesDatabase: SpeciesRegulation[] = [
  {
    taxonomy: {
      scientificName: 'Python regius',
      commonName: ['Python royal', 'Ball python'],
      family: 'Pythonidae',
      genus: 'Python',
      species: 'regius',
      synonyms: []
    },
    france: {
      domestic: true,
      category: 'DOMESTIQUE',
      column: null,
      dangerous: false,
      protected: false,
      notes: 'Espèce domestique, aucune autorisation requise'
    },
    cites: {
      appendix: 'II'
    },
    eu: {
      annex: 'B',
      intraCommunityPermitRequired: false,
      proofOfOriginRequired: true,
      restrictions: []
    },
    invasive: {
      euList: false,
      forbiddenActivities: [],
      restrictions: ''
    },
    sources: [
      {
        type: 'ARRETE',
        name: 'Arrêté du 11 août 2006 fixant la liste des espèces domestiques',
        date: new Date('2006-08-11')
      }
    ],
    lastUpdated: new Date('2024-01-15'),
    version: '1.2'
  },
  
  {
    taxonomy: {
      scientificName: 'Testudo hermanni',
      commonName: ["Tortue d'Hermann"],
      family: 'Testudinidae',
      genus: 'Testudo',
      species: 'hermanni',
      synonyms: []
    },
    france: {
      domestic: false,
      category: 'PROTEGE',
      column: 'c',
      dangerous: false,
      protected: true,
      notes: 'Espèce protégée française. Détention uniquement spécimens nés en captivité avec preuves origine.'
    },
    cites: {
      appendix: 'II'
    },
    eu: {
      annex: 'A',
      intraCommunityPermitRequired: true, // CIC obligatoire
      proofOfOriginRequired: true,
      restrictions: ['Marquage obligatoire (puce)', 'CIC pour toute cession']
    },
    invasive: {
      euList: false,
      forbiddenActivities: [],
      restrictions: ''
    },
    sources: [
      {
        type: 'ARRETE',
        name: 'Arrêté du 8 octobre 2018',
        date: new Date('2018-10-08')
      },
      {
        type: 'REGLEMENT_EU',
        name: 'Règlement CE 338/97',
        date: new Date('1997-12-09')
      }
    ],
    lastUpdated: new Date('2024-01-15'),
    version: '1.3'
  },
  
  {
    taxonomy: {
      scientificName: 'Trachemys scripta elegans',
      commonName: ['Tortue de Floride', 'Red-eared slider'],
      family: 'Emydidae',
      genus: 'Trachemys',
      species: 'scripta',
      subspecies: 'elegans',
      synonyms: []
    },
    france: {
      domestic: false,
      category: 'NON_DOMESTIQUE',
      column: 'b',
      dangerous: false,
      protected: false,
      notes: 'INTERDICTION commercialisation et reproduction depuis 2017'
    },
    cites: {
      appendix: 'III'
    },
    eu: {
      annex: 'B',
      intraCommunityPermitRequired: false,
      proofOfOriginRequired: true,
      restrictions: ['Espèce envahissante : vente et reproduction interdites']
    },
    invasive: {
      euList: true, // Règlement 1143/2014
      forbiddenActivities: [
        'IMPORT',
        'VENTE',
        'REPRODUCTION',
        'RELACHER',
        'TRANSPORT',
        'UTILISATION_COMMERCIALE'
      ],
      restrictions: 'Détention possible uniquement spécimens acquis avant interdiction. Pas de cession. Stérilisation recommandée.'
    },
    sources: [
      {
        type: 'REGLEMENT_EU',
        name: 'Règlement UE 1143/2014 (Espèces envahissantes)',
        date: new Date('2014-10-22')
      }
    ],
    lastUpdated: new Date('2024-01-15'),
    version: '1.1'
  },
  
  {
    taxonomy: {
      scientificName: 'Pogona vitticeps',
      commonName: ['Agame barbu', 'Bearded dragon'],
      family: 'Agamidae',
      genus: 'Pogona',
      species: 'vitticeps',
      synonyms: ['Amphibolurus vitticeps']
    },
    france: {
      domestic: true,
      category: 'DOMESTIQUE',
      column: null,
      dangerous: false,
      protected: false,
      notes: 'Espèce domestique, aucune autorisation requise'
    },
    cites: {
      // Non listé CITES
    },
    eu: {
      // Non listé EU
      intraCommunityPermitRequired: false,
      proofOfOriginRequired: false,
      restrictions: []
    },
    invasive: {
      euList: false,
      forbiddenActivities: [],
      restrictions: ''
    },
    sources: [
      {
        type: 'ARRETE',
        name: 'Arrêté du 11 août 2006 modifié',
        date: new Date('2017-05-29')
      }
    ],
    lastUpdated: new Date('2024-01-15'),
    version: '1.1'
  }
  
  // ... + 500 autres espèces pré-configurées
];
```

### 6.3 Moteur de Conformité

#### Architecture du Moteur

```typescript
interface ComplianceEngine {
  checkAnimalCompliance(animalId: UUID): ComplianceResult;
  checkCollectionCompliance(collectionId: UUID): ComplianceResult;
  suggestActions(animalId: UUID): ActionRequise[];
  validateDocuments(documents: DocumentReglementaire[]): ValidationResult;
}

interface ComplianceResult {
  compliant: boolean;
  status: 'CONFORME' | 'ATTENTION' | 'NON_CONFORME';
  checks: ComplianceCheck[];
  alerts: Alerte[];
  actions: ActionRequise[];
  summary: string;
}

interface ComplianceCheck {
  id: string;
  category: 'IDENTIFICATION' | 'AUTORISATION' | 'DOCUMENT' | 'SEUIL' | 'MARQUAGE';
  rule: string;
  passed: boolean;
  details: string;
  severity: 'INFO' | 'WARNING' | 'ERROR';
  references: LegalReference[];
}
```

#### Règles de Conformité (Rules Engine)

```typescript
// Règle : Animal domestique = pas d'autorisation
class DomesticSpeciesRule implements ComplianceRule {
  check(animal: Animal, regulation: SpeciesRegulation): ComplianceCheck {
    if (regulation.france.domestic) {
      return {
        id: 'DOMESTIC_SPECIES',
        category: 'AUTORISATION',
        rule: 'Espèce domestique : aucune autorisation requise',
        passed: true,
        details: `${regulation.taxonomy.scientificName} est une espèce domestique`,
        severity: 'INFO',
        references: [/* sources */]
      };
    }
    return this.checkNonDomestic(animal, regulation);
  }
  
  private checkNonDomestic(animal: Animal, regulation: SpeciesRegulation): ComplianceCheck {
    // Logique pour espèces non-domestiques
    // ...
  }
}

// Règle : Seuils de détention
class ThresholdRule implements ComplianceRule {
  check(
    animal: Animal,
    regulation: SpeciesRegulation,
    collection: Collection
  ): ComplianceCheck {
    if (!regulation.france.thresholds) {
      return { passed: true, /* ... */ };
    }
    
    const count = collection.animaux.filter(
      a => a.espece.nomScientifique === regulation.taxonomy.scientificName
    ).length;
    
    const thresholds = regulation.france.thresholds;
    
    if (thresholds.c && count > thresholds.c) {
      return {
        id: 'THRESHOLD_CDC_REQUIRED',
        category: 'AUTORISATION',
        rule: 'Dépassement seuil CDC',
        passed: false,
        details: `Vous détenez ${count} individus de ${regulation.taxonomy.scientificName}. ` +
                 `Au-delà de ${thresholds.c}, un Certificat de Capacité est obligatoire.`,
        severity: 'ERROR',
        references: [/* Arrêté 8/10/2018 */]
      };
    }
    
    if (thresholds.b && count > thresholds.b) {
      return {
        id: 'THRESHOLD_DECLARATION_REQUIRED',
        category: 'AUTORISATION',
        rule: 'Dépassement seuil déclaration',
        passed: false,
        details: `Vous détenez ${count} individus. Une déclaration préfectorale est requise.`,
        severity: 'WARNING',
        references: [/* Arrêté 8/10/2018 */]
      };
    }
    
    return { passed: true, /* ... */ };
  }
}

// Règle : CIC Annexe A
class IntraCommunityPermitRule implements ComplianceRule {
  check(animal: Animal, regulation: SpeciesRegulation): ComplianceCheck {
    if (regulation.eu.annex === 'A' && regulation.eu.intraCommunityPermitRequired) {
      const hasCIC = animal.documents?.some(d => d.type === 'CIC' && d.statut === 'VALIDE');
      
      return {
        id: 'CIC_REQUIRED',
        category: 'DOCUMENT',
        rule: 'Certificat Intra-Communautaire obligatoire',
        passed: hasCIC,
        details: hasCIC
          ? 'CIC présent et valide'
          : `Espèce Annexe A EU : CIC obligatoire pour toute détention et cession`,
        severity: hasCIC ? 'INFO' : 'ERROR',
        references: [/* Règlement CE 338/97 */]
      };
    }
    
    return { passed: true, /* ... */ };
  }
}

// Règle : Marquage obligatoire
class MarkingRule implements ComplianceRule {
  check(animal: Animal, regulation: SpeciesRegulation): ComplianceCheck {
    // Espèces Annexe A nécessitent marquage
    if (regulation.eu.annex === 'A') {
      const isMarked = animal.marquage != null;
      
      return {
        id: 'MARKING_REQUIRED',
        category: 'MARQUAGE',
        rule: 'Marquage obligatoire (puce électronique)',
        passed: isMarked,
        details: isMarked
          ? `Marquage : ${animal.marquage!.type} n°${animal.marquage!.numero}`
          : 'Espèce Annexe A : marquage obligatoire (puce électronique recommandée)',
        severity: isMarked ? 'INFO' : 'WARNING',
        references: [/* Règlement CE 338/97 */]
      };
    }
    
    return { passed: true, /* ... */ };
  }
}

// Règle : Espèces envahissantes
class InvasiveSpeciesRule implements ComplianceRule {
  check(animal: Animal, regulation: SpeciesRegulation): ComplianceCheck {
    if (regulation.invasive.euList) {
      return {
        id: 'INVASIVE_SPECIES',
        category: 'AUTORISATION',
        rule: 'Espèce envahissante : restrictions',
        passed: false,
        details: `${regulation.taxonomy.scientificName} est une espèce envahissante. ` +
                 `Interdiction : ${regulation.invasive.forbiddenActivities.join(', ')}. ` +
                 `Détention possible uniquement si acquis avant interdiction. Pas de cession possible.`,
        severity: 'ERROR',
        references: [/* Règlement 1143/2014 */]
      };
    }
    
    return { passed: true, /* ... */ };
  }
}

// Orchestrateur de règles
class ComplianceOrchestrator {
  private rules: ComplianceRule[] = [
    new DomesticSpeciesRule(),
    new ThresholdRule(),
    new IntraCommunityPermitRule(),
    new MarkingRule(),
    new InvasiveSpeciesRule(),
    // ... autres règles
  ];
  
  async checkCompliance(animal: Animal, collection: Collection): Promise<ComplianceResult> {
    const regulation = await this.getRegulation(animal.espece.nomScientifique);
    
    const checks = await Promise.all(
      this.rules.map(rule => rule.check(animal, regulation, collection))
    );
    
    const compliant = checks.every(c => c.passed);
    const hasErrors = checks.some(c => c.severity === 'ERROR');
    const hasWarnings = checks.some(c => c.severity === 'WARNING');
    
    const status = hasErrors ? 'NON_CONFORME' : hasWarnings ? 'ATTENTION' : 'CONFORME';
    
    const alerts = this.generateAlerts(checks);
    const actions = this.generateActions(checks, animal, regulation);
    
    return {
      compliant,
      status,
      checks,
      alerts,
      actions,
      summary: this.generateSummary(status, checks)
    };
  }
  
  private generateAlerts(checks: ComplianceCheck[]): Alerte[] {
    return checks
      .filter(c => !c.passed)
      .map(c => ({
        niveau: c.severity === 'ERROR' ? 'CRITIQUE' : 'AVERTISSEMENT',
        type: c.category,
        message: c.details,
        actionsRecommandees: [c.rule]
      }));
  }
  
  private generateActions(
    checks: ComplianceCheck[],
    animal: Animal,
    regulation: SpeciesRegulation
  ): ActionRequise[] {
    const actions: ActionRequise[] = [];
    
    // Exemple : si CIC manquant
    const cicCheck = checks.find(c => c.id === 'CIC_REQUIRED' && !c.passed);
    if (cicCheck) {
      actions.push({
        id: uuidv4(),
        type: 'OBTENIR_CIC',
        priorite: 'HAUTE',
        description: `Obtenir un Certificat Intra-Communautaire pour ${regulation.taxonomy.scientificName}`,
        etapes: [
          { ordre: 1, titre: 'Marquer l\'animal (puce électronique)', statut: 'A_FAIRE' },
          { ordre: 2, titre: 'Préparer dossier : preuves origine captivité, photos', statut: 'A_FAIRE' },
          { ordre: 3, titre: 'Déposer demande DREAL/DRIEAT', statut: 'A_FAIRE' },
          { ordre: 4, titre: 'Attendre visite inspection (possible)', statut: 'A_FAIRE' },
          { ordre: 5, titre: 'Recevoir CIC', statut: 'A_FAIRE' }
        ],
        statut: 'A_FAIRE'
      });
    }
    
    // Exemple : si seuil déclaration dépassé
    const thresholdCheck = checks.find(c => c.id === 'THRESHOLD_DECLARATION_REQUIRED' && !c.passed);
    if (thresholdCheck) {
      actions.push({
        id: uuidv4(),
        type: 'DECLARER',
        priorite: 'MOYENNE',
        description: 'Effectuer une déclaration de détention en préfecture',
        deadline: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000), // 30 jours
        etapes: [
          { ordre: 1, titre: 'Télécharger formulaire CERFA', statut: 'A_FAIRE' },
          { ordre: 2, titre: 'Remplir déclaration avec liste complète animaux', statut: 'A_FAIRE' },
          { ordre: 3, titre: 'Joindre preuves origine (factures)', statut: 'A_FAIRE' },
          { ordre: 4, titre: 'Envoyer à DDPP/DDETSPP', statut: 'A_FAIRE' }
        ],
        statut: 'A_FAIRE'
      });
    }
    
    return actions;
  }
  
  private generateSummary(status: string, checks: ComplianceCheck[]): string {
    const totalChecks = checks.length;
    const passedChecks = checks.filter(c => c.passed).length;
    const errors = checks.filter(c => !c.passed && c.severity === 'ERROR').length;
    const warnings = checks.filter(c => !c.passed && c.severity === 'WARNING').length;
    
    if (status === 'CONFORME') {
      return `Conformité totale : ${passedChecks}/${totalChecks} vérifications passées.`;
    } else if (status === 'ATTENTION') {
      return `Conformité avec réserves : ${warnings} avertissement(s) détecté(s).`;
    } else {
      return `Non conforme : ${errors} erreur(s) critique(s) et ${warnings} avertissement(s).`;
    }
  }
}
```

### 6.4 Système d'Alertes Proactives

```typescript
// Scheduler d'alertes
class ComplianceAlertScheduler {
  async scheduleChecks() {
    // Vérifications quotidiennes
    cron.schedule('0 8 * * *', () => {
      this.checkExpiringDocuments();
      this.checkNewRegulations();
    });
    
    // Vérifications hebdomadaires
    cron.schedule('0 9 * * 1', () => {
      this.checkThresholds();
      this.checkMissingDocuments();
    });
    
    // Vérifications mensuelles
    cron.schedule('0 10 1 * *', () => {
      this.generateComplianceReports();
    });
  }
  
  async checkExpiringDocuments() {
    // Documents expirant dans les 30 jours
    const expiringDocs = await this.findExpiringDocuments(30);
    
    for (const doc of expiringDocs) {
      await this.sendAlert({
        niveau: 'AVERTISSEMENT',
        type: 'DOCUMENT_EXPIRING',
        message: `Votre ${doc.type} expire le ${doc.dateExpiration}`,
        dateEcheance: doc.dateExpiration,
        actionsRecommandees: [
          'Préparer dossier de renouvellement',
          'Contacter l\'autorité compétente'
        ]
      });
    }
  }
  
  async checkNewRegulations() {
    // Vérifier si de nouveaux textes légaux ont été publiés
    const newRegulations = await this.fetchNewRegulations();
    
    if (newRegulations.length > 0) {
      await this.sendAlert({
        niveau: 'INFO',
        type: 'NEW_REGULATION',
        message: `${newRegulations.length} nouveau(x) texte(s) réglementaire(s) publié(s)`,
        actionsRecommandees: [
          'Consulter les nouvelles réglementations',
          'Vérifier l\'impact sur votre collection'
        ]
      });
    }
  }
}

// Notification Service
interface NotificationChannel {
  sendEmail(to: string, subject: string, body: string): Promise<void>;
  sendPush(userId: UUID, title: string, body: string): Promise<void>;
  sendSMS(phone: string, message: string): Promise<void>;
}

class NotificationService {
  async notifyUser(userId: UUID, alert: Alerte) {
    const user = await this.getUser(userId);
    const preferences = user.notificationPreferences;
    
    if (preferences.email && this.shouldNotifyByEmail(alert)) {
      await this.channels.sendEmail(
        user.email,
        `[Alerte ${alert.niveau}] ${alert.type}`,
        this.renderEmailTemplate(alert)
      );
    }
    
    if (preferences.push && this.shouldNotifyByPush(alert)) {
      await this.channels.sendPush(
        userId,
        alert.type,
        alert.message
      );
    }
    
    // SMS uniquement pour alertes critiques
    if (alert.niveau === 'CRITIQUE' && preferences.sms) {
      await this.channels.sendSMS(
        user.phone,
        alert.message.substring(0, 160)
      );
    }
  }
  
  private shouldNotifyByEmail(alert: Alerte): boolean {
    return alert.niveau !== 'INFO';
  }
  
  private shouldNotifyByPush(alert: Alerte): boolean {
    return true; // Toutes les alertes en push
  }
}
```

---

## 7. INTÉGRATION & APIs

### 7.1 API Gateway Pattern

```typescript
// Gateway central pour tous les services
class APIGateway {
  private services = {
    animal: new AnimalServiceClient(),
    regulation: new RegulationServiceClient(),
    breeding: new BreedingServiceClient(),
    document: new DocumentServiceClient(),
    user: new UserServiceClient()
  };
  
  async handleRequest(req: Request): Promise<Response> {
    // 1. Authentification
    const user = await this.authenticate(req);
    
    // 2. Authorization
    if (!await this.authorize(user, req)) {
      return { status: 403, body: 'Forbidden' };
    }
    
    // 3. Rate limiting
    if (!await this.checkRateLimit(user)) {
      return { status: 429, body: 'Too many requests' };
    }
    
    // 4. Router vers le bon service
    const route = this.parseRoute(req.path);
    const service = this.services[route.service];
    
    // 5. Circuit breaker
    return await this.withCircuitBreaker(() => {
      return service.handleRequest(req);
    });
  }
}
```

### 7.2 GraphQL Schema (Requêtes Complexes)

```graphql
type Query {
  # Animals
  animal(id: ID!): Animal
  animals(
    filters: AnimalFilters
    sort: SortInput
    pagination: PaginationInput
  ): AnimalConnection!
  
  # Regulations
  speciesRegulation(scientificName: String!): SpeciesRegulation
  animalCompliance(animalId: ID!): ComplianceResult!
  collectionCompliance: ComplianceResult!
  myAlerts(filters: AlertFilters): [Alert!]!
  myActions(status: ActionStatus): [ActionRequise!]!
  
  # Breeding
  breedingCycle(id: ID!): BreedingCycle
  breedingCycles(filters: BreedingFilters): [BreedingCycle!]!
  offspring(cycleId: ID!): [Animal!]!
  genealogy(animalId: ID!, depth: Int = 3): Genealogy!
  
  # Documents
  document(id: ID!): Document
  myDocuments(type: DocumentType): [Document!]!
  
  # Statistics
  collectionStats: CollectionStatistics!
  breedingStats(year: Int): BreedingStatistics!
}

type Mutation {
  # Animals
  createAnimal(input: CreateAnimalInput!): Animal!
  updateAnimal(id: ID!, input: UpdateAnimalInput!): Animal!
  deleteAnimal(id: ID!): Boolean!
  markAnimal(id: ID!, marking: MarkingInput!): Animal!
  
  # Breeding
  startBreedingCycle(input: StartBreedingInput!): BreedingCycle!
  recordMating(cycleId: ID!, input: MatingInput!): BreedingCycle!
  recordClutch(cycleId: ID!, input: ClutchInput!): BreedingCycle!
  recordHatching(cycleId: ID!, input: HatchingInput!): BreedingCycle!
  
  # Documents
  generateDocument(template: DocumentTemplate!, data: JSON!): Document!
  uploadDocument(file: Upload!, metadata: DocumentMetadata!): Document!
  
  # Regulations
  checkCompliance(animalId: ID!): ComplianceResult!
  resolveAction(actionId: ID!): ActionRequise!
}

type Animal {
  id: ID!
  species: SpeciesInfo!
  identification: Identification!
  sex: Sex!
  status: AnimalStatus!
  dateOfBirth: Date
  dateOfAcquisition: Date!
  provenance: Provenance!
  marking: Marking
  photos: [Photo!]!
  healthRecords: [HealthRecord!]!
  
  # Relations
  parents: Parents
  offspring: [Animal!]!
  breedingCycles: [BreedingCycle!]!
  
  # Regulatory
  regulatoryStatus: RegulatoryStatus!
  documents: [Document!]!
  compliance: ComplianceResult!
  
  # Metadata
  createdAt: DateTime!
  updatedAt: DateTime!
}

type ComplianceResult {
  compliant: Boolean!
  status: ComplianceStatus!
  checks: [ComplianceCheck!]!
  alerts: [Alert!]!
  actions: [ActionRequise!]!
  summary: String!
}

enum ComplianceStatus {
  CONFORME
  ATTENTION
  NON_CONFORME
}

type Alert {
  id: ID!
  level: AlertLevel!
  type: String!
  message: String!
  deadline: Date
  recommendedActions: [String!]!
  createdAt: DateTime!
}

enum AlertLevel {
  INFO
  AVERTISSEMENT
  CRITIQUE
}

type ActionRequise {
  id: ID!
  type: ActionType!
  priority: Priority!
  description: String!
  deadline: Date
  steps: [ActionStep!]!
  status: ActionStatus!
}

enum ActionType {
  OBTENIR_CDC
  DECLARER
  RENOUVELER_CIC
  MARQUER_ANIMAL
  OBTENIR_FACTURE
  AUTRE
}

enum Priority {
  BASSE
  MOYENNE
  HAUTE
  URGENTE
}

type ActionStep {
  order: Int!
  title: String!
  description: String
  status: StepStatus!
  completedAt: DateTime
}

enum StepStatus {
  A_FAIRE
  EN_COURS
  TERMINEE
  BLOQUEE
}
```

### 7.3 Webhooks (Events Publics)

```typescript
// Système de webhooks pour intégrations tierces
interface WebhookConfig {
  id: UUID;
  userId: UUID;
  url: string;
  events: WebhookEvent[];
  secret: string; // Pour signature HMAC
  active: boolean;
  retryPolicy: RetryPolicy;
}

enum WebhookEvent {
  ANIMAL_CREATED = 'animal.created',
  ANIMAL_UPDATED = 'animal.updated',
  ANIMAL_DELETED = 'animal.deleted',
  BREEDING_STARTED = 'breeding.started',
  HATCHING_RECORDED = 'breeding.hatching',
  COMPLIANCE_ALERT = 'compliance.alert',
  DOCUMENT_GENERATED = 'document.generated',
}

interface WebhookPayload {
  id: UUID;
  event: WebhookEvent;
  timestamp: Date;
  data: Record<string, any>;
  signature: string; // HMAC-SHA256
}

class WebhookService {
  async dispatch(event: WebhookEvent, data: any) {
    const webhooks = await this.getActiveWebhooksForEvent(event);
    
    for (const webhook of webhooks) {
      const payload = this.createPayload(webhook, event, data);
      await this.send(webhook, payload);
    }
  }
  
  private createPayload(
    webhook: WebhookConfig,
    event: WebhookEvent,
    data: any
  ): WebhookPayload {
    const payload = {
      id: uuidv4(),
      event,
      timestamp: new Date(),
      data
    };
    
    const signature = this.signPayload(payload, webhook.secret);
    
    return { ...payload, signature };
  }
  
  private signPayload(payload: any, secret: string): string {
    const hmac = crypto.createHmac('sha256', secret);
    hmac.update(JSON.stringify(payload));
    return hmac.digest('hex');
  }
  
  private async send(webhook: WebhookConfig, payload: WebhookPayload) {
    try {
      await axios.post(webhook.url, payload, {
        headers: {
          'Content-Type': 'application/json',
          'X-Webhook-Signature': payload.signature
        },
        timeout: 5000
      });
    } catch (error) {
      await this.handleFailure(webhook, payload, error);
    }
  }
  
  private async handleFailure(
    webhook: WebhookConfig,
    payload: WebhookPayload,
    error: any
  ) {
    // Exponential backoff retry
    const retries = webhook.retryPolicy.maxRetries;
    const delay = webhook.retryPolicy.initialDelay;
    
    for (let i = 0; i < retries; i++) {
      await this.sleep(delay * Math.pow(2, i));
      try {
        await this.send(webhook, payload);
        return; // Success
      } catch (retryError) {
        // Continue
      }
    }
    
    // Échec définitif : désactiver webhook ?
    await this.logFailure(webhook, payload, error);
  }
}
```

---

## 8. SÉCURITÉ & CONFORMITÉ

### 8.1 Sécurité des Données

#### Chiffrement

```typescript
// Chiffrement des données sensibles
class EncryptionService {
  private algorithm = 'aes-256-gcm';
  
  encrypt(plaintext: string, key: Buffer): EncryptedData {
    const iv = crypto.randomBytes(16);
    const cipher = crypto.createCipheriv(this.algorithm, key, iv);
    
    let ciphertext = cipher.update(plaintext, 'utf8', 'hex');
    ciphertext += cipher.final('hex');
    
    const authTag = cipher.getAuthTag();
    
    return {
      ciphertext,
      iv: iv.toString('hex'),
      authTag: authTag.toString('hex')
    };
  }
  
  decrypt(encrypted: EncryptedData, key: Buffer): string {
    const decipher = crypto.createDecipheriv(
      this.algorithm,
      key,
      Buffer.from(encrypted.iv, 'hex')
    );
    
    decipher.setAuthTag(Buffer.from(encrypted.authTag, 'hex'));
    
    let plaintext = decipher.update(encrypted.ciphertext, 'hex', 'utf8');
    plaintext += decipher.final('utf8');
    
    return plaintext;
  }
}

// Données à chiffrer
interface SensitiveData {
  // Informations personnelles
  fullName: string;
  email: string;
  phone?: string;
  address?: Address;
  
  // Documents légaux
  idCardNumber?: string;
  siretNumber?: string;
  
  // Informations animaux (si confidentielles)
  veterinaryRecords?: string;
}
```

#### Gestion des Secrets

```typescript
// Secrets Manager Integration
class SecretsManager {
  async getSecret(key: string): Promise<string> {
    // AWS Secrets Manager, HashiCorp Vault, etc.
    return await this.vaultClient.read(`secret/data/${key}`);
  }
  
  async rotateSecret(key: string): Promise<void> {
    const newSecret = this.generateSecureSecret();
    await this.vaultClient.write(`secret/data/${key}`, newSecret);
    await this.notifyServices(key);
  }
  
  private generateSecureSecret(): string {
    return crypto.randomBytes(32).toString('base64');
  }
}

// Configuration
const config = {
  database: {
    host: process.env.DB_HOST,
    password: await secretsManager.getSecret('db-password'),
    encryptionKey: await secretsManager.getSecret('db-encryption-key')
  },
  jwt: {
    secret: await secretsManager.getSecret('jwt-secret'),
    expiresIn: '24h'
  },
  api: {
    key: await secretsManager.getSecret('api-key')
  }
};
```

### 8.2 RGPD Compliance

```typescript
// Consentements RGPD
interface GDPRConsent {
  userId: UUID;
  purpose: 'ESSENTIAL' | 'ANALYTICS' | 'MARKETING' | 'THIRD_PARTY';
  granted: boolean;
  grantedAt?: Date;
  revokedAt?: Date;
  ipAddress: string;
  userAgent: string;
}

class GDPRService {
  // Droit d'accès
  async exportUserData(userId: UUID): Promise<UserDataExport> {
    const animals = await this.animalService.getByOwner(userId);
    const documents = await this.documentService.getByOwner(userId);
    const breeding = await this.breedingService.getByOwner(userId);
    
    return {
      user: await this.userService.get(userId),
      animals,
      documents,
      breeding,
      exportedAt: new Date()
    };
  }
  
  // Droit à l'effacement
  async deleteUserData(userId: UUID): Promise<void> {
    // 1. Anonymiser plutôt que supprimer (traçabilité légale)
    await this.userService.anonymize(userId);
    
    // 2. Conserver données légales requises (registres)
    await this.markLegalDataAsArchived(userId);
    
    // 3. Supprimer données non-essentielles
    await this.deleteNonEssentialData(userId);
  }
  
  // Droit à la portabilité
  async generatePortableData(userId: UUID): Promise<Buffer> {
    const data = await this.exportUserData(userId);
    return this.convertToJSON(data); // Format JSON standard
  }
}

// Audit Trail RGPD
interface GDPRAuditLog {
  id: UUID;
  userId: UUID;
  action: 'ACCESS' | 'EXPORT' | 'DELETE' | 'RECTIFY';
  dataType: string;
  timestamp: Date;
  ipAddress: string;
  legal_basis: 'CONSENT' | 'CONTRACT' | 'LEGAL_OBLIGATION';
}
```

### 8.3 Authentification & Autorisation

```typescript
// JWT-based Auth
interface JWTPayload {
  sub: UUID; // User ID
  email: string;
  role: 'USER' | 'PROFESSIONAL' | 'ADMIN';
  permissions: Permission[];
  iat: number;
  exp: number;
}

enum Permission {
  ANIMAL_READ = 'animal:read',
  ANIMAL_WRITE = 'animal:write',
  ANIMAL_DELETE = 'animal:delete',
  BREEDING_READ = 'breeding:read',
  BREEDING_WRITE = 'breeding:write',
  DOCUMENT_GENERATE = 'document:generate',
  REGULATION_VIEW = 'regulation:view',
  ADMIN_USERS = 'admin:users',
  ADMIN_REGULATIONS = 'admin:regulations'
}

// RBAC (Role-Based Access Control)
const rolePermissions: Record<string, Permission[]> = {
  USER: [
    Permission.ANIMAL_READ,
    Permission.ANIMAL_WRITE,
    Permission.BREEDING_READ,
    Permission.BREEDING_WRITE,
    Permission.DOCUMENT_GENERATE,
    Permission.REGULATION_VIEW
  ],
  PROFESSIONAL: [
    // Toutes les permissions USER +
    Permission.ANIMAL_DELETE,
    // Accès API étendu
  ],
  ADMIN: [
    // Toutes les permissions
    ...Object.values(Permission)
  ]
};

// Middleware d'autorisation
const requirePermission = (permission: Permission) => {
  return async (req: Request, res: Response, next: NextFunction) => {
    const user = req.user; // Attaché par auth middleware
    
    if (!user.permissions.includes(permission)) {
      return res.status(403).json({ error: 'Forbidden' });
    }
    
    next();
  };
};

// Exemple d'utilisation
router.delete('/animals/:id', 
  authenticate,
  requirePermission(Permission.ANIMAL_DELETE),
  deleteAnimalController
);
```

---

## 9. QUALITÉ & TESTS

### 9.1 Stratégie de Tests

#### Pyramide des Tests

```
        /\
       /  \  E2E Tests (10%)
      /____\
     /      \  Integration Tests (30%)
    /________\
   /          \  Unit Tests (60%)
  /__________/\
```

#### Tests Unitaires

```typescript
// Example: Regulation Engine Unit Test
describe('ComplianceOrchestrator', () => {
  let orchestrator: ComplianceOrchestrator;
  let mockRegulationRepo: jest.Mocked<RegulationRepository>;
  
  beforeEach(() => {
    mockRegulationRepo = createMockRegulationRepository();
    orchestrator = new ComplianceOrchestrator(mockRegulationRepo);
  });
  
  describe('checkCompliance', () => {
    it('should pass for domestic species with no documents', async () => {
      const animal = createTestAnimal({
        espece: { nomScientifique: 'Python regius' }
      });
      
      const regulation = createTestRegulation({
        france: { domestic: true }
      });
      
      mockRegulationRepo.findBySpecies.mockResolvedValue(regulation);
      
      const result = await orchestrator.checkCompliance(animal, collection);
      
      expect(result.compliant).toBe(true);
      expect(result.status).toBe('CONFORME');
    });
    
    it('should fail for Annex A species without CIC', async () => {
      const animal = createTestAnimal({
        espece: { nomScientifique: 'Testudo hermanni' },
        documents: []
      });
      
      const regulation = createTestRegulation({
        eu: { annex: 'A', intraCommunityPermitRequired: true }
      });
      
      mockRegulationRepo.findBySpecies.mockResolvedValue(regulation);
      
      const result = await orchestrator.checkCompliance(animal, collection);
      
      expect(result.compliant).toBe(false);
      expect(result.status).toBe('NON_CONFORME');
      expect(result.alerts).toContainEqual(
        expect.objectContaining({
          niveau: 'CRITIQUE',
          type: 'DOCUMENT'
        })
      );
    });
    
    it('should alert when threshold is exceeded', async () => {
      const collection = createTestCollection({
        animaux: Array(6).fill(null).map(() => 
          createTestAnimal({ espece: { nomScientifique: 'Morelia spilota' } })
        )
      });
      
      const regulation = createTestRegulation({
        france: {
          domestic: false,
          column: 'b',
          thresholds: { a: 1, b: 4, c: 10 }
        }
      });
      
      mockRegulationRepo.findBySpecies.mockResolvedValue(regulation);
      
      const result = await orchestrator.checkCompliance(
        collection.animaux[0], 
        collection
      );
      
      expect(result.status).toBe('ATTENTION');
      expect(result.actions).toContainEqual(
        expect.objectContaining({
          type: 'DECLARER'
        })
      );
    });
  });
});
```

#### Tests d'Intégration

```typescript
// Example: End-to-end compliance check
describe('Compliance Integration', () => {
  let app: Express;
  let db: Database;
  
  beforeAll(async () => {
    db = await setupTestDatabase();
    app = await createTestApp(db);
  });
  
  afterAll(async () => {
    await db.close();
  });
  
  it('should trigger compliance check on animal creation', async () => {
    const response = await request(app)
      .post('/api/v1/animals')
      .set('Authorization', `Bearer ${testToken}`)
      .send({
        espece: { nomScientifique: 'Testudo hermanni' },
        dateAcquisition: '2024-01-15',
        provenance: { type: 'ACHAT', vendeur: 'Test Breeder' }
      });
    
    expect(response.status).toBe(201);
    
    const animalId = response.body.id;
    
    // Attendre que l'event soit traité
    await wait(1000);
    
    // Vérifier que la conformité a été calculée
    const complianceResponse = await request(app)
      .get(`/api/v1/regulations/animals/${animalId}/status`)
      .set('Authorization', `Bearer ${testToken}`);
    
    expect(complianceResponse.status).toBe(200);
    expect(complianceResponse.body).toMatchObject({
      compliant: false,
      status: 'NON_CONFORME',
      alerts: expect.arrayContaining([
        expect.objectContaining({
          type: 'DOCUMENT',
          niveau: 'CRITIQUE'
        })
      ])
    });
  });
});
```

#### Tests E2E (Playwright)

```typescript
// Example: User journey
test('Amateur user can add animal and see compliance alerts', async ({ page }) => {
  // Login
  await page.goto('/login');
  await page.fill('[name="email"]', 'test@example.com');
  await page.fill('[name="password"]', 'password123');
  await page.click('button[type="submit"]');
  
  // Navigate to animals
  await page.click('text=Mes animaux');
  await expect(page).toHaveURL('/animals');
  
  // Add new animal
  await page.click('text=Ajouter un animal');
  await page.selectOption('[name="species"]', 'Testudo hermanni');
  await page.fill('[name="dateAcquisition"]', '2024-01-15');
  await page.click('button:has-text("Enregistrer")');
  
  // Should see compliance alert
  await expect(page.locator('.alert-critique')).toBeVisible();
  await expect(page.locator('text=CIC obligatoire')).toBeVisible();
  
  // Click on action
  await page.click('text=Voir les actions requises');
  await expect(page).toHaveURL(/\/actions/);
  await expect(page.locator('text=Obtenir un Certificat Intra-Communautaire')).toBeVisible();
});
```

### 9.2 CI/CD Pipeline

```yaml
# .github/workflows/ci.yml
name: CI

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    
    services:
      postgres:
        image: postgres:15
        env:
          POSTGRES_PASSWORD: test
        options: >-
          --health-cmd pg_isready
          --health-interval 10s
      
      rabbitmq:
        image: rabbitmq:3-management
        ports:
          - 5672:5672
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Node.js
        uses: actions/setup-node@v3
        with:
          node-version: '20'
      
      - name: Install dependencies
        run: npm ci
      
      - name: Lint
        run: npm run lint
      
      - name: Type check
        run: npm run type-check
      
      - name: Unit tests
        run: npm run test:unit
      
      - name: Integration tests
        run: npm run test:integration
      
      - name: E2E tests
        run: npm run test:e2e
      
      - name: Coverage
        run: npm run coverage
      
      - name: Upload coverage
        uses: codecov/codecov-action@v3

  security:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Run Snyk
        uses: snyk/actions/node@master
        env:
          SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}
      
      - name: Run Trivy
        uses: aquasecurity/trivy-action@master
        with:
          scan-type: 'fs'
          scan-ref: '.'
  
  build:
    needs: [test, security]
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Build Docker images
        run: docker-compose build
      
      - name: Push to registry
        if: github.ref == 'refs/heads/main'
        run: |
          echo ${{ secrets.DOCKER_PASSWORD }} | docker login -u ${{ secrets.DOCKER_USERNAME }} --password-stdin
          docker-compose push
```

---

## 10. ROADMAP & ÉVOLUTIVITÉ

### 10.1 Phase 1 - MVP (3-4 mois)

**Objectif** : Version fonctionnelle minimum avec conformité réglementaire

#### Sprint 1-2 : Infrastructure & Core (4 semaines)
- [ ] Setup architecture modulaire
- [ ] Base de données PostgreSQL
- [ ] API Gateway
- [ ] Auth service (JWT)
- [ ] Animal service (CRUD basique)
- [ ] Regulation service (base de données espèces)

#### Sprint 3-4 : Conformité & Documents (4 semaines)
- [ ] Moteur de conformité
- [ ] Système d'alertes
- [ ] Document service (registre, attestation)
- [ ] Frontend web (React) : dashboard, liste animaux
- [ ] Tests unitaires & intégration

#### Sprint 5-6 : Élevage & Finalisation MVP (4 semaines)
- [ ] Breeding service (cycles basiques)
- [ ] Généalogie
- [ ] Export PDF registres
- [ ] Tests E2E
- [ ] Documentation utilisateur
- [ ] Déploiement production

### 10.2 Phase 2 - Enrichissement (3-4 mois)

#### Features Additionnelles
- [ ] **Module Santé** : vétérinaires, traitements, vaccinations
- [ ] **Gestion Alimentaire** : planning, proies, nutrition
- [ ] **Photos & Galerie** : upload multi-photos, reconnaissance morphs
- [ ] **Statistiques Avancées** : graphiques, insights
- [ ] **Mobile App** : React Native (iOS/Android)
- [ ] **Notifications Push** : alertes temps réel
- [ ] **Multi-utilisateurs** : collections partagées, équipes

### 10.3 Phase 3 - Climatique & IoT (4-6 mois)

#### Module Gestion Climatique
- [ ] **Terrariums** : configuration dimensions, équipements
- [ ] **Paramètres** : température, humidité, éclairage
- [ ] **Monitoring** : historiques, graphiques
- [ ] **Alertes** : dépassements seuils
- [ ] **Intégration IoT** :
  - Capteurs température/humidité (WiFi, Zigbee)
  - Thermostats connectés
  - Systèmes d'éclairage programmables
  - API intégration (Home Assistant, MQTT)

### 10.4 Phase 4 - Communauté & Marketplace (6+ mois)

#### Réseau Social
- [ ] Profils publics éleveurs
- [ ] Feed communautaire
- [ ] Partage photos, réussites reproduction
- [ ] Groupes par espèce, région

#### Marketplace
- [ ] Annonces vente animaux
- [ ] Vérification conformité vendeurs
- [ ] Génération automatique documents légaux
- [ ] Système de notation/avis
- [ ] Paiement sécurisé (Stripe)
- [ ] Expédition spécialisée (partenaires transporteurs)

#### Pro Features
- [ ] **Comptabilité** : facturation, TVA, déclarations
- [ ] **Stock Management** : matériel, nourriture
- [ ] **CRM** : clients, suivi ventes
- [ ] **Multi-sites** : gestion plusieurs établissements
- [ ] **Employés** : gestion permissions, tâches

### 10.5 Évolutivité Technique

#### Scalabilité Horizontale
```yaml
# Kubernetes Deployment Example
apiVersion: apps/v1
kind: Deployment
metadata:
  name: regulation-service
spec:
  replicas: 3  # Auto-scaling basé sur CPU/mémoire
  selector:
    matchLabels:
      app: regulation-service
  template:
    metadata:
      labels:
        app: regulation-service
    spec:
      containers:
      - name: regulation-service
        image: reptile-manager/regulation-service:latest
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
        env:
          - name: DB_HOST
            valueFrom:
              secretKeyRef:
                name: db-credentials
                key: host
```

#### Cache Strategy
```typescript
// Redis Cache Layer
class CachedRegulationService {
  private cache: Redis;
  private ttl = 3600; // 1 heure
  
  async getSpeciesRegulation(scientificName: string): Promise<SpeciesRegulation> {
    const cacheKey = `species:${scientificName}`;
    
    // Try cache first
    const cached = await this.cache.get(cacheKey);
    if (cached) {
      return JSON.parse(cached);
    }
    
    // Cache miss: fetch from DB
    const regulation = await this.db.findSpecies(scientificName);
    
    // Store in cache
    await this.cache.setex(cacheKey, this.ttl, JSON.stringify(regulation));
    
    return regulation;
  }
  
  async invalidateCache(scientificName: string) {
    await this.cache.del(`species:${scientificName}`);
  }
}
```

#### Monitoring & Observability
```typescript
// Prometheus Metrics
import { register, Counter, Histogram } from 'prom-client';

// Business metrics
const complianceChecksCounter = new Counter({
  name: 'compliance_checks_total',
  help: 'Total compliance checks performed',
  labelNames: ['status', 'species']
});

const complianceCheckDuration = new Histogram({
  name: 'compliance_check_duration_seconds',
  help: 'Duration of compliance checks',
  buckets: [0.1, 0.5, 1, 2, 5]
});

// Application dans le code
async function checkCompliance(animal: Animal): Promise<ComplianceResult> {
  const end = complianceCheckDuration.startTimer();
  
  const result = await orchestrator.checkCompliance(animal, collection);
  
  end(); // Record duration
  complianceChecksCounter.inc({
    status: result.status,
    species: animal.espece.nomScientifique
  });
  
  return result;
}
```

---

## CONCLUSION

Ce document de spécification définit une architecture **modulaire, évolutive et conforme** pour un gestionnaire d'élevage de reptiles.

### Points Clés à Retenir

1. **Architecture NON-Monolithique** : Services découplés, communication via events
2. **Conformité au Cœur** : Module réglementaire central avec moteur de règles
3. **Traçabilité Totale** : Registres, documents, audit trail
4. **Évolutivité** : Phases progressives, infrastructure scalable
5. **Sécurité & RGPD** : Chiffrement, consentements, droits utilisateurs

### Prochaines Étapes

1. **Validation Stakeholders** : Éleveurs, vétérinaires, autorités
2. **Choix Stack Technique Finale** : Langages, frameworks, cloud provider
3. **Kickoff Développement** : Sprint planning Phase 1
4. **Tests Alpha** : Groupe d'éleveurs beta-testeurs
5. **Launch MVP** : Mise en production version 1.0

### Références Légales Principales

- Arrêté du 8 octobre 2018 (conditions détention)
- Règlement CE 338/97 (CITES EU)
- Convention CITES (Washington)
- Règlement UE 1143/2014 (espèces envahissantes)
- Code de l'Environnement (L.411-1 à L.413-6)
- RGPD (Règlement UE 2016/679)

---

**Document Version** : 1.0  
**Date** : 2025-01-28  
**Auteur** : Claude (Anthropic)  
**Basé sur** : Skill god-mode-dev-herp

---

## ANNEXES

### Annexe A : Glossaire

- **CDC** : Certificat de Capacité
- **AOE** : Autorisation d'Ouverture d'Établissement
- **CIC** : Certificat Intra-Communautaire
- **CITES** : Convention on International Trade in Endangered Species
- **DDPP** : Direction Départementale de la Protection des Populations
- **DREAL** : Direction Régionale de l'Environnement, de l'Aménagement et du Logement
- **DDD** : Domain-Driven Design
- **CQRS** : Command Query Responsibility Segregation
- **RGPD** : Règlement Général sur la Protection des Données

### Annexe B : Contacts Utiles

- **DDPP** : Direction départementale (variable selon département)
- **DREAL** : Direction régionale pour CITES/CIC
- **OFB** : Office Français de la Biodiversité
- **SNPN** : Société Nationale de Protection de la Nature
- **FFC** : Fédération Française de Captivité

### Annexe C : Ressources Techniques

- **Documentation CITES** : https://cites.org/
- **Légifrance** : https://www.legifrance.gouv.fr/
- **Base espèces** : Reptile Database (http://www.reptile-database.org/)
- **GitHub Reptile Manager** : [À créer]
- **Documentation API** : [À créer]

---

**FIN DU DOCUMENT**
