#include "../include/mem.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

static BYTE _ram[RAM_SIZE]; // data on ram, each cell is 1 byte

/* NUM_PAGES: total page in memory = segments * pages */
static struct {
	uint32_t proc;	// ID of process currently uses this page
	int index;		// Index of the page in the list of pages allocated
	int next;		// The next page in the list. -1 if it is the last page.
} _mem_stat [NUM_PAGES];

static pthread_mutex_t mem_lock;
static pthread_mutex_t ram_lock;

void init_mem(void) {
	memset(_mem_stat, 0, sizeof(*_mem_stat) * NUM_PAGES);
	memset(_ram, 0, sizeof(BYTE) * RAM_SIZE);
	pthread_mutex_init(&mem_lock, NULL);
	pthread_mutex_init(&ram_lock, NULL);
}

/* get offset of the virtual address */
static addr_t get_offset(addr_t addr) {
	return addr & ~((~0U) << OFFSET_LEN);
}

/* get the first layer index */
static addr_t get_first_lv(addr_t addr) {
	return addr >> (OFFSET_LEN + PAGE_LEN);
}

/* get the second layer index */
static addr_t get_second_lv(addr_t addr) {
	return (addr >> OFFSET_LEN) - (get_first_lv(addr) << PAGE_LEN);
}

/* search for page table table from the a segment table */
static page_table_t* get_page_table(
	addr_t index, 			// segment level index
	seg_table_t* seg_table) // first level table
{	
	if(seg_table == NULL) return NULL;
	
	for(int i = 0; i < seg_table->size; ++i)
	if(seg_table->table[i].v_index == index)
		return seg_table->table[i].pages;

	return NULL;
}

/* remove page table table from the a segment table */
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

/* translate virtual address to physical address */
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

bool isMemAvail(int num_pages, struct pcb_t * proc) {
	// check physical space
	unsigned int free_page = 0;

	for(int i = 0; i < NUM_PAGES; ++i)
		if(_mem_stat[i].proc == 0)
		if(++free_page == num_pages)
			break; // enough memory

	if(free_page < num_pages) return false;
	
	if(proc->bp + num_pages*PAGE_SIZE >= RAM_SIZE) 
		return false; // check virtual space

	return true;
}

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

addr_t alloc_mem(uint32_t size, pcb_t* proc) {
	addr_t ret_mem = 0;

	// number of pages we will use
	uint32_t num_pages = ((size % PAGE_SIZE) == 0) ?
	        size / PAGE_SIZE : size / PAGE_SIZE + 1;
	
	if (isMemAvail(num_pages, proc)) {
		// first byte of new memory required for process
		ret_mem = proc->bp;
		// update break pointer for heap segment process
		proc->bp += num_pages * PAGE_SIZE;

		allocMemAvail(ret_mem, num_pages, proc);
	}

	return ret_mem;
}


int free_mem(addr_t address, struct pcb_t * proc) {
	addr_t virtual_addr  = address; // virtual address to free in process
	addr_t physical_addr = 0;		// physical address to free in memory

	// find physical page in memory
	if(!translate(virtual_addr, &physical_addr, proc)) return 1;

	// clear physical page in memory
	addr_t p_segm_page_index = physical_addr >> OFFSET_LEN;
	int num_pages = 0;

	for(int i = p_segm_page_index; i != -1; i = _mem_stat[i].next) {
		_mem_stat[i].proc = 0;		// clear physical memory
		num_pages = num_pages + 1; 	// count pages
	}

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

	return 0;
}

int read_mem(addr_t address, struct pcb_t * proc, BYTE * data) {
	addr_t physical_addr;
	if (translate(address, &physical_addr, proc)) {
		pthread_mutex_lock(&ram_lock);
		*data = _ram[physical_addr];
		pthread_mutex_unlock(&ram_lock);
		return 0;
	}else{
		return 1;
	}
}

int write_mem(addr_t address, struct pcb_t * proc, BYTE data) {
	addr_t physical_addr;
	if (translate(address, &physical_addr, proc)) {
		pthread_mutex_lock(&ram_lock);
		_ram[physical_addr] = data;
		pthread_mutex_unlock(&ram_lock);
		return 0;
	}else{
		return 1;
	}
}

void dump(void) {
	int i;
	for (i = 0; i < NUM_PAGES; i++) {
		if (_mem_stat[i].proc != 0) {
			printf("%03d: ", i);
			printf("%05x-%05x - PID: %02d (idx %03d, nxt: %03d)\n",
				i << OFFSET_LEN,
				((i + 1) << OFFSET_LEN) - 1,
				_mem_stat[i].proc,
				_mem_stat[i].index,
				_mem_stat[i].next
			);
			int j;
			for (	j = i << OFFSET_LEN;
				j < ((i+1) << OFFSET_LEN) - 1;
				j++) {
				
				if (_ram[j] != 0) {
					printf("\t%05x: %02x\n", j, _ram[j]);
				}
			}
		}
	}
}
