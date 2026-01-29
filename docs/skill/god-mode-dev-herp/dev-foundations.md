# Fondations Théoriques & Algorithmique

## 1. Mathématiques & Logique

### Logique formelle
- Logique propositionnelle : tables de vérité, CNF/DNF, résolution
- Prédicats : quantificateurs, unification, inférence
- Preuves : induction, contradiction, construction

### Théorie des ensembles & graphes
- Relations : réflexivité, symétrie, transitivité, équivalence, ordre
- Graphes : dirigés/non-dirigés, pondérés, bipartis, DAG
- Propriétés : connexité, cycles, arbres, planarité

### Probabilités & statistiques
- Distributions : normale, Poisson, exponentielle, binomiale
- Bayes, espérance, variance, covariance
- Tests : A/B, chi², t-test, intervalles de confiance
- Applications : observabilité, fiabilité, ML

### Algèbre linéaire
- Vecteurs, matrices, transformations
- Décompositions : LU, QR, SVD, eigenvalues
- Applications : ML, 3D, vision, compression

### Analyse numérique
- Erreurs flottantes : IEEE 754, ULP, catastrophic cancellation
- Stabilité numérique, conditionnement
- Méthodes itératives, convergence

## 2. Informatique Théorique

### Complexité
- Big-O, Ω, Θ : worst/average/best case
- Analyse amortie : aggregate, accounting, potential
- Cache complexity, I/O complexity
- Classes : P, NP, NP-complet, NP-hard, PSPACE

### Théorie des langages
- Hiérarchie Chomsky : régulier, context-free, context-sensitive
- Automates : DFA, NFA, PDA, Turing
- Grammaires : LL, LR, LALR, GLR
- Parsing : recursive descent, Earley, CYK

### Calculabilité
- Décidabilité, semi-décidabilité
- Réductions : many-one, Turing
- Problèmes indécidables : halting, Rice
- Limites fondamentales

## 3. Structures de Données

### Linéaires
- Arrays : dynamiques, circulaires, gap buffer
- Listes : singly/doubly linked, XOR linked
- Piles, files, deques : implémentations array/linked

### Hash tables
- Open addressing : linear/quadratic probing, double hashing
- Chaining : linked lists, BST
- Robin Hood hashing, cuckoo hashing
- Perfect hashing, minimal perfect hashing
- Bloom filters, cuckoo filters, count-min sketch

### Arbres
- BST : recherche, insertion, suppression O(h)
- Équilibrés : AVL (rotations), Red-Black (coloration)
- B-Tree/B+Tree : disque, bases de données, index
- Trie, Radix tree, Patricia : préfixes, dictionnaires
- Segment tree : range queries, lazy propagation
- Fenwick tree (BIT) : prefix sums, point updates
- Sparse table : RMQ statique O(1)

### Heaps
- Binary heap : array-based, heapify O(n)
- Binomial heap : merge O(log n)
- Fibonacci heap : decrease-key O(1) amorti
- Pairing heap : pratique, bounds conjecturés

### Graphes
- Adjacency list/matrix : trade-offs espace/temps
- CSR/CSC : sparse, cache-friendly
- Edge list : Kruskal, external memory

### Structures avancées
- Skip list : probabiliste, concurrent-friendly
- Union-Find/DSU : union by rank, path compression
- LRU/LFU caches, ARC (adaptive)
- Persistent DS : path copying, fat nodes, HAMT
- Lock-free : queues, stacks, Michael-Scott queue
- RCU : read-copy-update, epoch-based reclamation

## 4. Algorithmes

### Tri & sélection
- Comparaison : quicksort, mergesort, heapsort
- Non-comparaison : counting, radix, bucket
- Hybrides : introsort, timsort, pdqsort
- Sélection : quickselect, median-of-medians

### Recherche
- Binaire : variants (lower/upper bound)
- Interpolation : données uniformes
- Exponentielle : unbounded search

### Programmation dynamique
- Top-down (mémo) vs bottom-up (tabulation)
- Optimisations : space, Knuth, divide-and-conquer DP
- Classiques : knapsack, LCS, edit distance, matrix chain

### Graphes
- Parcours : BFS, DFS, applications
- Topological sort : Kahn, DFS-based
- SCC : Kosaraju, Tarjan
- Shortest path :
  - Dijkstra : O((V+E) log V), non-négatif
  - Bellman-Ford : O(VE), négatif, détection cycles
  - Floyd-Warshall : O(V³), all-pairs
  - A* : heuristique admissible
- MST : Kruskal (Union-Find), Prim (heap)
- Flots :
  - Ford-Fulkerson, Edmonds-Karp : O(VE²)
  - Dinic : O(V²E)
  - Push-relabel : O(V³)
  - Min-cut, max bipartite matching
  - Hopcroft-Karp : O(E√V)

### Chaînes de caractères
- Pattern matching : KMP, Z-algorithm, Rabin-Karp
- Suffix array : construction O(n log n), LCP
- Suffix tree : Ukkonen O(n)
- Aho-Corasick : multi-pattern

### Géométrie computationnelle
- Convex hull : Graham, Andrew, Jarvis
- Line intersection : sweep line
- Closest pair : divide-and-conquer
- Spatial : KD-tree, R-tree, BVH
- Range queries, nearest neighbor

### Algorithmes probabilistes
- Monte Carlo vs Las Vegas
- Randomized quicksort, selection
- Hashing universel, tabulation hashing
- Bloom filters, HyperLogLog
- Reservoir sampling, random permutation

### Approximation & heuristiques
- Greedy : set cover, vertex cover
- Local search : 2-opt, simulated annealing
- Metaheuristics : genetic, ant colony
- PTAS, FPTAS concepts