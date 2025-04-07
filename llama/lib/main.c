
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <sys/syscall.h>

#include "include/lib.h"

const char * file_path = "num.txt";

int main(int argc, char *argv[])
{
	
	FILE *fp = fopen(file_path, "r");
	FILE *fp2 = fopen("res.txt", "w");
	if ( !fp || !fp2 )
	{
		perror("open");
		return;
	}

	// reach to end of file
	if (fseek(fp, 0, SEEK_END) != 0)
	{
		perror("seek");
	}
	// get file size
	long count = ftell(fp);
	printf("Length = %ld \n", count);
	
	// Move to last pos
	count--;
	
	// read file in reverse
	while ( count-- ) //reverse seek
	{
		fseek(fp, count, SEEK_SET);
		int ch = fgetc(fp);

		fputc(ch, fp2);
		printf("char = %c \n", ch);
	}
	
	printf("\n");
    return 0;
}
