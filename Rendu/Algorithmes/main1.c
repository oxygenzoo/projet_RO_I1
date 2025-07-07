#include <stdio.h>      // Inclusion de la bibliothèque standard d'entrée/sortie
#include <stdlib.h>     // Inclusion de la bibliothèque standard pour malloc, free, etc.
#include <string.h>     // Inclusion de la bibliothèque pour strcmp, etc.
#include <limits.h>     // Inclusion de la bibliothèque pour INT_MAX
#include <time.h>
#include <math.h>

// Définition d'une structure FlowNetwork pour représenter un réseau de flot
typedef struct {
    int n;              // Nombre de sommets dans le réseau
    int **capacity;     // Pointeur sur une matrice des capacités (n x n)
    int **cost;         // Pointeur sur une matrice des coûts (n x n), NULL si pas de coûts
    int s, t;           // Indices de la source (s) et du puits (t)
    int isMinCost;      // Indicateur (0 ou 1) pour savoir si c'est un problème de flot max (0) ou min (1)
} FlowNetwork;

// Fonction pour allouer une matrice n x n d'entiers et l'initialiser à 0
int **allocate_matrix(int n) {
    int **mat = (int **)malloc(n * sizeof(int *)); // Alloue un tableau de n pointeurs vers des int*
    for (int i = 0; i < n; i++) {                   // Boucle sur chaque ligne
        mat[i] = (int *)calloc(n, sizeof(int));     // Alloue et initialise à 0 une ligne de n int
    }
    return mat; // Retourne le pointeur vers la matrice allouée
}

// Fonction pour libérer la mémoire d'une matrice n x n
void free_matrix(int **mat, int n) {
    for (int i = 0; i < n; i++) { // Boucle sur chaque ligne de la matrice
        free(mat[i]);            // Libère la mémoire de la ligne i
    }
    free(mat); // Libère le tableau de pointeurs
}

// Fonction pour afficher une matrice n x n avec un titre
void print_matrix(int **mat, int n, const char *title) {
    printf("%s\n", title);              // Affiche le titre
    for (int i = 0; i < n; i++) {       // Parcourt chaque ligne
        for (int j = 0; j < n; j++) {   // Parcourt chaque colonne
            printf("%4d ", mat[i][j]);  // Affiche la valeur mat[i][j] avec un espacement
        }
        printf("\n");                  // Retour à la ligne après chaque ligne de la matrice
    }
    printf("\n");                       // Ligne vide après la matrice
}

// Fonction pour lire les données du fichier d'entrée dans la structure FlowNetwork
int read_input_file(const char *filename, FlowNetwork *G) {
    FILE *f = fopen(filename, "r");             // Ouvre le fichier en lecture
    if (!f) {                                   // Si f est NULL, erreur d'ouverture
        fprintf(stderr, "Impossible d'ouvrir le fichier %s. Vérifiez le nom et réessayez.\n", filename); // Message d'erreur plus clair
        return 0;                               // Retourne 0 pour indiquer l'échec, ce qui permettra de redemander un nom de fichier
    }

    fscanf(f, "%d", &(G->n));        // Lit le nombre de sommets n
    G->capacity = allocate_matrix(G->n); // Alloue la matrice de capacités de taille n x n

    for (int i = 0; i < G->n; i++) {     // Boucle sur chaque ligne de la matrice de capacités
        for (int j = 0; j < G->n; j++) { // Boucle sur chaque colonne
            fscanf(f, "%d", &(G->capacity[i][j])); // Lit la capacité c(i,j) depuis le fichier
        }
    }

    int c = fgetc(f);                              // Lit un caractère pour voir s'il y a plus de données
    while (c == ' ' || c == '\n' || c == '\r' || c == '\t') c = fgetc(f); // Ignore les espaces et sauts de ligne
    if (c != EOF) {                                // S'il y a encore des données dans le fichier
        ungetc(c, f);                              // Replace le caractère lu
        G->cost = allocate_matrix(G->n);            // Alloue la matrice de coûts n x n
        for (int i = 0; i < G->n; i++) {            // Parcourt chaque ligne de la matrice de coûts
            for (int j = 0; j < G->n; j++) {        // Parcourt chaque colonne
                fscanf(f, "%d", &(G->cost[i][j]));  // Lit le coût d(i,j)
            }
        }
        G->isMinCost = 1;                           // Indique qu'il s'agit d'un problème de flot à coût min
    } else {
        G->isMinCost = 0;                           // Pas de matrice de coûts
        G->cost = NULL;                             // cost reste NULL
    }

    G->s = 0;                   // Définition de la source s=0
    G->t = G->n - 1;            // Définition du puits t=n-1

    fclose(f);                  // Ferme le fichier
    return 1;                   // Retourne 1 pour indiquer le succès
}

// Fonction auxiliaire : BFS utilisée par Edmond-Karp pour trouver un chemin augmentant
int bfs_edmond_karp(int n, int **residual, int s, int t, int parent[]) {
    int *visited = (int *)calloc(n, sizeof(int)); // Tableau visited initialisé à 0
    for (int i = 0; i < n; i++)
        parent[i] = -1;        // Initialise parent[] à -1

    visited[s] = 1;            // Marque la source comme visitée
    parent[s] = -1;            // La source n'a pas de parent

    int *queue = (int *)malloc(n * sizeof(int));  // File pour la BFS
    int front = 0, rear = 0;                      // Indices de la file
    queue[rear++] = s;                            // Enfile la source

    while (front < rear) {          // Tant que la file n'est pas vide
        int u = queue[front++];     // Défile un sommet u
        for (int v = 0; v < n; v++) {           // Parcourt tous les sommets v
            if (!visited[v] && residual[u][v] > 0) { // Si non visité et capacité résiduelle > 0
                queue[rear++] = v;                // Enfile v
                parent[v] = u;                    // Met à jour le parent de v
                visited[v] = 1;                   // Marque v comme visité
                if (v == t) {                     // Si on a atteint le puits
                    free(queue);                  // Libère la file
                    free(visited);                // Libère visited
                    return 1;                     // Chemin trouvé
                }
            }
        }
    }
    free(queue);   // Libère la file si pas de chemin
    free(visited); // Libère visited
    return 0;       // Aucun chemin trouvé
}

// Algorithme Ford-Fulkerson (Edmond-Karp) pour trouver le flot max
int ford_fulkerson(FlowNetwork *G) {
    int n = G->n;                                   // Récupère le nombre de sommets
    int **residual = allocate_matrix(n);            // Alloue le graphe résiduel
    for (int i = 0; i < n; i++) {                   // Copie des capacités dans residual
        for (int j = 0; j < n; j++) {
            residual[i][j] = G->capacity[i][j];     // Initialisation du graphe résiduel = capacités
        }
    }

    int *parent = (int *)malloc(n * sizeof(int));   // Tableau pour stocker le chemin BFS
    int max_flow = 0;                               // Flot maximal initialisé à 0

    // Tant qu'un chemin augmentant existe
    while (bfs_edmond_karp(n, residual, G->s, G->t, parent)) {
        int path_flow = INT_MAX;                    // Flot possible sur ce chemin
        for (int v = G->t; v != G->s; v = parent[v]) { // Remonte le chemin trouvé
            int u = parent[v];                      // u est le parent de v dans le chemin
            if (residual[u][v] < path_flow)
                path_flow = residual[u][v];         // Trouve le minimum résiduel sur le chemin
        }

        for (int v = G->t; v != G->s; v = parent[v]) { // Met à jour le graphe résiduel
            int u = parent[v];
            residual[u][v] -= path_flow;            // Réduit la capacité résiduelle dans le sens u->v
            residual[v][u] += path_flow;            // Augmente la résiduelle dans l'autre sens v->u
        }

        printf("Chemin augmentant trouvé avec flot = %d\n", path_flow); // Affiche le flot trouvé sur le chemin
        max_flow += path_flow;                     // Ajoute path_flow au flot max total
    }

    free(parent);           // Libère parent
    free_matrix(residual, n); // Libère le graphe résiduel
    return max_flow;        // Retourne le flot maximal
}

// push_func : fonction pour pousser du flot de u vers v dans l'algorithme Push-Relabel
static void push_func(int u, int v, int n, int **residual, int *excess) {
    int send = (excess[u] < residual[u][v]) ? excess[u] : residual[u][v]; // On envoie le min(excess[u],residual[u][v])
    residual[u][v] -= send;      // Réduit la résiduelle u->v
    residual[v][u] += send;      // Augmente la résiduelle v->u
    excess[u] -= send;           // Réduit l'excès de u
    excess[v] += send;           // Augmente l'excès de v
    // Au lieu de "Push de %d unités de %d vers %d", on affiche en français :
    printf("Envoi de %d unités de flot du sommet %d vers le sommet %d\n", send, u+1, v+1); // Affiche l'action d'envoi
}

// relabel_func : fonction pour réétiqueter (augmenter la hauteur du sommet u)
static void relabel_func(int u, int n, int **residual, int *height) {
    int mh = INT_MAX;                      // mh = hauteur minimale parmi les voisins admissibles
    for (int v = 0; v < n; v++) {          // Parcourt tous les voisins v
        if (residual[u][v] > 0 && height[v] < mh) { // Si arête résiduelle u->v positive et height[v] plus petite
            mh = height[v];                // Met à jour mh
        }
    }
    if (mh < INT_MAX) {
        height[u] = mh + 1;                // Augmente la hauteur de u
        // Au lieu de "Relabel du sommet ...", on affiche la version française :
        printf("Réétiquetage du sommet %d, nouvelle hauteur = %d\n", u+1, height[u]); // Affiche l'action de réétiquetage
    }
}

// Algorithme pousser-réétiqueter (Push-Relabel) pour le flot max
int push_relabel(FlowNetwork *G) {
    int n = G->n;                                  // Nombre de sommets
    int **capacity = G->capacity;                  // Raccourci vers la matrice des capacités

    int **residual = allocate_matrix(n);           // Alloue le graphe résiduel
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            residual[i][j] = capacity[i][j];       // Copie des capacités dans residual

    int *height = (int *)calloc(n, sizeof(int));   // Tableau des hauteurs, init à 0
    int *excess = (int *)calloc(n, sizeof(int));   // Tableau des excès, init à 0
    int *seen = (int *)calloc(n, sizeof(int));     // Tableau vu, pas forcément utilisé ici

    height[G->s] = n;                              // Hauteur de la source = n
    for (int v = 0; v < n; v++) {                  // Pousse immédiatement depuis la source
        if (residual[G->s][v] > 0) {
            excess[v] = residual[G->s][v];         // L'excès du voisin v = la capacité s->v
            residual[v][G->s] = residual[G->s][v]; // Résiduel inverse
            residual[G->s][v] = 0;                 // On a poussé tout le flot possible
        }
    }

    while (1) {
        int done = 1;                      // Indicateur si aucune action n'est effectuée
        for (int u = 0; u < n; u++) {      // Parcourt tous les sommets
            if (u != G->s && u != G->t && excess[u] > 0) { // Si u a de l'excès (hors s,t)
                done = 0;                                  // On va tenter quelque chose
                for (int v = 0; v < n && excess[u] > 0; v++) { // Essaye de pousser vers les voisins
                    if (residual[u][v] > 0 && height[u] == height[v] + 1) {
                        push_func(u, v, n, residual, excess); // Pousse si admissible
                    }
                }
                if (excess[u] > 0) {        // Si après tentative de push, excès > 0
                    relabel_func(u, n, residual, height); // On relabel u
                }
            }
        }
        if (done)                           // Si done = 1, aucune action, on s'arrête
            break;
    }

    int max_flow = excess[G->t];            // Le flot max est l'excès au puits
    free(height);                           // Libère height
    free(excess);                           // Libère excess
    free(seen);                             // Libère seen
    free_matrix(residual, n);               // Libère le graphe résiduel
    return max_flow;                        // Retourne le flot max
}

// Bellman-Ford pour trouver un plus court chemin en coût dans le graphe résiduel
int bellman_ford(int n, int **capacity, int **cost, int **residual_cap, int **residual_cost, int s, int t, int *parent) {
    int *dist = (int *)malloc(n * sizeof(int)); // Tableau des distances
    for (int i = 0; i < n; i++) {
        dist[i] = INT_MAX;          // Distance initiale = infinie
        parent[i] = -1;             // Pas de parent au début
    }
    dist[s] = 0;                     // Distance à la source = 0

    for (int k = 0; k < n-1; k++) {  // Relaxation n-1 fois
        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                if (residual_cap[u][v] > 0 && dist[u] != INT_MAX && dist[u] + residual_cost[u][v] < dist[v]) {
                    dist[v] = dist[u] + residual_cost[u][v]; // Mise à jour de la distance
                    parent[v] = u;                            // Mise à jour du parent
                }
            }
        }
    }

    printf("Table de Bellman-Ford (dist):\n"); // Affiche les distances trouvées
    for (int i = 0; i < n; i++) {
        if (dist[i] == INT_MAX) printf("  INF");  // Affiche INF si inaccessible
        else printf("%5d", dist[i]);              // Sinon affiche la distance
    }
    printf("\n");

    int reachable = (dist[t] != INT_MAX); // 1 si t est accessible, 0 sinon
    free(dist);                           // Libère dist
    return reachable;                     // Retourne reachable
}

// Flot à coût minimal pour un flot donné desired_flow
int min_cost_flow(FlowNetwork *G, int desired_flow) {
    int n = G->n;                                         // Nombre de sommets
    int **residual_cap = allocate_matrix(n);              // Graphe résiduel pour les capacités
    int **residual_cost = allocate_matrix(n);             // Graphe résiduel pour les coûts

    for (int i = 0; i < n; i++) {                         // Initialisation du graphe résiduel
        for (int j = 0; j < n; j++) {
            residual_cap[i][j] = G->capacity[i][j];       // Copie des capacités
            residual_cost[i][j] = G->cost[i][j];          // Copie des coûts
        }
    }

    int *parent = (int *)malloc(n * sizeof(int));          // Tableau parent pour Bellman-Ford
    int flow = 0;                                          // Flot actuel = 0
    int cost_total = 0;                                    // Coût total = 0

    while (flow < desired_flow) {                          // Tant qu'on n'a pas atteint le flot désiré
        if (!bellman_ford(n, G->capacity, G->cost, residual_cap, residual_cost, G->s, G->t, parent))
            break; // Pas de chemin augmentant de coût minimal si Bellman-Ford échoue

        int path_flow = desired_flow - flow;               // On peut envoyer au plus (desired_flow - flow)
        for (int v = G->t; v != G->s; v = parent[v]) {     // Trouve le max de flot possible sur ce chemin
            int u = parent[v];
            if (residual_cap[u][v] < path_flow)
                path_flow = residual_cap[u][v];            // min sur le chemin
        }

        for (int v = G->t; v != G->s; v = parent[v]) {     // Met à jour le graphe résiduel
            int u = parent[v];
            residual_cap[u][v] -= path_flow;               // Réduit la capacité résiduelle u->v
            residual_cap[v][u] += path_flow;               // Augmente la résiduelle v->u
            cost_total += path_flow * residual_cost[u][v]; // Ajoute le coût
        }

        printf("Chaîne augmentante trouvée, flot = %d\n", path_flow); // Affiche le flot sur ce chemin
        flow += path_flow;                                  // Met à jour le flot total
    }

    if (flow < desired_flow) {                              // Si on n'a pas atteint le flot désiré
        printf("Impossible d'atteindre le flot désiré.\n");
    } else {
        printf("Flot atteint = %d, Coût total = %d\n", flow, cost_total); // Affiche le flot et le coût total
    }

    free(parent);                       // Libère parent
    free_matrix(residual_cap, n);       // Libère residual_cap
    free_matrix(residual_cost, n);      // Libère residual_cost
    return 0;                           // Retourne 0
}

// Fonction main : point d'entrée du programme
int main() {
    FlowNetwork G;                                // Déclare une structure FlowNetwork
    char filename[256];                           // Tableau pour stocker le nom du fichier

    while (1) {                                   // Boucle infinie jusqu'à ce que l'utilisateur entre "0"
        printf("Entrez le nom du fichier (ou 0 pour quitter) : "); // Demande le nom du fichier
        if (scanf("%s", filename) != 1) {          // Lit la chaîne dans filename
            fprintf(stderr, "Entrée invalide.\n"); // Si échec, message d'erreur
            break;                                 // Sort de la boucle
        }
        if (strcmp(filename, "0") == 0) break;     // Si l'utilisateur tape "0", on quitte la boucle

        if (!read_input_file(filename, &G)) {      // Lit les données du fichier dans G
            // Si lecture ou ouverture échoue, on continue la boucle pour redemander un fichier
            continue;
        }

        print_matrix(G.capacity, G.n, "Matrice des Capacités:"); // Affiche la matrice des capacités
        if (G.isMinCost) {                         // Si flot à coût min
            print_matrix(G.cost, G.n, "Matrice des Coûts:"); // Affiche la matrice des coûts
        }

        if (!G.isMinCost) {                        // Si problème de flot max
            int choix_algo;                        // Variable pour stocker le choix de l'algorithme
            // Au lieu d'afficher "Pousser-Relabeller", on met "Pousser-Réétiqueter"
            printf("Choisissez l'algorithme de flot max:\n1 - Ford-Fulkerson (Edmond-Karp)\n2 - Pousser-Réétiqueter\nVotre choix: ");
            if (scanf("%d", &choix_algo) != 1) {   // Lit le choix
                fprintf(stderr, "Entrée invalide.\n");
                continue;                           // Recommence la boucle
            }
            int max_flow;                           // Variable pour stocker le flot max
            if (choix_algo == 1) {                  // Si choix = 1
                max_flow = ford_fulkerson(&G);      // Appelle ford_fulkerson
            } else {
                max_flow = push_relabel(&G);        // Sinon appelle push_relabel
            }
            printf("Flot maximal trouvé = %d\n", max_flow); // Affiche le flot max
        } else {
            // Si problème de flot à coût min
            int val_flot;                           // Valeur du flot désiré
            printf("Entrez la valeur de flot désirée: ");
            if (scanf("%d", &val_flot) != 1) {      // Lit val_flot
                fprintf(stderr, "Entrée invalide.\n");
                continue;                           // Recommence la boucle
            }
            min_cost_flow(&G, val_flot);            // Lance l'algorithme de flot à coût min
        }

        free_matrix(G.capacity, G.n);               // Libère la matrice des capacités
        if (G.isMinCost) {                          // Si flot à coût min, libère aussi la matrice de coûts
            free_matrix(G.cost, G.n);
        }
    }

    return 0; // Retourne 0 pour indiquer que le programme s'est terminé normalement
}