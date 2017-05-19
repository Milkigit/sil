#ifdef BENCH_H_INCLUDED
#error bench.h included twice!
#endif

#include <stdio.h>

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

enum {
        /*
         * Add the given element from the input stream to the tree. Write to
         * the output stream whether the element was already in the tree.
         */
        BENCH_ACTION_ADD = 1,
        /*
         * Find the given element. Write to output stream whether the element
         * was found.
         */
        BENCH_ACTION_FIND = 2,
        /*
         * Remove the given element from the input stream to the tree. Write
         * to the output stream whether the element was already in the tree.
         */
        BENCH_ACTION_REMOVE = 3,
        /*
         * Calculate the hash sum of all elements currently in the tree
         * instead.
         */
        BENCH_ACTION_HASHSUM = 4,
};

struct action {
        /* BENCH_ACTION_ADD / BENCH_ACTION_REMOVE / BENCH_ACTION_HASHSUM */
        int action;
        /* for BENCH_ACTION_ADD / BENCH_ACTION_REMOVE */
        unsigned index;
};

struct treebenchfuncs {
        const char *benchname;
        void *(*init)(void);
        void (*exit)(void *self);
        void (*bench)(void *self, struct benchpayload *data, struct action *action, size_t ndata, size_t naction, unsigned *result);
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
static unsigned int BENCH_UNUSED
hash_benchdata(struct benchpayload *x)
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

/* for debugging */
static void BENCH_UNUSED
bench_payload_print(FILE *f, struct benchpayload *payload)
{
#if BENCH_PAYLOAD_TYPE == 0
        fprintf(f, "%d", payload->a);
#elif BENCH_PAYLOAD_TYPE == 1
        fprintf(f, "%lf %lf", payload->a, payload->b);
#elif BENCH_PAYLOAD_TYPE == 2
        fprintf(f, "%lf %lf %lf", payload->a, payload->b, payload->c);
#else
        fprintf(f, "%lf %lf %lf <7 more values..>", payload->a, payload->b, payload->c);
#endif
}

#ifdef __cplusplus
}
#endif
