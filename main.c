#include <stdio.h>
#include "graphmatrix.h"
#include "queue.h"

/* Code test of graph library */

int main() {
    FILE* filePointer = fopen("saida.txt", "w");
    fclose(filePointer);
    Graph graph;
    readGraph("entrada.txt", &graph);
    print(&graph);
    BFS(&graph);
    DFS(&graph);
}