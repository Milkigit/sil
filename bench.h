#ifdef BENCH_H_INCLUDED
#error bench.h included twice!
#endif

#define BENCH_H_INCLUDED

#include <stddef.h>

#define BENCH_UNUSED __attribute__((unused))
#define BENCH_PAYLOAD_TYPE 0

/* Helper functions */

void *xcalloc(size_t n, size_t s);
void xfree(void *);

/* bench interface */

struct benchpayload {
        /* can be changed. Be sure to adapt compare_benchpayload() as well */

#if BENCH_PAYLOAAD_TYPE == 0
        int a;
#else
        float a;
        float b;
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
#else
        if (x->a != y->a)
                return y->a - x->a;
        return y->b - x->b;
#endif
}

/* only used for asserting correctness of the implementations */
static unsigned int BENCH_UNUSED hash_benchdata(struct benchpayload *x)
{
#if BENCH_PAYLOAD_TYPE == 0
        return x->a;
#else
        return x->a * 33 + x->b;
#endif
}
