#ifndef CC_STD_UTILITIES_H
#define CC_STD_UTILITIES_H
/**
 * utilities.h
 * Useful global utility functions.
*/
#include <math.h>
#include <stdint.h>
#include <stdio.h>

/** Get the next power of 2 >= x. */
#define CC_NEXT_POW2(x) (1ULL << (unsigned long long)(log2((double)x-1.)+1.))

#if defined(_MSC_VER)

/** Get the larger of two numbers. */
#define CC_MAX(a,b) (((a) > (b)) ? a : b)

/** Get the lesser of two numbers. */
#define CC_MIN(a,b) (((a) < (b)) ? a : b)

#else

/** Get the larger of two numbers. */
#define CC_MAX(a,b) \
({ \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; \
})

/** Get the lesser of two numbers. */
#define CC_MIN(a,b) \
({ \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; \
})

#endif // defined(_MSC_VER)

// POSIX function name resolution
#if defined(_MSC_VER)
	#include <shlwapi.h>
	#define strdup _strdup
	#define strcasecmp StrCmpIW
#else
	#define printf_s printf
	#define vprintf_s vprintf
	#define fprintf_s fprintf
	#define vfprintf_s vfprintf
	#define memcpy_s(dest, dest_size, src, src_size) memcpy(dest, src, src_size)
	#define memmove_s(dest, dest_size, src, src_size) memmove(dest, src, src_size)
	#define strcat_s(dest, dest_size, src) strcat(dest, src)
	#define strcpy_s(dest, dest_size, src) strcpy(dest, src)
#endif // defined(_MSC_VER)

#endif