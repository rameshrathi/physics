
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "lib/include/bmlib.h"

#define FILE_NAME "Package.swift"

int main()
{
	int fd;
	fd = open(FILE_NAME, O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(1);
	}
	
	printf("\n");
    return 0;

}
