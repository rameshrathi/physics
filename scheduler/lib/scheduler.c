#include "scheduler.h"

// RP2350-specific registers (Cortex-M33)
#define SYSTICK_CTRL (*(volatile unsigned int *)0xE000E010)  // SysTick control and status
#define SYSTICK_LOAD (*(volatile unsigned int *)0xE000E014)  // SysTick reload value
#define SYSTICK_VAL  (*(volatile unsigned int *)0xE000E018)  // SysTick current value
#define NVIC_ICSR    (*(volatile unsigned int *)0xE000ED04)  // NVIC interrupt control and state

// System clock frequency (adjust based on RP2350 configuration)
#define CLOCK_FREQ 120000000  // 120 MHz default

// Static task array and scheduler state
static TaskControlBlock tasks[MAX_TASKS];
static int num_tasks = 0;      // Number of active tasks
static int current_task = -1;  // Index of the currently running task

// Stack allocation for each task (4KB per task)
// Task stacks in a dedicated section for striped SRAM0-3
__attribute__((section(".task_stacks")))
static unsigned int task_stacks[MAX_TASKS][1024];

// Initialize the scheduler with a configurable time slice
void scheduler_init(unsigned int time_slice_ms) {
    // Calculate SysTick reload value based on clock frequency and time slice
    unsigned int ticks = (CLOCK_FREQ / 1000) * time_slice_ms;
    SYSTICK_LOAD = ticks - 1;     // Set reload value
    SYSTICK_VAL = 0;              // Reset current value
    SYSTICK_CTRL = 0x7;           // Enable SysTick, interrupt, use processor clock

    // Initialize task array
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].task = 0;
        tasks[i].stack = 0;
        tasks[i].active = 0;
    }
}

// Add a task with an argument to the scheduler
void scheduler_add_task(TaskFunc task, void *arg) {
    if (num_tasks < MAX_TASKS) {
        tasks[num_tasks].task = task;
        // Set stack pointer to top of stack minus space for context (16 words)
        unsigned int *sp = &task_stacks[num_tasks][1023 - 16];
        tasks[num_tasks].stack = sp;
        tasks[num_tasks].active = 1;

        // Initialize R4-R11 to 0 for consistency
        for (int i = 0; i < 8; i++) {
            *sp++ = 0;  // R4-R11
        }
        // Initialize hardware-saved context
        *sp++ = (unsigned int)arg;  // R0: task argument
        *sp++ = 0;                  // R1
        *sp++ = 0;                  // R2
        *sp++ = 0;                  // R3
        *sp++ = 0;                  // R12
        *sp++ = 0;                  // LR (return address, unused initially)
        *sp++ = (unsigned int)task; // PC: task entry point
        *sp++ = 0x01000000;         // xPSR: Thumb state enabled
        num_tasks++;
    }
}

// Start the scheduler by setting up the first task and triggering context switch
void scheduler_start(void) {
    if (num_tasks > 0) {
        current_task = 0;
        // Set Process Stack Pointer (PSP) to the first task's stack
        __asm volatile (
            "ldr r0, =tasks\n"
            "ldr r1, =current_task\n"
            "ldr r1, [r1]\n"
            "mov r2, #12\n"          // Size of TaskControlBlock
            "mla r0, r1, r2, r0\n"   // Calculate address of tasks[current_task]
            "ldr r0, [r0, #4]\n"     // Load tasks[current_task].stack
            "msr psp, r0\n"          // Set PSP
            ::: "r0", "r1", "r2"     // Clobbered registers
        );
        // Trigger PendSV to start the first task
        NVIC_ICSR = 0x10000000;  // Set PendSV pending bit
    }
}

// SysTick handler: triggers PendSV for context switching
__attribute__((naked)) void SysTick_Handler(void) {
    __asm volatile (
        "ldr r0, =0xE000ED04\n"  // Address of NVIC_ICSR
        "mov r1, #0x10000000\n"  // PendSV pending bit
        "str r1, [r0]\n"         // Set PendSV pending
        "bx lr\n"                // Return from interrupt
    );
}

// PendSV handler: performs context switching between tasks
__attribute__((naked)) void PendSV_Handler(void) {
    __asm volatile (
        "cpsid i\n"              // Disable interrupts
        "mrs r0, psp\n"          // Get current PSP
        "stmdb r0!, {r4-r11}\n"  // Save R4-R11 on task stack (decrement before store)
        "ldr r1, =current_task\n"
        "ldr r2, [r1]\n"         // Load current_task index
        "ldr r3, =tasks\n"
        "mov r4, #12\n"          // Size of TaskControlBlock
        "mla r3, r2, r4, r3\n"   // Calculate address of tasks[current_task]
        "str r0, [r3, #4]\n"     // Save updated PSP in TCB

        // Select next task (round-robin)
        "add r2, r2, #1\n"       // Increment task index
        "ldr r4, =num_tasks\n"
        "ldr r4, [r4]\n"         // Load number of tasks
        "cmp r2, r4\n"           // Compare with num_tasks
        "it ge\n"                // If greater or equal
        "movge r2, #0\n"         // Reset to 0
        "str r2, [r1]\n"         // Update current_task

        // Load next task's PSP
        "ldr r3, =tasks\n"
        "mla r3, r2, r4, r3\n"   // Calculate address of tasks[next_task]
        "ldr r0, [r3, #4]\n"     // Load next task's PSP

        // Restore next task's context
        "ldmia r0!, {r4-r11}\n"  // Restore R4-R11 (increment after load)
        "msr psp, r0\n"          // Update PSP to point to hardware context
        "cpsie i\n"              // Enable interrupts
        "mov lr, #0xFFFFFFFD\n"  // EXC_RETURN: return to Thread mode with PSP
        "bx lr\n"                // Return from exception
    );
}

/*
Registers: Uses Cortex-M33 SysTick and NVIC registers for timing and interrupts.

Task Stacks: Each task gets a 4KB stack, initialized with a basic context (xPSR, PC, etc.).

scheduler_init: Sets up SysTick for 1ms interrupts.

scheduler_add_task: Adds a task with an initialized stack for context switching.

scheduler_start: Starts the first task by loading its context.

SysTick_Handler: Preempts the current task every 1ms, saves its context, and switches to the next task.
It uses the Process Stack Pointer (PSP) for task stacks, saves/restores R4-R11 (callee-saved registers), and handles task switching. 
The naked attribute ensures no prologue/epilogue is added by the compiler.

Context Switching: Saves the current task’s stack pointer, updates current_task, and restores the next task’s context.

*/