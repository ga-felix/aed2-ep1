#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphmatrix.h"

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

typedef struct {
    int array[SIZE];
    int top;
} List;

List* initializeList() {
    List* list = (List*) malloc(sizeof(List));
    list->top = 0;
    list->array[list->top] = -1;
    return list;
}

void add(List* list, int value) {
    int i;
    for(i = 0; i < list->top; i++) {
        if(list->array[i] == value) return;
    }
    list->array[list->top] = value;
    list->top += 1;
    list->array[list->top] = -1;
}

void sortList(int* array, int size) {
    int i, j;
    for(i = 0; i < size; i++) {
        int key = array[i];
        j = i - 1;
        while(j >= 0 && array[j] > key) {
            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = key;
   }
}


/* Definição da estrutura auxiliar de fila */

typedef struct {
    int first, last, size;
    unsigned capacity;
    int* array;
} Queue;

/* Procedimentos necessários para manipular a fila:
- Inicialização
- Cheio
- Vazio
- Push
- Pop
*/

Queue* initializeQueue(unsigned capacity) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->first = queue->size = 0;
    queue->last = capacity - 1;
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

bool isFull(Queue* queue) {
    return (queue->size == queue->capacity);
}

bool isEmpty(Queue* queue) {
    return (queue->size == 0);
}
 
void insertQueue(Queue* queue, int item) {
    if (isFull(queue)) return;
    queue->last = (queue->last + 1) % queue->capacity;
    queue->array[queue->last] = item;
    queue->size = queue->size + 1;
}
 
int dequeue(Queue* queue) {
    if (isEmpty(queue)) return -1;
    int item = queue->array[queue->first];
    queue->first = (queue->first + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

/* Implementação da busca em profundidade e suas funções
auxiliares.
Tabela de cores (número, cor):
0 <=> branco
1 <=> cinza
2 <=> preto */

void visitDFS(Graph* graph, int node, int* time, int* colors, int* foundTimer, int* lowest, int* predecessor, char* tree, List* articulation, List* component) {
    colors[node] = 1;
    lowest[node] = foundTimer[node] = ++(*time);
    char str[4];
    char temp[graph->nodesNumber];
    temp[0] = '\0';
    sprintf(str, "%d ", node);
    strcat(tree, str);
    add(component, node);

    int column;
    for(column = 0; column < graph->nodesNumber; column++) { // Percorre as adjacências
        if(isConnected(graph, node, column)) { // Checa se existe aresta entre 'node' e 'column'
            if(colors[column] == 1) { // Aresta de retorno
                lowest[node] = min(lowest[node], foundTimer[column]);
            }
            
            if(colors[column] == 0) { // Checa se é branco
                predecessor[column] = node;
                visitDFS(graph, column, time, colors, foundTimer, lowest, predecessor, tree, articulation, component);
                lowest[node] = min(lowest[node], lowest[column]);
                if(lowest[column] >= foundTimer[node] && predecessor[node] != -1) {
                    fprintf(stdout, "Vertice de articulacao %i\n", node);
                    add(articulation, node);
                }
            }
        }
    }
    colors[node] = 2;
}

void DFS (Graph* graph) {
    int colors[graph->nodesNumber], foundTimer[graph->nodesNumber], lowest[graph->nodesNumber], predecessor[graph->nodesNumber];
    List* articulation = initializeList();
    List* component = initializeList();
    char groups[(graph->nodesNumber * 4)];
    groups[0] = '\0';
    char tree[(graph->nodesNumber * 4)];
    tree[0] = '\0';
    char paths[(graph->nodesNumber * graph->edgesNumber * 2)];
    paths[0] = '\0';
    int time = 0;
    for(int line = 0; line < graph->nodesNumber; line++) {
        colors[line] = 0;
        foundTimer[line] = 0;
        lowest[line] = 0;
        predecessor[line] = -1;
    }

    FILE* filePointer = fopen("saida.txt", "a");
    int count = 0;
    int previous = 0;

    for(int line = 0; line < graph->nodesNumber; line++) {
        if(colors[line] == 0){
            count += 1;
            char aux[4];
            sprintf(aux, "C%d: ", count);
            strcat(groups, aux);
            strcpy(aux, "");
            visitDFS(graph, line, &time, colors, foundTimer, lowest, predecessor, tree, articulation, component);
            int i;
            for(i = 0; i < component->top - 1; i++) {
                sprintf(aux, "%d ", component->array[i]);
                strcat(groups, aux);
                strcpy(aux, "");
            }
            sprintf(aux, "%d\n", component->array[component->top - 1]);
            strcat(groups, aux);
            strcpy(aux, "");
            component->top = 0;
        }
    }
    
    fprintf(filePointer, "\nBP:\n");
    tree[strlen(tree) - 1] = '\0';
    fprintf(filePointer, "%s", tree);
    fprintf(filePointer, "\n\nCaminhos BP:\n");

    char str[4];
    for(int index = 0; index < graph->nodesNumber; index++) {
        char line[(graph->nodesNumber * graph->edgesNumber * 2)];
        line[0] = '\0';
        int nextStep = index;
        bool hasNext = true;
        while(hasNext) {
            sprintf(str, "%d ", nextStep);
            char *tmp = strdup(line);
            strcpy(line, str);
            strcat(line, tmp);
            free(tmp);

            if(predecessor[nextStep] < 0) {
                hasNext = false;
                line[strlen(line) - 1] = '\0';
                strcat(paths, line);
                strcat(paths, "\n");
                strcpy(line, "");
            } else {
                nextStep = predecessor[nextStep];
            }
        }
    }
    fprintf(filePointer, "%s", paths);
    fprintf(filePointer, "\nComponentes Conectados:\n");
    fprintf(filePointer, "%s", groups);
    fprintf(filePointer, "\nVertices de articulação:\n");
    sortList(articulation->array, articulation->top);
    int i;
    for(i = 0; i < articulation->top - 1; i++) {
        fprintf(filePointer, "%d ", articulation->array[i]);
    }
    fprintf(filePointer, "%d\n", articulation->array[articulation->top - 1]);
    fclose(filePointer);
}

/* Implementação da busca em largura e suas funções
auxiliares.
Tabela de cores (número, cor):
0 <=> branco
1 <=> cinza
2 <=> preto */

void visitBFS(Graph* graph, int node, int* colors, int* distance, int* predecessor, char* tree) {
    colors[node] = 1;
    distance[node] = 0;
    Queue* queue = initializeQueue(graph->nodesNumber);
    insertQueue(queue, node);
    while(!isEmpty(queue)) {
        int target = dequeue(queue);
        char str[4];
        sprintf(str, "%d ", target);
        strcat(tree, str);
        for(int column = 0; column < graph->nodesNumber; column++) {
            if(isConnected(graph, target, column)) {
                if(colors[column] == 0) {
                    colors[column] = 1;
                    predecessor[column] = target;
                    distance[column] = distance[target] + 1;
                    insertQueue(queue, column);
                }
            }
        }
        colors[target] = 2;
    }
}

void BFS(Graph* graph) {
    char tree[(graph->nodesNumber * 2)];
    FILE* filePointer = fopen("saida.txt", "a");
    tree[0] = '\0';
    int colors[graph->nodesNumber], distance[graph->nodesNumber], predecessor[graph->nodesNumber];
    for(int line = 0; line < graph->nodesNumber; line++) {
        colors[line] = 0;
        distance[line] = -1;
        predecessor[line] = -1;
    }
    for(int line = 0; line < graph->nodesNumber; line++) {
        if(colors[line] == 0) {
            visitBFS(graph, line, colors, distance, predecessor, tree);
        }
    }

    fprintf(filePointer, "\nBL:\n");
    tree[strlen(tree) - 1] = '\0';
    fprintf(filePointer, "%s", tree);

    char paths[(graph->nodesNumber * graph->edgesNumber * 2)];
    char str[4];
    paths[0] = '\0';
    fprintf(filePointer, "\n\nCaminhos BL:\n");

    for(int index = 0; index < graph->nodesNumber; index++) {
        char line[(graph->nodesNumber * graph->edgesNumber * 2)];
        line[0] = '\0';
        int nextStep = index;
        bool hasNext = true;
        while(hasNext) {
            sprintf(str, "%d ", nextStep);
            char *tmp = strdup(line);
            strcpy(line, str);
            strcat(line, tmp);
            free(tmp);

            if(predecessor[nextStep] < 0) {
                hasNext = false;
                line[strlen(line) - 1] = '\0';
                strcat(paths, line);
                strcat(paths, "\n");
                strcpy(line, "");
            } else {
                nextStep = predecessor[nextStep];
            }
        }
    }
    fprintf(filePointer, "%s", paths);
    fclose(filePointer);
}

/* Função de teste. Entrada do programa. */

int main() {
    FILE* filePointer = fopen("saida.txt", "w");
    fclose(filePointer);
    Graph graph;
    readGraph("entrada.txt", &graph);
    print(&graph);
    BFS(&graph);
    DFS(&graph);
}