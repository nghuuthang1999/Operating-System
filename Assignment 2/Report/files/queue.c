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
	unsigned int priority = q->proc[0]->priority;
	unsigned int index = 0;
	// Find element hold highest priotiry
	for (int i = 1; i < q->size; ++i){
		if (q->proc[i]->priority > priority){
			priority = q->proc[i]->priority;
			index = i;
		}
	}
	struct pcb_t * result = q->proc[index];
	
	for (int i = index + 1; i < q->size; ++i){
		q->proc[i - 1] = q->proc[i];
	}
	q->size--;

	return result;
}