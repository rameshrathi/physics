//
//  types.h
//

#ifndef types_h
#define types_h

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

/*  Commonly used types */

typedef     unsigned char           uint8_t;
typedef     unsigned short          uint16_t;
typedef     unsigned int            uint32_t;
typedef     unsigned long long      uint64_t;

typedef     short                   int16_t;
typedef     int                     int32_t;
typedef     long long               int64_t;


#if defined(__arm64__) || defined(__x64__)
typedef     unsigned long    size_t;    // For 64 bits machine
#else
typedef     unsigned int     size_t;    // For 32 bits machine
#endif

#endif
