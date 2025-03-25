//
//  memory.h
//

#ifndef memory_h
#define memory_h


void mem_copy(void *dest, const void *src, size_t len); // Param: dest, src, count
void mem_set(void *dest, size_t bits, size_t len);

#endif
