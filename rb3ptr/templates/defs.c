/*
 * ---------------------------------------------------------------------------
 * Preprocessor definitions
 *
 * This file contains preprocessor definitions used across the project. They
 * are implementation details (not part of the user-facing API).
 * ---------------------------------------------------------------------------
 */

/* don't want no assert.h dependency */
#ifndef RB3_DEBUG
#define _RB3_ASSERT(cond)
#else
#include <assert.h>
#define _RB3_ASSERT assert
#endif

/* don't want no stddef.h dependency */
#ifdef __cplusplus
#define _RB3_NULL 0
#else
#define _RB3_NULL ((void *)0)
#endif
#define _RB3_offsetof(st, m) ((char *)&(((st *)0)->m)-((char *)0))

/* only used in impl */
#define _RB3_DIR_BIT (1<<0)
#define _RB3_COLOR_BIT (1<<1)
#define _RB3_BLACK (0)
#define _RB3_RED (_RB3_COLOR_BIT)
#define _RB3_CHILD_PTR(head, color) ((rb3_ptr)(head) | color)
#define _RB3_PARENT_PTR(head, dir) ((rb3_ptr)(head) | dir)

/*
 * Function attributes. This currently needs "inline", but works both in C99
 * and GNU89 inline models (only C99 supported!).
 */
#define _RB3_API_STATIC_INLINE static inline __attribute__((unused))
#define _RB3_INLINE inline __attribute__((always_inline))
#define _RB3_NEVERINLINE __attribute__((noinline))
#define _RB3_COLD __attribute__((cold))

/*
 * Depending on whether the generic implementation is built as a linkable
 * object or statically in a single file, the meaning of _RB3_API can be either
 * of these two:
 */
#define _RB3_API_EXTERNIMPL extern
#define _RB3_API_STATICIMPL static __attribute__((unused))
