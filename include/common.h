#ifndef C_COMMON_H
#define C_COMMON_H
/*
 * common.h
 * Common definitions and inclusions.
 */

// Std includes
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

// POSIX name resolution
#if (defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)) && !defined(__INTEL_COMPILER)
#define memcpy_s(dest, dest_size, src, src_size) memcpy(dest, src, src_size);
#define memmove_s(dest, dest_size, src, src_size) memmove(dest, src, src_size);
#endif

#endif // C_COMMON_H