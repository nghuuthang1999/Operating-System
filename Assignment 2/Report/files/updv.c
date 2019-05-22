...
	for(int i = 0; i < num_pages; ++i) {
		addr_t v_page_addr = virtual_addr + i*PAGE_SIZE;
		addr_t first_lv  = get_first_lv(v_page_addr);
		addr_t second_lv = get_second_lv(v_page_addr);

		page_table_t * page_table = get_page_table(first_lv, proc->seg_table);
		if(page_table == NULL) {puts("Error\n"); continue;}

		for(int j = 0; j < page_table->size; ++j)
			if(page_table->table[j].v_index == second_lv) {
				int last = --page_table->size;
				page_table->table[j] = page_table->table[last];
				break;
			}

		if(page_table->size == 0)
			remove_page_table(first_lv, proc->seg_table);
	}
	proc->bp = proc->bp - num_pages*PAGE_SIZE;
...