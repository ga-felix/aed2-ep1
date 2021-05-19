#include <stdio.h>
#include "graphmatrix.h"
#include "queue.h"

/* Code test of graph library */

int main() {
    Graph graph;
    readGraph("entrada.txt", &graph);
    print(&graph);
    BFS(&graph);
    DFS(&graph);
}