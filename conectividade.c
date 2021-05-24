#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphmatrix.h"

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

typedef struct {
    int first, last, size;
    unsigned capacity;
    int* array;
} Queue;

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

void sortString(char* string, int size) {
    for(int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (string[i] > string[j]) {
                char temp = string[i];
                string[i] = string[j];
                string[j] = temp;
         }
      }
   }
}

/* Color 0 <=> white, 1 <=> gray, 2 <=> black */

void visitDFS(Graph* graph, int node, int* time, int* colors, int* foundTimer, int* lowest, int* predecessor, char* tree, char* articulation, char* component) {
    colors[node] = 1;
    lowest[node] = foundTimer[node] = ++(*time);
    char str[15];
    char temp[graph->nodesNumber];
    temp[0] = '\0';
    sprintf(str, "%d ", node);
    strcat(tree, str);
    sprintf(str, "%d", node);
    strcat(component, str);

    for(int column = 0; column < graph->nodesNumber; column++) { // Percorre as adjacências
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
                    sprintf(str, "%d ", node);
                    strcat(temp, str);
                    strcat(articulation, str);
                }
            }
        }
    }
    colors[node] = 2;
}

void DFS (Graph* graph) {
    int colors[graph->nodesNumber], foundTimer[graph->nodesNumber], lowest[graph->nodesNumber], predecessor[graph->nodesNumber];
    char articulation[graph->nodesNumber];
    articulation[0] = '\0';
    char tree[(graph->nodesNumber * 2)];
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
    
    fprintf(filePointer, "\nComponentes conectados:\n");
    for(int line = 0; line < graph->nodesNumber; line++) {
        if(colors[line] == 0){
            count += 1;
            fprintf(filePointer, "C%i: ", count);
            char component[graph->nodesNumber];
            component[0] = '\0';
            visitDFS(graph, line, &time, colors, foundTimer, lowest, predecessor, tree, articulation, component);
            sortString(component, time - previous);
            for(int i = 0; i < time - previous; i++) {
                if(i == time - previous - 1) {
                    fprintf(filePointer, "%c", component[i]);
                    break;
                }
                fprintf(filePointer, "%c ", component[i]);
            }
            component[strlen(component) - 1] = '\0';
            previous = time;
            fprintf(filePointer, "\n");
        }
    }
    
    fprintf(filePointer, "\nBP:\n");
    tree[strlen(tree) - 1] = '\0';
    fprintf(filePointer, "%s", tree);
    fprintf(filePointer, "\n\nCaminhos BP:\n");

    char str[15];
    // SAFELINE
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
    fprintf(filePointer, "\nVertices de articulacao:\n");
    articulation[strlen(articulation) - 1] = '\0';
    fprintf(filePointer, "%s\n", articulation);
    fclose(filePointer);
}

void visitBFS(Graph* graph, int node, int* colors, int* distance, int* predecessor, char* tree) {
    colors[node] = 1;
    distance[node] = 0;
    Queue* queue = initializeQueue(graph->nodesNumber);
    insertQueue(queue, node);
    while(!isEmpty(queue)) {
        int target = dequeue(queue);
        char str[15];
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
    char str[15];
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

int main() {
    FILE* filePointer = fopen("saida.txt", "w");
    fclose(filePointer);
    Graph graph;
    readGraph("entrada.txt", &graph);
    print(&graph);
    BFS(&graph);
    DFS(&graph);
}