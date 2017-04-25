#ifdef BENCH_H_INCLUDED
#error bench.h included twice!
#endif

#if !(defined(BENCH_PAYLOAD_TYPE))
#error BENCH_PAYLOAD_TYPE undefined!
#endif

#if BENCH_PAYLOAD_TYPE < 0 || BENCH_PAYLOAD_TYPE > 2
#error Valid values for BENCH_PAYLOAD_TYPE are 0, 1, and 2!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BENCH_H_INCLUDED

#include <stddef.h>

#define BENCH_UNUSED __attribute__((unused))

/* Helper functions */

void *xcalloc(size_t n, size_t s);
void xfree(void *);

/* bench interface */

struct benchpayload {
#if BENCH_PAYLOAD_TYPE == 0
        int a;
#elif BENCH_PAYLOAD_TYPE == 1
        double a;
        double b;
#elif BENCH_PAYLOAD_TYPE == 2
        double a;
        double b;
        double c;
#else
        double a;
        double b;
        double c;
        double d;
        double e;
        double f;
        double g;
        double h;
        double i;
        double j;
#endif
};

struct treebenchfuncs {
        const char *benchname;
        void *(*init)(void);
        void (*exit)(void *self);
        void (*insertbench)(void *self, struct benchpayload *data, size_t n);
        void (*retrievebench)(void *self, struct benchpayload *data, size_t n);
        void (*removebench)(void *self, struct benchpayload *data, size_t n);
        void (*addelems)(void *self, size_t *out_count, unsigned *out_sumofhashes);
};

static int BENCH_UNUSED compare_benchpayload(struct benchpayload *x, struct benchpayload *y)
{
#if BENCH_PAYLOAD_TYPE == 0
        return y->a - x->a;
#elif BENCH_PAYLOAD_TYPE == 1
        if (x->a != y->a)
                return y->a - x->a;
        return y->b - x->b;
#elif BENCH_PAYLOAD_TYPE == 2
        if (x->a != y->a)
                return y->a - x->a;
        if (x->b != y->b)
                return y->b - x->b;
        return y->c - x->c;
#else
        if (x->a != y->a)
                return y->a - x->a;
        if (x->b != y->b)
                return y->b - x->b;
        if (x->c != y->c)
                return y->c - x->c;
        if (x->d != y->d)
                return y->d - x->d;
        if (x->e != y->e)
                return y->e - x->e;
        if (x->f != y->f)
                return y->f - x->f;
        if (x->g != y->g)
                return y->g - x->g;
        if (x->h != y->h)
                return y->h - x->h;
        return y->i - x->i;
#endif
}

/* only used for asserting correctness of the implementations */
static unsigned int BENCH_UNUSED hash_benchdata(struct benchpayload *x)
{
#if BENCH_PAYLOAD_TYPE == 0
        return x->a;
#elif BENCH_PAYLOAD_TYPE == 1
        return x->a * 33 + x->b;
#elif BENCH_PAYLOAD_TYPE == 2
        return (x->a * 33 + x->b) * 33 + x->c;
#else
        /* TODO */
        return (x->a * 33 + x->b) * 33 + x->c;
#endif
}

#ifdef __cplusplus
}
#endif
