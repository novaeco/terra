# Génétique des Reptiles - Phases & Morphs

## 1. Fondamentaux Génétiques

### Terminologie
- **Wild-type (WT)** : phénotype naturel, "normal"
- **Morph/Phase** : variation phénotypique distincte
- **Allèle** : variante d'un gène
- **Homozygote** : deux allèles identiques (AA ou aa)
- **Hétérozygote (het)** : deux allèles différents (Aa)
- **Phénotype** : apparence observable
- **Génotype** : constitution génétique

### Modes de transmission

#### Récessif simple
- Nécessite 2 copies (homozygote) pour expression
- Hétérozygote (het) = porteur non visible
- Croisements :
  - WT × WT = 100% WT
  - Morph × WT = 100% het Morph (phénotype WT)
  - Het × Het = 25% Morph, 50% het, 25% WT
  - Morph × Het = 50% Morph, 50% het
  - Morph × Morph = 100% Morph

#### Dominant simple
- Une seule copie suffit pour expression
- Homozygote (super) parfois différent ou létal
- Croisements :
  - Morph × WT = 50% Morph, 50% WT
  - Morph × Morph = 25% Super, 50% Morph, 25% WT

#### Co-dominant / Incomplet
- Hétérozygote a phénotype intermédiaire distinct
- Homozygote (super form) = phénotype amplifié
- Exemple : Pastel (het) vs Super Pastel (homozygote)

#### Lié au sexe
- Serpents : système ZW (femelles ZW, mâles ZZ)
- Mâles ne peuvent être "het" pour traits liés Z
- Femelles expriment avec une seule copie

### Polygénique & sélection
- **Traits polygéniques** : multiples gènes, sélection ligne
- Exemples : taille, intensité couleur, pattern clean
- Sélection sur générations (line breeding)

## 2. Python regius (Ball Python) - Morphs Majeurs

### Récessifs
| Morph | Description | Notes |
|-------|-------------|-------|
| Albino (Amelanistic) | Absence mélanine, yeux rouges | Plusieurs lignées (différentes compatibilités) |
| Axanthic | Réduction xanthophores, noir/blanc/gris | Plusieurs lignées non-compatibles |
| Clown | Pattern altéré, tête claire distinctive | Tête "alien" caractéristique |
| Piebald | Zones blanches non pigmentées | % blanc variable |
| Ghost (Hypo) | Réduction mélanine, couleurs lavées | Plusieurs lignées |
| Genetic Stripe | Rayure dorsale continue | Pattern réduit latéralement |
| Desert Ghost | Pattern réduit, couleurs claires | Distinct de Ghost |
| Ultramel | Yeux rouges, coloration unique | Allèle différent d'Albino |
| Puzzle | Pattern fragmenté unique | Relativement récent |

### Co-dominants/Dominants
| Morph | Hétéro | Super | Notes |
|-------|--------|-------|-------|
| Pastel | Jaune intensifié, pattern propre | Super Pastel (très jaune) | Fondamental combos |
| Spider | Pattern aberrant, tête claire | Super Spider = létal | Wobble neurologique |
| Pinstripe | Rayures fines, pattern réduit | Super Pin = très réduit | Combos populaires |
| Mojave | Motif altéré, flammes | Blue-Eyed Leucistic (BEL) | Avec Lesser/Butter |
| Lesser/Butter | Tons jaune-brun clairs | BEL (blanc yeux bleus) | Complexe BEL |
| Fire | Couleurs intensifiées | Black-Eyed Leucistic | Avec Fire |
| Cinnamon/Black Pastel | Brun foncé, oxidé | Super = 8-ball (problématique) | Attention kinks |
| Enchi | Bandes latérales orangées | Super Enchi | Excellent modificateur |
| Yellow Belly | Subtil, ventre jaune propre | Ivory | Populaire combos |
| Spotnose | Tête tachetée, pattern unique | Super Spotnose | Trait facial distinctif |
| Banana/Coral Glow | Lavande + jaune, taches noires | Super Banana | Sex-linked effects |

### Combinaisons populaires
- **Killer Bee** = Pastel + Spider
- **Bumble Bee** = Pastel + Spider (autre usage)
- **Firefly** = Fire + Pastel
- **Pewter** = Cinnamon + Pastel
- **Killer Clown** = Clown + Spider + Pastel
- **Pied combos** : Piebald + presque tout

## 3. Boa constrictor - Morphs

### Récessifs
| Morph | Description |
|-------|-------------|
| Albino (Kahl, Sharp, etc.) | Plusieurs lignées non-compatibles |
| Anery (Anerythristic) | Absence rouge/orange, gris/noir |
| Blood | Coloration rouge intensifiée |
| Leopard | Pattern tacheté, queue aberrante |
| IMG (Increasing Melanin Gene) | Darkening progressif avec âge |

### Co-dominants
| Morph | Super |
|-------|-------|
| Hypo | Super Hypo (salmon) |
| Jungle | Super Jungle (pattern fragmenté) |
| Motley | Super Motley (problèmes fertilité) |
| Aztec | Super Aztec |

### Combinaisons
- **Sunglow** = Albino + Hypo
- **Ghost** = Anery + Hypo
- **Moonglow** = Anery + Albino + Hypo
- **Snow** = Albino + Anery

## 4. Pantherophis guttatus (Corn Snake)

### Récessifs de base
| Morph | Effet |
|-------|-------|
| Amelanistic (Amel) | Pas de mélanine, rouge/orange/jaune |
| Anerythristic (Anery) | Pas de rouge, gris/noir |
| Hypomelanistic (Hypo) | Mélanine réduite |
| Caramel | Pigments altérés, tons caramel |
| Charcoal | Réduction jaune/rouge |
| Diffused (Bloodred) | Pattern ventral absent, couleur envahissante |
| Lavender | Tons lavande/rose |
| Motley | Pattern dorsale en selle fusionné |
| Stripe | Rayures longitudinales |
| Sunkissed | Hypo-like, lignée distincte |

### Combinaisons classiques
- **Snow** = Amel + Anery (blanc + rose)
- **Ghost** = Anery + Hypo
- **Butter** = Amel + Caramel
- **Pewter** = Charcoal + Diffused
- **Opal** = Amel + Lavender
- **Blizzard** = Amel + Charcoal
- **Palmetto** = Leucistique avec écailles colorées aléatoires

## 5. Pogona vitticeps (Bearded Dragon)

### Traits majeurs
| Morph | Type | Description |
|-------|------|-------------|
| Hypo | Récessif | Ongles clairs, couleurs réduites |
| Trans (Translucent) | Récessif | Écailles translucides, yeux noirs solides |
| Het Trans | Porteur | Yeux parfois légèrement différents |
| Leatherback | Co-dom | Écailles dorsales réduites |
| Silkback | Super Leather | Pas d'écailles (soins spéciaux requis) |
| Dunner | Dominant | Écailles direction aléatoire |
| Witblits | Récessif | Pattern absent, couleur unie claire |
| Zero | Récessif | Pattern/couleur absents (blanc-gris) |

### Combinaisons
- **Hypo Trans** : Translucide + couleurs réduites
- **Witblits + Hypo** : Très clair uni
- **Zero Trans** : Rare, très striking

## 6. Eublepharis macularius (Leopard Gecko)

### Morphs communs
| Morph | Type | Description |
|-------|------|-------------|
| High Yellow | Sélection | Jaune intensifié |
| Tangerine | Sélection/polygénique | Orange intense |
| SHTCTB | Sélection | Super Hypo Tangerine Carrot Tail Baldy |
| Mack Snow | Co-dom | Réduction jaune, banding blanc |
| Super Snow | Super | Très blanc, yeux eclipse possibles |
| Tremper Albino | Récessif | Lignée albino Tremper |
| Bell Albino | Récessif | Lignée Bell (non compatible Tremper) |
| Rainwater Albino | Récessif | Lignée Rainwater (non compatible) |
| Eclipse | Récessif | Yeux solides (rouge ou noir) |
| Blizzard | Récessif | Pattern absent, couleur variable |
| Murphy Patternless | Récessif | Sans pattern, leucistique |
| Enigma | Dominant | Pattern unique, problèmes neuro (ES) |
| RAPTOR | Combo | Red eye Albino Patternless Tremper ORange |
| Diablo Blanco | Combo | Blizzard + Tremper Albino + Eclipse |
| Black Night | Polygénique | Mélanistique développé |

## 7. Outils de Calcul

### Calculateurs génétiques
- Carrés de Punnett pour traits simples
- Logiciels : MorphMarket genetics calculator
- Probabilités pour multi-traits : multiplication probabilités indépendantes

### Notation standard
- Visual morph (majuscule ou nom complet)
- het morph (minuscule "het")
- % het ou poss het (possible het non prouvé)
- Exemple : "Pastel het Clown het Pied" ou "Super Pastel 66% het Clown"

### Proving out hets
- Croiser avec morph visuel : 50% visuels si vraiment het
- Statistiques : plusieurs clutches pour confirmer
- 7+ essais sans visuel = probablement pas het (>99% confiance)