//
//  convert.c
//  assignment
//

#include <stdlib.h>

#include "include/string.h"
#include "include/convert.h"

int str_to_int(const char *s)
{
    int r = 0;
    short neg = 0; // 1 for negative
    if (*s == '-')
    {
        neg = 1;
        s++;
    }
    while (*s <= '9' && *s >= '0') {
        r = (r * 10) + (*s - '0');
        s++;
    }
    if (neg) r = (r-1) ^ (-1); // 2's compliment
    return r;
}

int absolute(int val)
{
    return (val ^ -1)+1;
}

#define MAX_CHAR 9
/// Caller must deallocate the memory created for string
void int_to_str(int val, char *str)
{
	char s[MAX_CHAR];
	s[MAX_CHAR-1] = 0;
	char *t = s + MAX_CHAR-2;
	while (val) {
		int d = val % 10;
		*(t--) = (char)('0' + d);
		val /= 10;
	}
	str_copy(str, t+1);
	return str;
}

char * float_to_str(float val)
{
	char s[MAX_CHAR];
	s[MAX_CHAR-1] = 0; // string termination
	char *t = s + MAX_CHAR-2;
	
}
