#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphmatrix.h"
#include "queue.h"

#define EMPTY_NODE -1 /* Signs an empty node.*/
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

/* Initializes a new graph, setting all fields to empty default value */

bool initialize(Graph *graph, int nodesNumber) {

    if(!nodeExists(graph, nodesNumber)) {
        fprintf(stderr, "[ERROR] nodes number is invalid %d\n", nodesNumber);
        return false;
    }
    graph->nodesNumber = nodesNumber; // Signs the number of nodes
    graph->edgesNumber = 0;

    for(int line = 0; line < graph->nodesNumber; line++) { // Initialize matrix structure
        for(int column = 0; column < graph->nodesNumber; column++) {
            graph->adjacencyList[line][column] = EMPTY_EDGE;
        }
    }
    return true;
}

int findKey(int key, int *array) {
    for(int i = 0; i < sizeof(array) / sizeof(array[0]); i++) {
        if(key == array[i]) return i;
    }
    return -1;
}

void initializeArray(int *array, int len) {
    for(int i = 0; i < len; i++) {
        array[i] = -1;
    }
}

/* Prints a graph structure, going through each valid/existent edge
value. */

bool print(Graph *graph) {

    FILE* filePointer;

    if(!(filePointer = fopen("saida.txt", "a+"))) {
        fprintf(stderr, "[PRINT] Failed to create saida.txt file\n");
        return false;
    }

    if(graph->nodesNumber == 0) {
        fprintf(stderr, "[PRINT] Cannot print an empty graph\n");
        return false;
    }

    fprintf(filePointer, "%d %d\n", graph->nodesNumber, graph->edgesNumber);
    int alreadyPrinted[graph->nodesNumber];
    initializeArray(alreadyPrinted, graph->nodesNumber);

    for(int line = 0; line < graph->nodesNumber; line++) {
        fprintf(stdout, "[PRINT] Node %d\nConnections: ", line);
        for(int column = 0; column < graph->nodesNumber; column++) {
            if(isConnected(graph, line, column) && alreadyPrinted[column] != column) {
                fprintf(filePointer, "%d %d %d\n", line, column, graph->adjacencyList[line][column]);
                fprintf(stdout, "%d (weight %d), ", column, graph->adjacencyList[line][column]);
            }
        }
        alreadyPrinted[line] = line;
        fprintf(stdout, "\n");
    }
    fclose(filePointer);
    return true;
}

/* Checks node value validity (if it's inbounds) */

bool nodeExists(Graph* graph, int node) {
    return (node < graph->nodesNumber && node >= 0);
}

/* Checks if there's an edge of any weight between two nodes */

bool isConnected(Graph* graph, int fromNode, int toNode) {
    return (graph->adjacencyList[fromNode][toNode] != EMPTY_EDGE);
}

/* Creates an edge between two given nodes. If it already exists,
increments weight value */

bool addEdge(Graph *graph, int fromNode, int toNode, Weight weight) {

    if(!nodeExists(graph, fromNode) || !nodeExists(graph, toNode)) {
        fprintf(stderr, "[ERROR] Cannot insert new edge: node %d or %d value(s) out of bounds\n", fromNode, toNode);
        return false;
    }

    graph->adjacencyList[fromNode][toNode] = weight;
    graph->adjacencyList[toNode][fromNode] = weight; // Uncomment to generate a digraph
    graph->edgesNumber += 1;
    
    fprintf(stdout, "[INSERT-EDGE] Edge inserted between %d and %d of weight %d\n", fromNode, toNode, graph->adjacencyList[fromNode][toNode]);
    return true;
    
}

/* Removes an edge between two given nodes */

bool removeEdge(Graph *graph, int fromNode, int toNode) {

    if(!nodeExists(graph, fromNode) || !nodeExists(graph, toNode)) {
        fprintf(stderr, "[ERROR] Cannot remove edge: node %d or %d value(s) out of bounds\n", fromNode, toNode);
        return false;
    }

    if(!isConnected(graph, fromNode, toNode)) {
        fprintf(stderr, "[ERROR] Cannot remove edge: nodes %d and %d edge not found\n", fromNode, toNode);
        return false;
    }

    graph->adjacencyList[fromNode][toNode] = EMPTY_EDGE;
    graph->adjacencyList[toNode][fromNode] = EMPTY_EDGE; // Uncomment if you want a digraph
    
    fprintf(stdout, "[REMOVE-EDGE] Edge removed between %d and %d\n", fromNode, toNode);
    return true;
    
}

/* Checks if an edge has neighbors */

bool noNeighbors(Graph *graph, int node) {
    
    if(!nodeExists(graph, node)) {
        fprintf(stderr, "[ERROR] Cannot check if node has neighbors: value %d out of bounds\n", node);
        return false;
    }

    for(int column = 0; column < graph->nodesNumber; column++) {
        if(isConnected(graph, node, column)) {
            return false;
        }
    }
    return true;
}

/* Returns next neighbor to a node */

int nextNeighbor(Graph *graph, int node, int adjacentNode) {

    if(!nodeExists(graph, node) || !nodeExists(graph, adjacentNode)) {
        fprintf(stderr, "[ERROR] Cannot check next neighbor: node %d or %d value(s) out of bounds\n", node, adjacentNode);
        return EMPTY_NODE;
    }

    for(int column = adjacentNode + 1; column < graph->nodesNumber; column++) {
        if(isConnected(graph, node, column)) {
            return column;
        }
    }
    return EMPTY_NODE;
}

/* Read input from file */

int readGraph(char* fileName, Graph* graph) {
    FILE* filePointer;
    int nodes, edges;

    filePointer = fopen(fileName, "r");
    if(!filePointer) {
        fprintf(stderr, "[READ-GRAPH] Unable to open file");
        return(0);
    }

    if(fscanf(filePointer, "%d %d", &nodes, &edges) != 2)
        return (0);
    initialize(graph, nodes);

    int fromNode, toNode;
    Weight weight;
    while((fscanf(filePointer, "%d %d %d", &fromNode, &toNode, &weight)) != EOF) {
        addEdge(graph, fromNode, toNode, weight);
    }

    fclose(filePointer);
    return (1);
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

/* DFS */

void DFS (Graph* graph) {
    int colors[graph->nodesNumber], foundTimer[graph->nodesNumber], lowest[graph->nodesNumber], predecessor[graph->nodesNumber];
    char* articulation;
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
    free(paths);
    fprintf(filePointer, "\nVertices de articulacao:\n");
    //articulation[strlen(articulation) - 1] = '\0';
    fprintf(filePointer, "%s", articulation);
    fclose(filePointer);
}

/* Color 0 <=> white, 1 <=> gray, 2 <=> black */

void visitDFS(Graph* graph, int node, int* time, int* colors, int* foundTimer, int* lowest, int* predecessor, char* tree, char* articulation, char* component) {
    colors[node] = 1;
    lowest[node] = foundTimer[node] = ++(*time);
    char str[15];
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
                    /*char* tmp = realloc(articulation, ((sizeof(articulation) / sizeof(char)) + 1) * sizeof(char));
                    if(!tmp) {
                        fprintf(stderr, "[DFS] Realloc to articulation buffer failed.\n");
                        free(articulation);
                        exit(-1);
                    }
                    articulation = tmp;*/
                    char temp[0];
                    sprintf(temp, "%i", node);
                    strcat(articulation, temp);
                }
            }
        }
    }
    //lowest[node] = ++(*time);
    colors[node] = 2;
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