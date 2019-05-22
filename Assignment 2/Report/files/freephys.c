...
// find physical page in memory
if(!translate(virtual_addr, &physical_addr, proc)) return 1;

// clear physical page in memory
addr_t p_segm_page_index = physical_addr >> OFFSET_LEN;
int num_pages = 0;

for(int i = p_segm_page_index; i != -1; i = _mem_stat[i].next) {
	_mem_stat[i].proc = 0;		// clear physical memory
	num_pages = num_pages + 1; 	// count pages
}
...
