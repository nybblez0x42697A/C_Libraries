/** @file queue.h
* 
* @brief A description of the module's purpose. *****CHANGE ME*****
*
*/
 
#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct position position_t;
typedef struct queue_node_s queue_node_t;
typedef struct queue_s queue_t;

 // Prototypes for the queue functions
queue_t *queue_create(int capacity);
bool is_empty(queue_t *queue);
bool is_full(queue_t * queue);
int get_size(queue_t *queue);
void enqueue(queue_t *queue, position_t element);
// void queue_destroy(queue_t **queue);
void * dequeue(queue_t *queue);

#endif
