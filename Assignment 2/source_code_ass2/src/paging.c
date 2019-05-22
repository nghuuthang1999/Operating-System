#include "../include/mem.h"
#include "../include/cpu.h"
#include "../include/loader.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Cannot find input process\n");
		exit(1);
	}

	pcb_t* proc = load(argv[1]);

	for (unsigned int i = 0; i < proc->code->size; i++)
		run(proc);

	dump();
	return 0;
}
