// huffman coding
#include <stdio.h>

#include "include/lib.h"

int number_count(int num)
{
	int c = 0;
	while (num)
	{
		c++;
		num /= 10;
	}
	return c;
}

int compress_string( char *s )
{
	printf("Before compression = %s \n", s);

	int count = 0; // count number of chars

	char *p = s; // ptr
	while (p)
	{
		if (*p == '\0') break;
		while (*(p + ++count) == *p);
		if (count == 1)
		{
			p++;
			count = 0;
			continue;
		}
		if (count < 10)
		{
			*(++p) = count + '0';
			p += count;
		}
		else if (count < 100)
		{
			*(++p) = count/10 + '0';
			*(++p) = count%10 + '0';
			p += count-1;
		}
		
		count = 0;
	}
	
	
	// print
	printf("After compression = %s \n", s);
	return (int) (p - s);
}
