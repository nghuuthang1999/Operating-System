pcb_t* get_proc(void) {
	pcb_t * proc = NULL;

	pthread_mutex_lock(&queue_lock);

	if(empty(&ready_queue))
	while(!empty(&run_queue)) {
		proc = dequeue(&run_queue);
		enqueue(&ready_queue, proc);
	}
	
	proc = dequeue(&ready_queue);

	pthread_mutex_unlock(&queue_lock);

	return proc;
}