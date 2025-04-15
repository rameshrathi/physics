#include "buffer_mgr.h"
#include "http_client.h"
#include "thread_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
