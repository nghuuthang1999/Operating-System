
#include "queue.h"
#include "sched.h"
#include <pthread.h>
#include <stdlib.h>

static struct queue_t ready_queue;
static struct queue_t run_queue;
static pthread_mutex_t queue_lock;

int queue_empty(void) {
	return (empty(&ready_queue) && empty(&run_queue));
}

void init_scheduler(void) {
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}

void put_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&run_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&ready_queue, proc);
	pthread_mutex_unlock(&queue_lock);	
}

struct pcb_t * get_proc(void) {
	struct pcb_t * proc = NULL;
	/*TODO: get a process from [ready_queue]. If ready queue
	 * is empty, push all processes in [run_queue] back to
	 * [ready_queue] and return the highest priority one.
	 * Remember to use lock to protect the queue.
	 * */
	
	if (queue_empty())
		return NULL;
	struct pcb_t * temp = NULL;
	if (empty(&ready_queue)) {
		while (!empty(&run_queue)) {
			temp = dequeue(&run_queue);
			add_proc(temp);
		}
	}
	proc = dequeue(&ready_queue);
	put_proc(proc);

	temp = NULL;
	free(temp);
	return proc;
}




