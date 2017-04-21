#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer/sil_timer.h"

#include "bench.h"
#include "bench_np_rbtree.h"
#include "bench_sil_avltree.h"
#include "bench_sil_rb2ptr.h"

/*****************************************************************/
/*****************************************************************/
void *xcalloc(size_t n, size_t s)
{
        void *ptr = calloc(n, s);
        if (!ptr) {
                perror("calloc()");
                exit(1);
        }
        return ptr;
}

void xfree(void *ptr)
{
        free(ptr);
}
/*****************************************************************/
/*****************************************************************/

struct benchtestdata {
        struct benchpayload *data;
        size_t n;
};

static void permute_benchdata(struct benchtestdata *bench)
{
        /* Knuth Shuffle */
        size_t i;
        printf("Randomly permuting elements\n");
        for (i = 0; i < bench->n; i++) {
                size_t j = rand() % (i+1);
                struct benchpayload tmp = bench->data[i];
                bench->data[i] = bench->data[j];
                bench->data[j] = tmp;
        }
}

static void benchtestdata_init(struct benchtestdata *bench)
{
        size_t i;
        size_t n;
        struct benchpayload *data;
        /*
         * Generate random permutations of the numbers 0..n
         *
         * Before that it generated n random numbers, but tree.h Red-Black-Tree
         * doesn't have "remove-if-exists"/"remove-by-iterator", so for a fair
         * comparison of remove performance we will simply avoid duplicates.
         */
        n = 1024*1024;
        /*
        n = 64*1024;
        n = 1024;
        */
        printf("Generating %zd elements\n", n);
        data = xcalloc(n, sizeof *data);
        for (i = 0; i < n; i++) {
#if PAYLOAD_TYPE == 0
                data[i].a = i;
#else
                data[i].a = i;
                data[i].b = i;
#endif
        }
        bench->n = n;
        bench->data = data;
}

static void benchtestdata_exit(struct benchtestdata *bench)
{
        free(bench->data);
        bench->data = NULL;
        bench->n = 0;
}

static void runbench(struct treebenchfuncs *impl, struct benchtestdata *bench)
{
        struct sil_timer timer;
        void *data;
        long ms;
        int s;
        size_t nodecount;
        unsigned nodesum;

        printf("Running bench %s\n", impl->benchname);
        printf("===========================\n");

        data = impl->init();

        sil_timer_reset(&timer);
        impl->insertbench(data, bench->data, bench->n);
        sil_timer_elapsed_s_ms(&timer, &s, &ms);
        printf("Inserting each element: %d.%.03lds\n", s, ms); fflush(stdout);

        impl->addelems(data, &nodecount, &nodesum);
        printf("DEBUG count: %zd, hashsum: %u\n", nodecount, nodesum); fflush(stdout);

        sil_timer_reset(&timer);
        impl->retrievebench(data, bench->data, bench->n);
        sil_timer_elapsed_s_ms(&timer, &s, &ms);
        printf("Finding each element: %d.%.03lds\n", s, ms); fflush(stdout);

        sil_timer_reset(&timer);
        impl->removebench(data, bench->data, bench->n);
        sil_timer_elapsed_s_ms(&timer, &s, &ms);
        printf("Removing each element: %d.%.03lds\n", s, ms); fflush(stdout);

        impl->addelems(data, &nodecount, &nodesum);
        printf("DEBUG count: %zd, hashsum: %u\n", nodecount, nodesum); fflush(stdout);

        impl->exit(data);

        printf("\n");
}

int main(void)
{
        struct benchtestdata bench;

        srand(time(NULL));
        benchtestdata_init(&bench);
        permute_benchdata(&bench);

        printf("\n");
        runbench(&np_rbtree_funcs, &bench);
        runbench(&sil_funcs, &bench);
        runbench(&silrb_funcs, &bench);
        benchtestdata_exit(&bench);

        return 0;
}
