static int translate(
	addr_t virtual_addr, 	// Given virtual address
	addr_t * physical_addr, // Physical address to be returned
	struct pcb_t * proc) {  // Process uses given virtual address

	/* Offset of the virtual address */
	addr_t offset    = get_offset(virtual_addr);
	/* The first layer index */
	addr_t first_lv  = get_first_lv(virtual_addr);
	/* The second layer index */
	addr_t second_lv = get_second_lv(virtual_addr);
	
	/* Search in the first level */
	page_table_t* page_table = NULL;
	page_table = get_page_table(first_lv, proc->seg_table);
	if (page_table == NULL) return 0;

	for (int i = 0; i < page_table->size; i++)
		if (page_table->table[i].v_index == second_lv) {
			addr_t p_index = page_table->table[i].p_index;
			*physical_addr = (p_index << OFFSET_LEN) | offset;
			return 1;
	}

	return 0;	
}