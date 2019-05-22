static page_table_t* get_page_table(
	addr_t index, 			// segment level index
	seg_table_t* seg_table) // first level table
{	
	if(seg_table == NULL) 
		return NULL;
	
	for(int i = 0; i < seg_table->size; ++i)
		if(seg_table->table[i].v_index == index)
			return seg_table->table[i].pages;

	return NULL;
}