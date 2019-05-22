#ifndef COMMON_H
#define COMMON_H

/* define structs and routine could be used by every source files */

#include <stdint.h>

#define ADDRESS_SIZE 20
#define OFFSET_LEN	 10
#define SEGMENT_LEN	 5
#define PAGE_LEN	 5

#define NUM_PAGES	(1 << (ADDRESS_SIZE - OFFSET_LEN))
#define PAGE_SIZE	(1 << OFFSET_LEN)

typedef char     BYTE;
typedef uint32_t addr_t;

/* instructions opcode type */
typedef enum ins_opcode_t {
	CALC,	// just perform calculation, only use CPU
	ALLOC,	// allocate memory
	FREE,	// deallocated a memory block
	READ,	// write data to a byte on memory
	WRITE	// read data from a byte on memory
} ins_opcode_t;


/** - instructions executed by the CPU 
*   - arguments for each opcode:
* + calc		
* + alloc	[size] 	 [reg]
* + free	[reg]
* + read	[source] [offset] 	[dest]
* + write	[data] 	 [dest] 	[offset]
*/
typedef struct inst_t {
	// instructions opcode type
	ins_opcode_t opcode;
	// argument lists for instructions
	uint32_t arg_0, arg_1, arg_2;
} inst_t;

/* code segment type */
typedef struct code_seg_t {
	inst_t*  text;
	uint32_t size;
} code_seg_t;

/* page table type */
typedef struct page_table_t {
	/* a row in the page table of the second layer */
	struct  {
		addr_t v_index; // the index of virtual address, [page]
		addr_t p_index; // the index of physical address, [segment] + [page]
	} table[1 << PAGE_LEN];
	int size;
} page_table_t;

/* mapping virtual addresses and physical ones */
typedef struct seg_table_t {
	/* translation table for the first layer */
	struct {
		addr_t v_index;	// virtual index, [segment]
		struct page_table_t * pages;
	} table[1 << SEGMENT_LEN];
	int size;	// number of row in the first layer
} seg_table_t;

/* PCB, describe information about a process */
typedef struct pcb_t {
	uint32_t pid; 					// pid
	uint32_t priority; 				// priority of proccess
	struct code_seg_t * code;		// code segment
	addr_t regs[10]; 				// registers
	uint32_t pc; 	 				// program counter
	struct seg_table_t * seg_table; // segment table
	uint32_t bp;	 				// break pointer
} pcb_t;

#endif
