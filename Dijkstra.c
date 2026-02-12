#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <assert.h>



typedef struct {
    int **adj_matrix;
    char *vertex_data;
    int size;
} Graph;

Graph* create_graph(int size) {
    Graph *g = malloc(sizeof(Graph));
    g->size = size;
    g->adj_matrix = malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        g->adj_matrix[i] = calloc(size, sizeof(int));
    }
    g->vertex_data = malloc(size * sizeof(char));
    // Inicializa com caracteres genéricos para benchmarks
    for(int i=0; i<size; i++) g->vertex_data[i] = '0' + (i % 10);
    return g;
}

void add_edge(Graph *g, int u, int v, int weight) {
    if (u >= 0 && u < g->size && v >= 0 && v < g->size) {
        g->adj_matrix[u][v] = weight;
        g->adj_matrix[v][u] = weight;  
    }
}

void add_vertex_data(Graph *g, int vertex, char data) {
    if (vertex >= 0 && vertex < g->size) {
        g->vertex_data[vertex] = data;
    }
}

void free_graph(Graph *g) {
    for (int i = 0; i < g->size; i++) {
        free(g->adj_matrix[i]);
    }
    free(g->adj_matrix);
    free(g->vertex_data);
    free(g);
}

int min_distance(int distances[], bool visited[], int size) {
    int min = INT_MAX, min_index = -1;
    for (int v = 0; v < size; v++) {
        if (!visited[v] && distances[v] <= min) {
            min = distances[v];
            min_index = v;
        }
    }
    return min_index;
}

int* dijkstra_solve(Graph *g, int start_vertex_index) {
    int *distances = malloc(g->size * sizeof(int));
    bool *visited = malloc(g->size * sizeof(bool));

    for (int i = 0; i < g->size; i++) {
        distances[i] = INT_MAX;
        visited[i] = false;
    }
    distances[start_vertex_index] = 0;

    for (int i = 0; i < g->size - 1; i++) {
        int u = min_distance(distances, visited, g->size);
        
        // Se u é -1 ou INT_MAX, o resto do grafo é inalcançável
        if (u == -1 || distances[u] == INT_MAX) break;

        visited[u] = true;

        for (int v = 0; v < g->size; v++) {
            if (!visited[v] && g->adj_matrix[u][v] && distances[u] != INT_MAX 
                && distances[u] + g->adj_matrix[u][v] < distances[v]) {
                distances[v] = distances[u] + g->adj_matrix[u][v];
            }
        }
    }
    
    free(visited);
    return distances;
}

// Verifica se a Desigualdade Triangular é mantida: Dist(v) <= Dist(u) + Peso(u,v)
bool verify_correctness(Graph *g, int *distances, int start_index) {
    if (distances[start_index] != 0) {
        printf("[ERRO] Distancia da origem nao e 0.\n");
        return false;
    }

    for (int u = 0; u < g->size; u++) {
        // Se u não foi alcançado, ignoramos arestas saindo dele
        if (distances[u] == INT_MAX) continue;

        for (int v = 0; v < g->size; v++) {
            if (g->adj_matrix[u][v] > 0) { // Existe aresta
                int weight = g->adj_matrix[u][v];
                long long d_u = distances[u]; // Cast para evitar overflow na soma
                long long d_v = distances[v];

                // Propriedade de Relaxamento: Nenhum caminho pode ser melhorado
                if (d_v > d_u + weight) {
                    printf("[ERRO] Violacao em %d->%d. Dist[%d]=%lld, Dist[%d]=%lld + %d\n", 
                           u, v, v, d_v, u, d_u, weight);
                    return false;
                }
            }
        }
    }
    return true;
}

Graph* generate_random_graph(int size, int density_percent) {
    Graph *g = create_graph(size);
    srand(time(NULL)); 
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            // Adiciona aresta baseado na densidade
            if ((rand() % 100) < density_percent) {
                int w = (rand() % 100) + 1; // Pesos 1-100
                add_edge(g, i, j, w);
            }
        }
    }
    return g;
}

void run_benchmark() {
    printf("\n--- INICIANDO AVALIACAO EMPIRICA ---\n");
    printf("Gerando dados para o grafico (CSV format)...\n");
    printf("Tamanho_V,Tempo_Segundos\n");

    // Testamos tamanhos crescentes para ver a curva quadrática
    int sizes[] = {100, 500, 1000, 2000, 3000, 4000, 5000}; 
    int num_sizes = 7;

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        Graph *g = generate_random_graph(n, 50); // Grafo com 50% de densidade

        clock_t start = clock();
        int *dists = dijkstra_solve(g, 0); // Executa
        clock_t end = clock();

        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        
        printf("%d,%.6f\n", n, time_taken);

        free(dists);
        free_graph(g);
    }
    printf("--- FIM DO BENCHMARK ---\n");
    printf("Copie os dados acima para Excel/Python para gerar o grafico.\n");
}


int main() {
    int choice;
    printf("Escolha o modo de execucao:\n");
    printf("1 - Executar Exemplo Original (Letras A-G)\n");
    printf("2 - Validacao de Corretude (Teste Automatizado)\n");
    printf("3 - Avaliacao Empirica (Gerar dados para Grafico)\n");
    printf("Opcao: ");
    scanf("%d", &choice);

    if (choice == 1 || choice == 2) {
        // Setup do Grafo Original
        Graph *g = create_graph(7);
        add_vertex_data(g, 0, 'A'); add_vertex_data(g, 1, 'B');
        add_vertex_data(g, 2, 'C'); add_vertex_data(g, 3, 'D');
        add_vertex_data(g, 4, 'E'); add_vertex_data(g, 5, 'F');
        add_vertex_data(g, 6, 'G');

        add_edge(g, 3, 0, 4); add_edge(g, 3, 4, 2); add_edge(g, 0, 2, 3);
        add_edge(g, 0, 4, 4); add_edge(g, 4, 2, 4); add_edge(g, 4, 6, 5);
        add_edge(g, 2, 5, 5); add_edge(g, 2, 1, 2); add_edge(g, 1, 5, 2);
        add_edge(g, 6, 5, 5);

        // Executa Dijkstra partindo de D (índice 3)
        int start_node = 3;
        int *dists = dijkstra_solve(g, start_node);

        if (choice == 1) {
            printf("\nResultados partindo de %c:\n", g->vertex_data[start_node]);
            for(int i=0; i<g->size; i++) {
                printf("Para %c: %d\n", g->vertex_data[i], dists[i]);
            }
        } else {
            printf("\nVerificando corretude...\n");
            if(verify_correctness(g, dists, start_node)) {
                printf("SUCESSO: O algoritmo produziu um resultado valido.\n");
            } else {
                printf("FALHA: O algoritmo produziu um resultado invalido.\n");
            }
        }
        
        free(dists);
        free_graph(g);
    } 
    else if (choice == 3) {
        run_benchmark();
    }

    return 0;
}