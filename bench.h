#ifdef BENCH_H_INCLUDED
#error bench.h included twice!
#endif

#define BENCH_H_INCLUDED

#include <stddef.h>

#define BENCH_UNUSED __attribute__((unused))

/* Helper functions */

void *xcalloc(size_t n, size_t s);
void xfree(void *);

/* bench interface */

#define PAYLOAD_TYPE 0

struct benchpayload {
        /* can be changed. Be sure to adapt compare_benchpayload() as well */

#if PAYLOAAD_TYPE == 0
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
};

static int BENCH_UNUSED compare_benchpayload(struct benchpayload *x, struct benchpayload *y)
{
#if PAYLOAD_TYPE == 0
        return y->a - x->a;
#else
        if (x->a != y->a)
                return y->a - x->a;
        return y->b - x->b;
#endif
}
