# Game Design & Développement de Jeux

## 1. Fondamentaux du Game Design

### Définitions
- **Game Design** : conception des règles, mécaniques, expérience
- **Level Design** : conception des niveaux, espaces de jeu
- **Narrative Design** : intégration histoire/gameplay
- **UX Design** : expérience utilisateur, interfaces

### MDA Framework
- **Mechanics** : règles, systèmes
- **Dynamics** : comportements émergeants
- **Aesthetics** : expérience émotionnelle du joueur

### Types d'expériences (Aesthetics)
- Sensation, Fantasy, Narrative
- Challenge, Fellowship, Discovery
- Expression, Submission

## 2. Mécaniques de Jeu

### Core loops
- Boucle principale : action → récompense → progression
- Session loop : début → pic → fin satisfaisante
- Retention loop : raison de revenir

### Systèmes de progression
- Niveaux, XP, arbres de compétences
- Loot, équipement, crafting
- Déblocage de contenu
- Prestige, New Game+

### Économie de jeu
- Monnaies (soft/hard currency)
- Sinks et sources
- Inflation, équilibrage
- F2P : monétisation éthique

### Systèmes de combat
- Tour par tour vs temps réel
- Action, tactical, hack'n'slash
- Hitboxes, hurtboxes, frames
- Damage types, résistances

### Systèmes émergents
- Physique, destruction
- IA réactive
- Systèmes météo, jour/nuit
- Écosystèmes simulés

## 3. Level Design

### Principes fondamentaux
- **Flow** : guidance naturelle du joueur
- **Pacing** : alternance tension/repos
- **Landmarks** : points de repère
- **Gating** : progression contrôlée

### Techniques
- Weenies : éléments visuels attractifs (châteaux Disney)
- Breadcrumbing : indices progressifs
- Chokepoints : passages obligés
- Safe zones : zones de repos

### Types de niveaux
- Linéaire : couloir, scripted
- Hub : zone centrale, branches
- Open world : exploration libre
- Procedural : généré algorithmiquement

### Metrics
- Dimensions joueur : hauteur, saut, vitesse
- Kill zones, cover, lignes de vue
- Temps de traversée

## 4. Narrative Design

### Structures narratives
- Trois actes : setup, confrontation, résolution
- Hero's journey (Campbell)
- Non-linéaire : branches, fins multiples
- Environmental storytelling

### Intégration narration/gameplay
- Cutscenes vs in-game
- Dialogues : arbres, systèmes de choix
- Lore : collectibles, journaux, audio logs
- Show don't tell

### Personnages
- Motivation, arc narratif
- Voix, personnalité distinctive
- Relations, factions

## 5. Genres de Jeux

### Action
- **Platformer** : précision, timing
- **Beat'em up / Hack'n'slash** : combat, combos
- **FPS** : tir première personne
- **TPS** : tir troisième personne
- **Battle Royale** : last man standing

### Aventure
- **Point & Click** : puzzles, narration
- **Action-aventure** : exploration, combat
- **Metroidvania** : exploration, abilities gating
- **Survival horror** : ressources limitées, peur

### RPG
- **JRPG** : tour par tour, linéaire, histoire
- **WRPG** : temps réel, choix, open world
- **Action-RPG** : combat temps réel, progression
- **Roguelike/lite** : permadeath, runs

### Stratégie
- **RTS** : temps réel, base building
- **TBS** : tour par tour, stratégie
- **4X** : explore, expand, exploit, exterminate
- **Tower Defense** : vagues, placement

### Simulation
- **Life sim** : gestion vie quotidienne
- **City builder** : urbanisme, gestion
- **Management** : business, équipes
- **Vehicle sim** : conduite réaliste

### Autres
- **Puzzle** : logique, réflexion
- **Rhythm** : musique, timing
- **Sports** : simulation sportive
- **Fighting** : versus, combos

## 6. Moteurs de Jeu

### Unity
- C#, multi-plateforme
- Asset Store riche
- 2D et 3D
- Mobile, VR, consoles, PC
- Gratuit jusqu'à seuil de revenus

### Unreal Engine
- C++, Blueprints (visual scripting)
- Graphismes AAA
- Nanite, Lumen (UE5)
- MetaHumans
- Royalties après seuil

### Godot
- GDScript, C#, C++
- Open source, gratuit
- Léger, flexible
- 2D excellent, 3D en amélioration

### Autres moteurs
- **GameMaker** : 2D, débutants
- **RPG Maker** : JRPG spécifiquement
- **Construct** : no-code, web
- **CryEngine, Lumberyard** : graphismes
- **Custom engines** : AAA studios

## 7. Programmation de Jeux

### Architecture
- Entity-Component-System (ECS)
- Game loop : input → update → render
- State machines : gameplay states
- Event systems : découplage

### Patterns courants
- Singleton : managers globaux
- Object pooling : performance
- Observer : événements
- Command : input, undo
- Factory : création objets

### Systèmes essentiels
- Input handling : abstraction
- Physics : collision, rigidbody
- Animation : state machines, blend trees
- Audio : spatial, mixing
- UI : menus, HUD

### IA de jeu
- Pathfinding : A*, NavMesh
- Behavior trees
- State machines
- GOAP (Goal-Oriented Action Planning)
- Utility AI

### Networking
- Client-server, P2P
- Autorité : qui décide
- Lag compensation, prediction
- Netcode : synchronisation état

## 8. Art & Audio

### Direction artistique
- Style : réaliste, stylisé, pixel art
- Cohérence visuelle
- Lisibilité, hiérarchie visuelle

### Assets 2D
- Sprites, spritesheets
- Animation : frame by frame, skeletal
- Tilemaps, parallax

### Assets 3D
- Modélisation : high/low poly
- Texturing : PBR, hand-painted
- Rigging, skinning, animation
- LOD, optimization

### Audio
- SFX : effets sonores
- Musique : adaptive, interactive
- Voix : dialogues, cris
- Mix : ambiance, priorités

## 9. Production & Business

### Méthodologies
- Agile/Scrum adapté au jeu
- Milestones : prototype, alpha, beta, gold
- Playtesting itératif

### Documentation
- GDD (Game Design Document)
- TDD (Technical Design Document)
- Art bible, style guide

### Publication
- Plateformes : Steam, consoles, mobile
- Marketing : wishlists, trailers
- Community management
- Reviews, updates post-launch

### Modèles économiques
- Premium (achat unique)
- F2P + IAP (in-app purchases)
- Subscription
- DLC, Season pass

## 10. Simulation & Réalisme

### Simulation d'écosystèmes
- Chaînes alimentaires
- Reproduction, populations
- Comportements émergents
- Équilibre dynamique

### Simulation physique
- Rigid body, soft body
- Fluides, particules
- Destruction, déformation

### Simulation de reptiles (spécifique)
- Comportements réalistes : thermorégulation, chasse
- Cycles de vie : mue, reproduction, croissance
- Paramètres environnementaux : température, humidité
- Génétique : morphs, hérédité
- Besoins : faim, soif, stress