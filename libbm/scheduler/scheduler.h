#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_TASKS 10  // Maximum number of tasks (configurable by user)

// Task function pointer type, now accepts a void* argument for flexibility
typedef void (*TaskFunc)(void *arg);

// Task control block structure
typedef struct {
    TaskFunc task;         // Pointer to the task function
    unsigned int *stack;   // Stack pointer for the task
    int active;            // 1 if task is active, 0 if not
} TaskControlBlock;

// Scheduler API
void scheduler_init(unsigned int time_slice_ms); // Initialize with configurable time slice
void scheduler_add_task(TaskFunc task, void *arg); // Add a task with an argument
void scheduler_start(void);                      // Start the scheduler
void SysTick_Handler(void);                      // SysTick handler to trigger scheduling
void PendSV_Handler(void);                       // PendSV handler for context switching

#endif // SCHEDULER_H

/*
Defines a TaskControlBlock to store task information (function pointer, stack, and active status).
Provides a modular API to initialize, add tasks, and start the scheduler.
SysTick_Handler is declared here for the interrupt-driven context switch.
*/