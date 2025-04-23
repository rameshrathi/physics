#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include "thread_mgr.h"
#include "buffer_mgr.h"
#include "http_client.h"

#define HOST "http://127.0.0.1:8080/api/v1/"

void create_threads(ThreadFunction func, int thread_count) {
    pthread_t threads[thread_count];

    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&threads[i], NULL, func, (void*)(long)i) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }
    pthread_cond_t cond1;
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
}

// Thread function
void* thread_function(void* arg) {
    long thread_id = (long)arg;

    // Get a buffer
    Buffer* buffer = get_free_buffer();
    if (!buffer) {
        fprintf(stderr, "Thread %ld: No free buffer available\n", thread_id);
        return NULL;
    }

    // Prepare HTTP request
    snprintf(buffer->buffer, BUFFER_SIZE,
             "GET / HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             HOST);

    // Send HTTP request
    send_http_request(HOST, buffer->buffer, strlen(buffer->buffer));

    // Release the buffer
    release_buffer(buffer);

    return NULL;
}
