#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <stdlib.h>

#define BUFFER_COUNT 10
#define BUFFER_SIZE 1024

// Buffer structure
typedef struct {
    char buffer[BUFFER_SIZE];
    int is_available; // 1 if available, 0 if in use
} Buffer;

// Initialize buffers
void initialize_buffers();

// Get a free buffer
Buffer* get_free_buffer();

// Release a buffer
void release_buffer(Buffer* buffer);

#endif
