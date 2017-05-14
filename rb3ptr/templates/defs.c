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
#define RB3_NULL ((void *)0)
#define RB3_offsetof(st, m) ((char *)&(((st *)0)->m)-((char *)0))

/* only used in impl */
#define RB3_DIR_BIT (1<<0)
#define RB3_COLOR_BIT (1<<1)
#define RB3_BLACK (0)
#define RB3_RED (RB3_COLOR_BIT)
#define RB3_CHILD_PTR(head, color) ((rb3_ptr)(head) | color)
#define RB3_PARENT_PTR(head, dir) ((rb3_ptr)(head) | dir)


/* Function attributes used for extern linkage */
#define RB3_EXTERN_ATTRS  \
        extern,  \
        static inline __attribute__((unused)),  \
        __attribute__((cold)),  \
        inline __attribute__((always_inline)),  \
        __attribute__((noinline))

/* Function attributes used for static linkage */
#define RB3_STATIC_ATTRS  \
        static __attribute__((unused)),  \
        static inline __attribute__((unused)),  \
        __attribute__((cold)),  \
        inline __attribute__((always_inline)),  \
        __attribute__((noinline))
