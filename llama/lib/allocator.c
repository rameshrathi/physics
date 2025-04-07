//  allocator.c

#include "include/types.h"
#include "include/allocator.h"


#define HEAP_SIZE 1024


typedef struct Block {
	size_t size;
	int free;
	struct Block* next;
} Block;

static uint8_t _heap[HEAP_SIZE];  // 1024 bytes of heap
static Block * _list = (Block*) _heap;

void init_allocator()
{
	_list->size = HEAP_SIZE - sizeof(Block);
	_list->free = 1;
	_list->next = NULL;
}

// allocate memory of n bytes
void * al_malloc(int n)
{
	Block * current = _list;
	while (current) {
		if (current->free && current->size >= n) { // satisfy the requirements
			if (current->size >= n + sizeof(Block)) {
				Block * new = (Block*)((uint8_t*)current+sizeof(Block)+n);
				new->size = current->size - n - sizeof(Block);
				new->free = 1;
				new->next = current->next;
				current->next = new;
				current->size = n;
			}
			current->free = 0;
			return current + sizeof(Block);
		}
		current = current->next;
	}
	return NULL;
}

// free memory related to f
void al_free(int * p)
{
	if ( !p ) return;
	
	Block* block = (Block*)((uint8_t*)p - sizeof(Block)); // block
	block->free = 1;
	
	Block *curr = _list;
	while (curr) {
		// Merge current and next if both free
		if (curr->free && curr->next && curr->next->free) {
			curr->size += sizeof(Block) + curr->next->size;
			curr->next = curr->next->next;
		} else {
			curr = curr->next;
		}
	}
}
