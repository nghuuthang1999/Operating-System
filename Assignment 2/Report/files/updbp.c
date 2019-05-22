static int remove_page_table(
	addr_t index, 			// segment level index
	seg_table_t* seg_table) // first level table
{
	if(seg_table == NULL) return 0;

	for(int i = 0; i < seg_table->size; ++i)
	if(seg_table->table[i].v_index == index) {
		int idx = --seg_table->size;
		seg_table->table[i] = seg_table->table[idx];
		seg_table->table[idx].v_index = 0;
		free(seg_table->table[idx].pages);
		return 1;
	}
	return 0;
}