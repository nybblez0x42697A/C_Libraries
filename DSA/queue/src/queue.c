#define INITIAL_CAPACITY 1024

#include "../include/queue.h"

// Structure to represent a position in the grid
typedef struct position {
    int row;
    int col;
    int elevation;
}position_t;

// Queue data structure for BFS
typedef struct queue_node_t {
    position_t pos; // node position within a grid. Added just to have for different implementations.
    queue_node_t *next;
}queue_node_t;

// Structure to represent a queue
typedef struct queue_t {
    void ** queue; // queue to hold data.
    position_t * elements;
    int front;  // Index of the front element in the queue
    int rear;  // Index of the next available position in the queue
    int size;  // Current number of elements in the queue
    int capacity;  // Maximum capacity of the queue
}queue_t;

// Function to create a queue
queue_t *queue_create(int capacity) {
    queue_t *queue = calloc(1, sizeof(queue_t));
    if (queue == NULL) {
        return NULL;
    }
    queue->capacity = capacity;
    queue->elements = calloc(queue->capacity, sizeof(queue_node_t));
    if (queue->elements == NULL) {
        free(queue);
        return NULL;
    }
    queue->front = 0;
    queue->rear = capacity - 1;
    queue->size = 0;
    return queue;
}

// Function to destroy a queue
void queue_destroy(queue_t **queue) {
    free((*queue)->elements);
    free(*queue);
    *queue = NULL;
}

// Function to check if a queue is empty
bool is_empty(queue_t *queue) {
    return (queue->size == 0);
}
bool is_full(queue_t * queue)
{
 return (queue->size == queue->capacity);
}

// Function to get the size of a queue
int get_size(queue_t *queue) {
    return queue->size;
}

// Function to add an element to the end of a queue
void enqueue(queue_t *queue, position_t element) {
    // Check if the queue is full
    if (queue->size == queue->capacity) {
        // Increase the capacity of the queue
        queue->capacity *= 2;
        queue->elements = realloc(queue->elements, queue->capacity * sizeof(position_t)); // maybe think about using a temp. Could lose pointer here and cause leaks.
        if (queue->elements == NULL) {
            // Failed to allocate memory, abort
            exit(1);
        }
    }
    // Add the element to the queue
    queue->elements[queue->front] = element;
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size++;
}

// Function to remove an element from the front of a queue
void * dequeue(queue_t *queue) {
    // Check if the queue is empty
    if (queue->size == 0) {
        // Return invalid
        return -1;
    }
    // Remove the front element from the queue
    void * element = queue->queue[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return element;
}

