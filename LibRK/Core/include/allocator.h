// handle memory allocation

// initialize the allocator
void init_allocator();

// allocate memory of n bytes
void * al_malloc(int n);

// free memory related to f
void al_free(int * p);
