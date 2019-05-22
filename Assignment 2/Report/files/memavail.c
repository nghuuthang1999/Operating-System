bool isMemAvail(int num_pages, struct pcb_t * proc) {
	// check physical space
	unsigned int empty_page = 0;

	for(int i = 0; i < NUM_PAGES; ++i)
		if(_mem_stat[i].proc == 0)
			if(++empty_page == num_pages)
				break; // enough memory

	if(empty_page < num_pages)
		return false;
	
	if(proc->bp + num_pages*PAGE_SIZE >= RAM_SIZE) 
		return false; // check virtual space

	return true;
}