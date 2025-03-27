//
//  convert.c
//  assignment
//
//  Created by Ramesh on 07/03/25.
//

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
