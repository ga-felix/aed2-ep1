#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
 
// ABAIXO OPERAÇÕES BÁSICAS DE FILA
 
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
 
int first(Queue* queue) {
    if (isEmpty(queue)) return -1;
    return queue->array[queue->first];
}
 
int last(Queue* queue) {
    if (isEmpty(queue)) return -1;
    return queue->array[queue->last];
}