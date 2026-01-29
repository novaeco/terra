# Data Science & Entrepreneuriat

## 1. Data Science

### Pipeline Data Science
1. **Définition problème** : objectif business, KPI
2. **Collecte données** : sources, APIs, scraping
3. **Nettoyage** : valeurs manquantes, outliers, formats
4. **Exploration (EDA)** : statistiques, visualisations
5. **Feature engineering** : création, sélection variables
6. **Modélisation** : choix algorithme, entraînement
7. **Évaluation** : métriques, validation croisée
8. **Déploiement** : mise en production
9. **Monitoring** : dérive, performance

### Exploration des données (EDA)
- Statistiques descriptives : moyenne, médiane, écart-type
- Distributions : histogrammes, boxplots
- Corrélations : heatmaps, scatter plots
- Valeurs manquantes, doublons
- Détection anomalies

### Nettoyage des données
- Valeurs manquantes : suppression, imputation (moyenne, médiane, KNN)
- Outliers : IQR, Z-score, isolation forest
- Encodage catégorielles : one-hot, label, target encoding
- Normalisation : MinMax, StandardScaler
- Transformation : log, Box-Cox

### Feature Engineering
- Création : ratios, agrégations, temporelles
- Extraction : TF-IDF (texte), embeddings
- Sélection : variance, corrélation, importance modèle
- Réduction : PCA, t-SNE, UMAP

## 2. Machine Learning

### Types d'apprentissage
- **Supervisé** : classification, régression (labels)
- **Non supervisé** : clustering, réduction dimensionnalité
- **Semi-supervisé** : peu de labels
- **Par renforcement** : agents, récompenses

### Algorithmes supervisés

#### Classification
| Algorithme | Forces | Faiblesses |
|------------|--------|------------|
| Logistic Regression | Simple, interprétable | Linéaire |
| Random Forest | Robuste, feature importance | Moins interprétable |
| XGBoost/LightGBM | Performance SOTA tabulaire | Hyperparamètres |
| SVM | Efficace haute dimension | Scalabilité |
| Neural Networks | Données complexes | Données massives nécessaires |

#### Régression
- Linear Regression, Ridge, Lasso
- Decision Trees, Random Forest
- Gradient Boosting (XGBoost, LightGBM, CatBoost)
- Neural Networks

### Algorithmes non supervisés
- **Clustering** : K-Means, DBSCAN, Hierarchical
- **Réduction** : PCA, t-SNE, UMAP
- **Association** : Apriori, FP-Growth

### Métriques

#### Classification
- Accuracy, Precision, Recall, F1-Score
- AUC-ROC, AUC-PR
- Matrice de confusion
- Log Loss

#### Régression
- MAE, MSE, RMSE
- R², R² ajusté
- MAPE

### Validation
- Train/Test split
- Cross-validation (K-fold, stratified)
- Holdout temporel (séries temporelles)
- Hyperparameter tuning : GridSearch, RandomSearch, Bayesian

### Problèmes courants
- **Overfitting** : régularisation, plus de données, simplifier
- **Underfitting** : modèle plus complexe, plus de features
- **Déséquilibre classes** : resampling, SMOTE, poids classes
- **Fuite de données** : attention preprocessing

## 3. Deep Learning

### Architectures
- **MLP** : couches denses, tabulaire
- **CNN** : convolutions, images
- **RNN/LSTM/GRU** : séquences, texte
- **Transformers** : attention, NLP state-of-the-art
- **Autoencoders** : compression, génération
- **GAN** : génération adversariale

### Frameworks
- **PyTorch** : recherche, flexible
- **TensorFlow/Keras** : production, écosystème
- **JAX** : performance, différentiation

### Entraînement
- Optimiseurs : SGD, Adam, AdamW
- Learning rate scheduling
- Batch size, epochs
- Early stopping, checkpoints
- GPU/TPU acceleration

### Transfer Learning
- Modèles pré-entraînés : ImageNet, BERT
- Fine-tuning : adapter à sa tâche
- Feature extraction : couches gelées

## 4. MLOps

### Versioning
- Données : DVC, Delta Lake
- Code : Git
- Modèles : MLflow, Weights & Biases

### Expérimentation
- Tracking : MLflow, W&B, Neptune
- Hyperparameter tuning : Optuna, Ray Tune
- Reproducibilité : seeds, environments

### Déploiement
- API : FastAPI, Flask
- Containers : Docker
- Orchestration : Kubernetes
- Serverless : AWS Lambda, Cloud Functions

### Monitoring
- Data drift : Evidently, Great Expectations
- Model performance : métriques en production
- Alerting : seuils, anomalies

## 5. Outils Data

### Python ecosystem
- **Pandas** : manipulation données
- **NumPy** : calcul numérique
- **Scikit-learn** : ML classique
- **Matplotlib/Seaborn** : visualisation
- **Plotly** : interactif

### Big Data
- **Spark** : traitement distribué
- **Dask** : parallélisme Python
- **Hadoop** : stockage distribué
- **Kafka** : streaming

### Bases de données
- SQL : PostgreSQL, MySQL
- NoSQL : MongoDB, Cassandra
- Data warehouses : Snowflake, BigQuery, Redshift
- Vector DB : Pinecone, Weaviate, Milvus

### BI & Visualisation
- Tableau, Power BI
- Looker, Metabase
- Superset (open source)

---

## 6. Entrepreneuriat

### Validation d'idée
- **Problem-Solution Fit** : le problème existe-t-il ?
- **Product-Market Fit** : la solution répond-elle au besoin ?
- Interviews clients (minimum 20-50)
- MVP (Minimum Viable Product)

### Business Model Canvas
1. Segments clients
2. Proposition de valeur
3. Canaux
4. Relations clients
5. Sources de revenus
6. Ressources clés
7. Activités clés
8. Partenaires clés
9. Structure de coûts

### Lean Startup
- Build → Measure → Learn
- Hypothèses → Expériences → Pivots
- Validated learning
- MVP, itérations rapides

### Financement

#### Étapes
1. **Bootstrapping** : fonds propres, proches
2. **Pre-seed** : business angels, premiers revenus
3. **Seed** : premiers VCs, validation traction
4. **Series A/B/C** : croissance, scale

#### Sources
- Love money : famille, amis
- Business Angels : investisseurs individuels
- Venture Capital : fonds d'investissement
- Crowdfunding : Kickstarter, Ulule
- Prêts : BPI, banques
- Subventions : France Num, régions

### Métriques Startup

#### SaaS
- **MRR/ARR** : revenus récurrents
- **Churn** : taux d'attrition
- **LTV** : valeur vie client
- **CAC** : coût acquisition client
- **LTV/CAC** : ratio >3 idéal

#### E-commerce
- Taux de conversion
- Panier moyen
- Coût par acquisition
- Marge brute

### Structure juridique (France)
| Forme | Capital | Responsabilité | Notes |
|-------|---------|----------------|-------|
| Auto-entrepreneur | 0 | Illimitée | Simple, plafonds |
| EURL | 1€ | Limitée apports | 1 associé |
| SARL | 1€ | Limitée apports | 2-100 associés |
| SAS | 1€ | Limitée apports | Flexible, levées |
| SA | 37 000€ | Limitée apports | Cotation possible |

### Aspects légaux
- Statuts, pacte d'associés
- CGV, CGU, mentions légales
- RGPD : conformité données
- Propriété intellectuelle : marques, brevets
- Contrats : travail, prestation, partenariat

### Équipe
- Cofondateurs : complémentarité, equity split
- Recrutement early stage : polyvalence, culture
- Equity : BSPCE, stock options, AGA
- Culture : vision, valeurs, communication

### Go-to-Market
- Positionnement, persona
- Canaux acquisition : SEO, SEA, social, content, PR
- Sales : inbound, outbound, self-service
- Pricing : freemium, tiered, usage-based
- Partnerships, channel sales

### Scale
- Processus, documentation
- Automatisation, outils
- Internationalisation
- M&A, exit strategies

## 7. Product Management

### Discovery
- User research : interviews, surveys
- Data analysis : comportements, funnel
- Competitive analysis
- Jobs-to-be-done framework

### Priorisation
- RICE : Reach, Impact, Confidence, Effort
- MoSCoW : Must, Should, Could, Won't
- Kano model : basic, performance, delight
- Value vs Effort matrix

### Méthodologies
- Agile/Scrum : sprints, ceremonies
- Kanban : flux continu
- Shape Up (Basecamp) : cycles 6 semaines
- Continuous discovery

### Métriques produit
- AARRR : Acquisition, Activation, Retention, Revenue, Referral
- Engagement : DAU, WAU, MAU
- Stickiness : DAU/MAU
- NPS : Net Promoter Score
- Feature adoption

### Documentation
- PRD : Product Requirements Document
- User stories, acceptance criteria
- Roadmap : now, next, later
- Release notes