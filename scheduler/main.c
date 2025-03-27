#include <stdint.h>
#include <stddef.h>

#include "lib/scheduler.h"

// Task 1: simple infinite loop with void* argument (unused here)
void task1(void *arg) {
    (void)arg;  // Suppress unused parameter warning
    volatile int i;
    while (1) {
        for (i = 0; i < 100000; i++);  // Simulate work
    }
}

// Task 2: another infinite loop with different timing
void task2(void *arg) {
    (void)arg;
    volatile int i;
    while (1) {
        for (i = 0; i < 50000; i++);  // Simulate work
    }
}

int main(void) {
    // Initialize scheduler with a 1ms time slice
    scheduler_init(1);
    // Add tasks with NULL arguments (arguments are optional)
    scheduler_add_task(task1, NULL);
    scheduler_add_task(task2, NULL);
    // Start the scheduler
    scheduler_start();
    while (1);  // Main thread should not exit
    return 0;
}