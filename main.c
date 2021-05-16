#include <stdio.h>
#include "graphmatrix.h"

/* Code test of graph library */

int main() {
    Graph graph;
    int verticeNumber = 5;

    readGraph("entrada.txt", &graph);
    print(&graph);
    DFS(&graph);
}