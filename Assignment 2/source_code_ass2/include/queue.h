
#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"
#include <stdbool.h>

#define MAX_QUEUE_SIZE 10

/* priority queue */
typedef struct queue_t {
	pcb_t* proc[MAX_QUEUE_SIZE];
	int size;
} queue_t;

/* insert process to priority queue */
void   enqueue(queue_t* q, pcb_t* proc);

/* remove the hightest priority process */
pcb_t* dequeue(queue_t* q);

/* test whether queue is empty */
bool empty(queue_t* q);

/* test whether queue is full */
bool full(queue_t* q);

#endif

