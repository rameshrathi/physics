#include "buffer_mgr.h"
#include "http_client.h"
#include "thread_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/pipe.h>

// Startup of the process
void nw_start_process()
{
    // init buffer
    initialize_buffers();
    
    // start threads
    create_threads(thread_function, 10);
    
    // Keep the clients running
    while (1);
}

int start_main(int agrc, char *argv[])
{
    int fd_input = stdin;
    int fd_output = stdout;
    
    
}
