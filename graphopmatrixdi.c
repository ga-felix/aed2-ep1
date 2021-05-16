#include <stdio.h>
#include <string.h>
#include "graphmatrix.h"

#define EMPTY_NODE -1 /* Signs an empty node

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

void initializeArray(int *array) {
    for(int i = 0; i < sizeof(array) / sizeof(array[0]); i++) {
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
    initializeArray(alreadyPrinted);

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

/* DFS */

void DFS (Graph* graph) {
    int colors[graph->nodesNumber], foundTimer[graph->nodesNumber], endTimer[graph->nodesNumber], predecessor[graph->edgesNumber];
    char tree[(graph->nodesNumber * 2)];
    tree[0] = '\0';
    char paths[(graph->nodesNumber * graph->edgesNumber * 2)];
    paths[0] = '\0';
    int time = 0;
    for(int line = 0; line < graph->nodesNumber; line++) {
        colors[line], foundTimer[line], endTimer[line] = 0;
        predecessor[line] = -1;
    }

    FILE* filePointer = fopen("saida.txt", "a");
    fprintf(filePointer, "\nBP:\n");

    for(int line = 0; line < graph->nodesNumber; line++) {
        visitDFS(graph, line, &time, colors, foundTimer, endTimer, predecessor, filePointer, tree, paths);
    }

    fprintf(filePointer, "%s", tree);
}

/* Color 0 <=> white, 1 <=> gray, 2 <=> black */

void visitDFS(Graph* graph, int node, int* time, int* colors, int* foundTimer, int* endTimer, int* predecessor, FILE* f, char* tree, char* paths) {
    colors[node] = 1;
    foundTimer[node] = ++(*time);
    char* str;
    if(node != graph->nodesNumber - 1) {
        sprintf(str, "%d ", node);
        strcat(tree, str);
    } else {
        sprintf(str, "%d\n", node);
        strcat(tree, str);
    }

    for(int column = 0; column < graph->nodesNumber; column++) { // Percorre as adjacências
        if(isConnected(graph, node, column)) { // Checa se existe aresta entre 'node' e 'column'
            if(colors[column] == 0) { // Checa se é branco
                predecessor[column] = node;
                visitDFS(graph, column, time, colors, foundTimer, endTimer, predecessor, f, tree, paths);
            }
        }
    }
    endTimer[node] = ++(*time);
    colors[node] = 2;
}