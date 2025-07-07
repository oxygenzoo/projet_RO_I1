# Projet de Recherche Opérationnelle – Problèmes de Flot

## Contexte pédagogique

Ce projet a été réalisé dans le cadre du module Recherche Opérationnelle à l'EFREI Paris (2024-2025). Il s'inscrit dans l’étude approfondie de la théorie des graphes, appliquée aux problèmes de flot.

- Langages utilisés : au choix (C pour les algorithmes, Python pour l’analyse de performance) 
- Remise : juillet 2025  
- Équipe projet : 4 étudiants  
- Pondération : projet final du module
- Note : 15

## Sujet

Le projet porte sur la modélisation et la résolution de problèmes de flot :

- Flot Maximal avec Ford-Fulkerson (algorithme glouton basé sur BFS)  
- Flot Maximal avec Pousser-Réétiqueter (push-relabel)  
- Flot à Coût Minimal résolu via l’algorithme de Bellman-Ford  

Chaque algorithme est testé et comparé sur plusieurs instances aléatoires de graphes orientés pondérés.

## Objectifs du projet

- Implémenter 3 algorithmes classiques de flot  
- Générer dynamiquement des graphes de différentes tailles  
- Évaluer et comparer expérimentalement les performances  
- Tracer les courbes de temps d'exécution en Python  

## Structure du projet

```
Projet/
├── ennoncé
├── Rendu/
│   ├── Algorithmes/  
│   │   ├── main1.c        # Test manuel d'un fichier (1 exécution)  
│   │   ├── main2.c        # Benchmark automatique sur 100 itérations  
│   │   ├── main3.c        # Générateur aléatoire de graphes  
│   │   └── python2.py     # Génère la courbe des temps d'exécution 
│   ├── Graphes/  
│   │   ├── comparison_FF_PR.png
│   │   ├── max_times_graph.png
│   ├── Benchmark/  
│   │   └── max_values.txt # Temps max pour chaque algo (utilisé par Python) 
│   │   └── results.txt
│   │   ├── flow_problem_n10.txt  
│   │   ├── ...  
│   │   └── flow_problem_n10000.txt
│   ├── Data/  
│   │   └── RO1.txt
│   │   └── ...
│   │   └── RO10.txt
│   ├── Rapport/  
│   │   └── Rapport_Etude_Complexite.docx
│   ├── Traces/  
│   │   └── A1-trace1-FF.txt
│   │   └── ...
│   │   └── A1-trace010-MIN.txt
```

## Utilisation

### 1. Compiler les programmes en C

Depuis le dossier algorithmes/ :

```bash
gcc main1.c -o test_unitaire -lm
gcc main2.c -o benchmark -lm
gcc main3.c -o generateur -lm
```

### 2. Générer les fichiers de test

```bash
./generateur
```

### 3. Lancer un test manuel

```bash
./test_unitaire graphes/flow_problem_n100.txt
```

### 4. Lancer le benchmark automatique (100 répétitions)

```bash
./benchmark
```

### 5. Tracer les résultats (Python)

```bash
cd analyse
python3 python2.py
```

## Analyse de Complexité

| Algorithme             | Complexité théorique      | Remarques                        |
|------------------------|---------------------------|----------------------------------|
| Ford-Fulkerson (FF)    | O(E * max_flow)           | Peut être lent sur grands flots  |
| Pousser-Réétiqueter    | O(V²√E) à O(V³)            | Efficace pour les grands graphes |
| Flot à coût minimal    | O(VE) par Bellman-Ford     | Peut traiter des graphes pondérés (coûts)

Les temps sont mesurés via clock() en C, et regroupés dans un fichier max_values.txt.

## Visualisation des performances

Le script Python `analyse/python2.py` génère une courbe logarithmique des temps max pour chaque algorithme sur des graphes de tailles croissantes (n = 10 à 10 000).

Exemple de graphe :

- Axe X : taille du graphe n (logarithmique)  
- Axe Y : temps d’exécution maximal (logarithmique)  
- Courbes : FF, PR, MIN-COST (différenciées par des marqueurs)  

## Compétences mobilisées

- Implémentation efficace d’algorithmes de graphe  
- Gestion mémoire dynamique en C  
- Génération et parsing de fichiers  
- Analyse expérimentale de complexité  
- Visualisation scientifique avec Python (matplotlib)  

## Auteurs

- Guignabert Enzo  
- Mehenni Aïssa  
- Perier Stanley  
- Duez Kiliann  
Étudiants en Informatique – EFREI Paris – Promotion 2025
