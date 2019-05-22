void allocMemAvail(int ret_mem, int num_pages, pcb_t* proc) {
	int count_alloc_pages = 0;
	int last_alloc_page_index = -1;

	pthread_mutex_lock(&mem_lock);
	for(int i = 0; i < NUM_PAGES; ++i) {
		if(_mem_stat[i].proc != 0) continue; // page is used

		_mem_stat[i].proc = proc->pid;
		_mem_stat[i].index = count_alloc_pages;

		// if not initial page, update "next" field
		if(last_alloc_page_index > -1) 
			_mem_stat[last_alloc_page_index].next = i;
		// update last page index
		last_alloc_page_index = i;

		addr_t virtual_addr = ret_mem + count_alloc_pages*PAGE_SIZE;
		addr_t first_lv     = get_first_lv(virtual_addr);
		addr_t second_lv    = get_second_lv(virtual_addr);

		page_table_t* page_table = get_page_table(first_lv, proc->seg_table);

		if(page_table == NULL) {
			int idx = proc->seg_table->size++;
			proc->seg_table->table[idx].v_index = first_lv;
			page_table
				= proc->seg_table->table[idx].pages
				= (page_table_t*)malloc(sizeof(page_table_t));
			page_table->size = 0;
		}

		int idx = page_table->size++;
		page_table->table[idx].v_index = second_lv;
		page_table->table[idx].p_index = i;

		if(++count_alloc_pages == num_pages)
			{_mem_stat[i].next = -1; break;}
	}
	pthread_mutex_unlock(&mem_lock);
}