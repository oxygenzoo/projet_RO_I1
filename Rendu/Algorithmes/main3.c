#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//----------------------------------------------------------
// Structure pour représenter un problème de flot
//----------------------------------------------------------
typedef struct {
    int n;              // Nombre de sommets
    int **capacity;     // Matrice des capacités (C)
    int **cost;         // Matrice des coûts (D)
} FlowProblem;

//----------------------------------------------------------
// Fonction pour allouer une matrice n x n initialisée à 0
//----------------------------------------------------------
int **allocate_matrix(int n) {
    int **mat = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        mat[i] = (int *)calloc(n, sizeof(int));  // Initialisation à 0
    }
    return mat;
}

//----------------------------------------------------------
// Fonction pour libérer une matrice n x n
//----------------------------------------------------------
void free_matrix(int **mat, int n) {
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}

//----------------------------------------------------------
// Génération aléatoire des matrices de capacité et de coût
//----------------------------------------------------------
void generate_random_flow_problem(FlowProblem *problem) {
    int n = problem->n;
    int edges = (n * n) / 2;  // Environ la moitié des arêtes possibles

    // Initialiser les matrices à 0
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            problem->capacity[i][j] = 0;
            if (problem->cost) {
                problem->cost[i][j] = 0;
            }
        }
    }

    // Remplir les matrices avec des valeurs aléatoires
    int count = 0;
    while (count < edges) {
        int i = rand() % n;
        int j = rand() % n;

        if (i != j && problem->capacity[i][j] == 0) {  // Pas de boucle (i != j) et pas de doublons
            problem->capacity[i][j] = (rand() % 100) + 1;  // Capacité aléatoire entre 1 et 100
            if (problem->cost) {
                problem->cost[i][j] = (rand() % 100) + 1;  // Coût aléatoire entre 1 et 100
            }
            count++;
        }
    }
}

//----------------------------------------------------------
// Sauvegarde d'un problème de flot dans un fichier texte
//----------------------------------------------------------
void save_flow_problem_to_file(FlowProblem *problem, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", filename);
        return;
    }

    int n = problem->n;
    fprintf(file, "%d\n", n);  // Écrire la taille de la matrice

    // Écrire la matrice des capacités
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(file, "%d ", problem->capacity[i][j]);
        }
        fprintf(file, "\n");
    }

    // Si une matrice de coûts existe, l'écrire également
    if (problem->cost) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                fprintf(file, "%d ", problem->cost[i][j]);
            }
            fprintf(file, "\n");
        }
    }

    fclose(file);
    printf("Problème de flot sauvegardé dans le fichier : %s\n", filename);
}

//----------------------------------------------------------
// Fonction principale pour générer et sauvegarder des problèmes
//----------------------------------------------------------
int main() {
    srand((unsigned int)time(NULL));  // Initialisation de la graine aléatoire

    // Tailles des problèmes de flot à générer
    int sizes[] = {10, 20, 40, 100, 400, 1000, 4000, 10000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];

        // Initialiser le problème de flot
        FlowProblem problem;
        problem.n = n;
        problem.capacity = allocate_matrix(n);
        problem.cost = allocate_matrix(n);

        // Générer les matrices aléatoires
        generate_random_flow_problem(&problem);

        // Sauvegarder dans un fichier
        char filename[50];
        sprintf(filename, "flow_problem_n%d.txt", n);  // Exemple : flow_problem_n10.txt
        save_flow_problem_to_file(&problem, filename);

        // Libérer la mémoire
        free_matrix(problem.capacity, n);
        free_matrix(problem.cost, n);
    }

    printf("Tous les problèmes de flot ont été générés et sauvegardés.\n");
    return 0;
}
