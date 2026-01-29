# Mathématiques

## 1. Analyse

### Suites et séries
- Convergence, divergence
- Critères : comparaison, ratio, racine, intégrale
- Séries entières, rayon de convergence
- Taylor, Maclaurin

### Limites et continuité
- Définition ε-δ
- Continuité uniforme
- Théorèmes : valeurs intermédiaires, Weierstrass

### Dérivation
- Définition, règles (chaîne, produit, quotient)
- Dérivées partielles, gradient
- Théorèmes : Rolle, accroissements finis
- Développements limités

### Intégration
- Riemann, Lebesgue
- Techniques : substitution, parties, fractions partielles
- Intégrales impropres
- Théorème fondamental

### Équations différentielles
- Ordre 1 : séparation, linéaires, Bernoulli
- Ordre 2 : coefficients constants, variation constantes
- Systèmes, stabilité
- Laplace, séries

### Analyse complexe
- Fonctions holomorphes, Cauchy-Riemann
- Intégration complexe, résidus
- Séries de Laurent
- Applications conformes

### Analyse fonctionnelle
- Espaces de Banach, Hilbert
- Opérateurs linéaires
- Théorème de Hahn-Banach
- Analyse spectrale

## 2. Algèbre

### Algèbre linéaire
- Espaces vectoriels, bases, dimension
- Applications linéaires, matrices
- Déterminants, rang
- Systèmes linéaires : Gauss, Cramer

### Diagonalisation
- Valeurs propres, vecteurs propres
- Polynôme caractéristique
- Diagonalisation, trigonalisation
- Décomposition spectrale

### Formes bilinéaires et quadratiques
- Produit scalaire, norme
- Orthogonalité, Gram-Schmidt
- Matrices symétriques, hermitiennes
- Signature, inertie

### Algèbre abstraite
- Groupes : définition, sous-groupes, quotients
- Anneaux : idéaux, corps de fractions
- Corps : extensions, théorie de Galois
- Modules, algèbres

### Théorie des nombres
- Divisibilité, PGCD, PPCM
- Nombres premiers, factorisation unique
- Congruences, arithmétique modulaire
- Théorèmes : Fermat, Euler, Wilson, chinois

## 3. Géométrie

### Géométrie euclidienne
- Axiomes, théorèmes fondamentaux
- Triangles : Pythagore, Thalès, similitude
- Cercles : tangentes, sécantes, puissance
- Polygones, aires, volumes

### Géométrie analytique
- Coordonnées cartésiennes, polaires
- Droites, plans, distances
- Coniques : ellipse, parabole, hyperbole
- Quadriques

### Géométrie différentielle
- Courbes : paramétrisation, courbure, torsion
- Surfaces : première/seconde forme fondamentale
- Courbure gaussienne, moyenne
- Variétés différentiables

### Topologie
- Espaces topologiques, ouverts, fermés
- Continuité, homéomorphismes
- Compacité, connexité
- Homotopie, groupe fondamental

## 4. Probabilités & Statistiques

### Probabilités
- Espaces probabilisés, σ-algèbres
- Variables aléatoires, distributions
- Espérance, variance, moments
- Indépendance, conditionnement

### Distributions classiques
| Discrètes | Continues |
|-----------|-----------|
| Bernoulli | Uniforme |
| Binomiale | Normale (Gaussienne) |
| Poisson | Exponentielle |
| Géométrique | Gamma |
| Hypergéométrique | Beta |

### Théorèmes limites
- Loi des grands nombres (faible, forte)
- Théorème central limite
- Convergences : presque sûre, en probabilité, en loi

### Statistiques inférentielles
- Estimation : ponctuelle, intervalles
- Tests d'hypothèses : p-value, puissance
- Régression : linéaire, multiple
- ANOVA, chi-deux

### Statistiques bayésiennes
- Théorème de Bayes
- Prior, posterior, likelihood
- Inférence bayésienne
- MCMC

## 5. Mathématiques Discrètes

### Combinatoire
- Permutations, arrangements, combinaisons
- Principe inclusion-exclusion
- Fonctions génératrices
- Récurrences

### Théorie des graphes
- Graphes, digraphes, multigraphes
- Parcours : chemins, cycles, connexité
- Arbres, forêts
- Graphes bipartis, planaires
- Coloration, couplages, flots

### Logique
- Logique propositionnelle, prédicats
- Systèmes de preuves
- Complétude, décidabilité
- Théorèmes de Gödel

### Cryptographie mathématique
- RSA : factorisation
- Diffie-Hellman : logarithme discret
- Courbes elliptiques
- Lattices, post-quantique

## 6. Optimisation

### Optimisation continue
- Conditions nécessaires : gradient nul
- Conditions suffisantes : Hessien
- Contraintes : Lagrange, KKT

### Programmation linéaire
- Forme standard
- Simplexe
- Dualité

### Optimisation convexe
- Fonctions convexes
- Problèmes convexes
- Algorithmes : descente gradient, Newton

### Optimisation combinatoire
- Problèmes NP-difficiles
- Heuristiques, métaheuristiques
- Approximation

## 7. Analyse Numérique

### Résolution d'équations
- Bissection, Newton, sécante
- Point fixe, convergence

### Systèmes linéaires
- Méthodes directes : LU, Cholesky, QR
- Méthodes itératives : Jacobi, Gauss-Seidel, gradient conjugué

### Interpolation & approximation
- Lagrange, Newton, splines
- Moindres carrés
- Approximation trigonométrique, FFT

### Intégration numérique
- Rectangles, trapèzes, Simpson
- Quadrature de Gauss
- Monte-Carlo

### EDO numériques
- Euler explicite, implicite
- Runge-Kutta
- Méthodes multipas

### EDP numériques
- Différences finies
- Éléments finis
- Volumes finis

## 8. Transformées

### Fourier
- Séries de Fourier : f(x) = Σ(aₙcos(nx) + bₙsin(nx))
- Transformée de Fourier : F(ω) = ∫f(t)e^(-iωt)dt
- DFT, FFT
- Applications : signal, filtrage

### Laplace
- L{f(t)} = ∫₀^∞ f(t)e^(-st)dt
- Tables de transformées
- Résolution EDO, circuits

### Z (discret)
- Z{x[n]} = Σx[n]z^(-n)
- Signaux discrets
- Filtres numériques

### Ondelettes
- Analyse temps-fréquence
- Haar, Daubechies
- Compression, débruitage