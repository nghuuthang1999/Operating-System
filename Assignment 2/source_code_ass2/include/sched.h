#ifndef SCHED_H
#define SCHED_H

#include "common.h"
#include <stdbool.h>

bool queue_empty(void);

void init_scheduler(void);
void finish_scheduler(void);

/* Get the next process from ready queue */
pcb_t* get_proc(void);

/* Put a process back to run queue */
void put_proc(pcb_t* proc);

/* Add a new process to ready queue */
void add_proc(pcb_t* proc);

#endif


