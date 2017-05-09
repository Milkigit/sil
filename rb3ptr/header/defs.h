/* don't want no assert.h dependency */
#define RB3_ASSERT(cond)   /* TODO */

/* don't want no stddef.h dependency */
#define RB3_NULL ((void *)0)
#define RB3_offsetof(st, m) ((char *)&(((st *)0)->m)-((char *)0))

#define RB3_COLD __attribute__((cold))
#define RB3_INLINE inline __attribute__((always_inline))
#define RB3_NEVERINLINE __attribute__((noinline))

#define RB3_API extern
#define RB3_API_INLINE static inline __attribute__((unused))
#define RB3_API_STATIC_INLINE static inline  __attribute__((unused))
