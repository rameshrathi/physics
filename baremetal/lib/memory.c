//
//  mem.c

#include <stdlib.h>
#include <sys/types.h>

#include "include/types.h"
#include "include/memory.h"

// copy n bytes from src to dest
void mem_copy(void *dest, const void *src, size_t len)
{
    // programmer should handle null pointer case
    // Copy byte by bte util any address aligned in multiple of 4
    while (len && ((size_t)dest % 4 || (size_t)src % 4)) {
        * (uint8_t*) dest++ = * (uint8_t*) src++;
        len--;
    }

    // copy bytes equal to word size
    while (len > sizeof(size_t)-1) {
        * (size_t*) dest = * (size_t*) src;
        dest += sizeof(size_t);
        src += sizeof(size_t);
        len -= sizeof(size_t);
    }
    // copy remaining of them byte by byte
    while (len--) {
        *(uint8_t*) dest++ = *(uint8_t*) src++;
    }
}


// set memory bits based on
void mem_set(void *dest, size_t bits, size_t len)
{
    if (!dest || len < 1)
        return;
    
    uint8_t ws = sizeof(size_t);
    len--; // move to last pos
    
    // copy bytes equal to word size
    while (len > ws) {
        *(size_t*) dest = bits;
        len -= ws;
    }
    // copy remaining of them byte by byte
    while (len) {
        *(uint8_t*) dest = bits;
        len--;
    }
}
