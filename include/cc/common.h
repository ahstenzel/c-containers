/**
 * cc/common.h
 * Useful global utility functions.
*/
#ifndef CC_COMMON_H
#define CC_COMMON_H
#include <stdint.h>

/// @brief Get the next power of 2 >= x.
#ifndef CC_NEXT_POW2
#define CC_NEXT_POW2(x) (1ULL << (unsigned long long)(log2((double)x-1.)+1.))
#endif

#if defined(_MSC_VER)

/// @brief Get the larger of two numbers.
#ifndef CC_MAX
#define CC_MAX(a,b) (((a) > (b)) ? a : b)
#endif

/// @brief Get the lesser of two numbers.
#ifndef CC_MIN
#define CC_MIN(a,b) (((a) < (b)) ? a : b)
#endif

#else

/// @brief Get the larger of two numbers.
#ifndef CC_MAX
#define CC_MAX(a,b) \
({ \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; \
})
#endif

/// @brief Get the lesser of two numbers.
#ifndef CC_MIN
#define CC_MIN(a,b) \
({ \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; \
})
#endif

#endif // defined(_MSC_VER)

// Custom memory allocators
#ifndef CC_MALLOC
#define CC_MALLOC malloc
#endif
#ifndef CC_CALLOC
#define CC_CALLOC calloc
#endif
#ifndef CC_FREE
#define CC_FREE free
#endif

// POSIX function name resolution
#if defined(_MSC_VER)
	#include <shlwapi.h>
	#define strdup _strdup
	#define strcasecmp StrCmpIW
	#define strtok_r strtok_s
	#define itoa_s _itoa_s
#else
	#define printf_s printf
	#define vprintf_s vprintf
	#define fprintf_s fprintf
	#define vfprintf_s vfprintf
	#define memcpy_s(dest, dest_size, src, src_size) memcpy(dest, src, src_size)
	#define memmove_s(dest, dest_size, src, src_size) memmove(dest, src, src_size)
	#define strcat_s(dest, dest_size, src) strcat(dest, src)
	#define strcpy_s(dest, dest_size, src) strcpy(dest, src)
	#define itoa_s(value, dest, dest_size, radix) itoa(value, dest, radix)
#endif // defined(_MSC_VER)

#endif // CC_COMMON_H