#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <pthread.h>

// Thread function
typedef void* (*ThreadFunction)(void*);

void* thread_function(void* arg);

// Create threads
void create_threads(ThreadFunction func, int thread_count);

#endif
