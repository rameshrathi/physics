
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>

#include "lib/include/bmlib.h"

#define FILE_NAME "/Users/ramesh/Downloads/main.o"

int main(int argc, char *argv[])
{
	
	int sock_fd[2];
	pid_t pid;
	char buff[1024];
	
	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sock_fd) == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

    return 0;
}
