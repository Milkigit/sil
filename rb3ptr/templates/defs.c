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
#define RB3_ASSERT(cond)
#else
#include <assert.h>
#define RB3_ASSERT assert
#endif

/* don't want no stddef.h dependency */
#ifdef __cplusplus
#define RB3_NULL 0
#else
#define RB3_NULL ((void *)0)
#endif
#define RB3_offsetof(st, m) ((char *)&(((st *)0)->m)-((char *)0))

/* only used in impl */
#define RB3_DIR_BIT (1<<0)
#define RB3_COLOR_BIT (1<<1)
#define RB3_BLACK (0)
#define RB3_RED (RB3_COLOR_BIT)
#define RB3_CHILD_PTR(head, color) ((rb3_ptr)(head) | color)
#define RB3_PARENT_PTR(head, dir) ((rb3_ptr)(head) | dir)

/*
 * Function attributes. This currently needs "inline", but works both in C99
 * and GNU89 inline models (only C99 supported!).
 */
#define RB3_API_STATIC_INLINE static inline __attribute__((unused))
#define RB3_INLINE inline __attribute__((always_inline))
#define RB3_NEVERINLINE __attribute__((noinline))
#define RB3_COLD __attribute__((cold))

/*
 * Depending on whether the generic implementation is built as a linkable
 * object or statically in a single file, the meaning of RB3_API can be either
 * of these two:
 */
#define RB3_API_EXTERNIMPL extern
#define RB3_API_STATICIMPL static __attribute__((unused))
