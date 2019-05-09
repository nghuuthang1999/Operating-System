#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
	return (q->size == 0);
}

int full(struct queue_t * q){
	return (q->size == MAX_QUEUE_SIZE);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
	if (full(q))
		return;
	q->proc[q->size++] = proc;
}


struct pcb_t * dequeue(struct queue_t * q) {
	// If queue empty 
	if(empty(q))
		return NULL;

	// If queue not empty
	struct pcb_t * result = NULL;
	unsigned int priority = q->proc[0]->priority;
	unsigned int index = 0;
	// Find element hold highest priotiry
	for (int i = 1; i < q->size; ++i){
		if (q->proc[i]->priority > priority){
			priority = q->proc[i]->priority;
			index = i;
		}
	}
	result = q->proc[index];
	// Decrease size of queue
	q->proc[index] =  q->proc[q->size - 1];
	q->size = q->size - 1;

	return result;
}

