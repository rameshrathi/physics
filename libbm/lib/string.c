//
//  str.c
//

#include "include/types.h"
#include "include/string.h"

// string copy
int str_copy(char *dst, const char *src)
{
    int total = (int) str_len(src);
    int count = 0;
    while (count + 4 < total) {
        *(int*)(dst+count) = *(int*)(src+count);
        count += 4;
    }
    while (count < total) {
        *(char*)(dst+count) = *(char*)(src+count);
        count++;
    }
    dst[count] = 0;
    return count;
}

// string length
size_t str_len(const char *src)
{
    size_t c = 0;
    while(*(src+c++) != '\0');
    return c-1;
}

int str_compare(const char *s1, const char *s2)
{
    int mis = 0;
    while (*s1 != '\0' || *s2 != '\0') {
        if (*s1++ != *s2++) mis++;
    }
    return mis;
}

void str_reverse(char *s)
{
    char *p = s;
    while ( *++p );
    p--; // move p 1 pos previous from current
    while (p > s)
    {
        char ch = *p;
        *p-- = *s;
        *s++ = ch;
    }
}
