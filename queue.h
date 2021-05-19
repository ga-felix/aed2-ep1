#include <stdbool.h>

typedef struct {
    int first, last, size;
    unsigned capacity;
    int* array;
} Queue;

 
Queue* initializeQueue(unsigned capacity);

bool isFull(Queue* queue);

bool isEmpty(Queue* queue);

void insertQueue(Queue* queue, int item);

int dequeue(Queue* queue);

int first(Queue* queue);

int last(Queue* queue);