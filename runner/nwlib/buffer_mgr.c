#include "buffer_mgr.h"

// Static array of buffers
static Buffer buffers[BUFFER_COUNT];

// Initialize buffers
void initialize_buffers() {
    for (int i = 0; i < BUFFER_COUNT; i++) {
        buffers[i].is_available = 1;
    }
}

// Get a free buffer
Buffer* get_free_buffer() {
    for (int i = 0; i < BUFFER_COUNT; i++) {
        if (buffers[i].is_available) {
            buffers[i].is_available = 0;
            return &buffers[i];
        }
    }
    return NULL; // No free buffer available
}

// Release a buffer
void release_buffer(Buffer* buffer) {
    buffer->is_available = 1;
}
