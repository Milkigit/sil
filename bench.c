#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "timer/sil_timer.h"
#include "bench.h"
#include "bench_np_rbtree.h"
#include "bench_sil_avltree.h"
#include "bench_sil_rb2ptr.h"
#include "bench_sil_rb3ptr.h"
#include "bench_stl_set.h"

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

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
        struct action *action;
        unsigned *reference_result;
        unsigned *result_area;
        size_t ndata;
        size_t naction;
};

static void benchtestdata_init(struct benchtestdata *bench)
{
        struct benchpayload *data;
        struct action *action;
        unsigned *reference_result;
        unsigned *result_area;
        size_t ndata;
        size_t naction;
        size_t i;
        size_t j;

        ndata = 32*1024;
        naction = ndata;

        printf("Generating %zd elements and %zd actions\n", ndata, naction);

        data = xcalloc(ndata, sizeof *data);
        action = xcalloc(naction, sizeof *action);
        reference_result = xcalloc(naction, sizeof *reference_result);
        result_area = xcalloc(naction, sizeof *result_area);

        for (i = 0; i < ndata; i++)
                data[i].a = i;
        /* Knuth Shuffle */
        for (i = 0; i < ndata; i++) {
                size_t j = rand() % (i+1);
                struct benchpayload tmp = data[i];
                data[i] = data[j];
                data[j] = tmp;
        }

        /* TODO: what's a good pattern (add/remove/find, what nodes?) */
        for (i = 0; i < naction / 2; i++) {
                action[i].action = BENCH_ACTION_ADD;
                action[i].index = rand() % ndata;
        }

        for (; i < naction; i += 128) {
                for (j = i; j < naction && j < i + 128; j++) {
                        switch (rand() % 4) {
                        case 0: action[j].action = BENCH_ACTION_ADD; break;
                        case 1: action[j].action = BENCH_ACTION_FIND; break;
                        case 2: action[j].action = BENCH_ACTION_REMOVE; break;
                        case 3: action[j].action = BENCH_ACTION_HASHSUM; break;
                        }
                        action[j].index = rand() % ndata;
                }
        }

        /* Get reference result from C++ implementation */
        printf("Generating reference result...\n");
        {
        void *stldata = bench_stlset_funcs.init();
        bench_stlset_funcs.bench(stldata, data, action, ndata, naction, reference_result);
        bench_stlset_funcs.exit(stldata);
        }

        bench->data = data;
        bench->action = action;
        bench->reference_result = reference_result;
        bench->result_area = result_area;
        bench->ndata = ndata;
        bench->naction = naction;
}

static void benchtestdata_exit(struct benchtestdata *bench)
{
        xfree(bench->data);
        xfree(bench->action);
        bench->data = NULL;
        bench->action = NULL;
        bench->ndata = 0;
        bench->naction = 0;
}

static void runbench(struct treebenchfuncs *impl, struct benchtestdata *bench)
{
        struct sil_timer timer;
        void *data;
        int s, ms;

        printf("Running bench %s\n", impl->benchname);
        printf("===========================\n");

        data = impl->init();

        memset(bench->result_area, 0, bench->naction * sizeof *bench->result_area);
        sil_timer_reset(&timer);
        impl->bench(data, bench->data, bench->action, bench->ndata, bench->naction, bench->result_area);
        sil_timer_elapsed_s_ms(&timer, &s, &ms);
        printf("Running stresstest: %d.%.03ds\n", s, ms);

        for (size_t i = 0; i < bench->naction; i++) {
                if (bench->result_area[i] != bench->reference_result[i]) {
                        printf("ERROR from stresstest at position %zd\n", i);
                        break;
                }
        }

        impl->exit(data);

        printf("\n");
}

int main(void)
{
        struct benchtestdata bench;

        printf("BENCH_PAYLOAD_TYPE=" STRINGIFY(BENCH_PAYLOAD_TYPE) "\n\n");

        srand(time(NULL));
        benchtestdata_init(&bench);

        printf("\n");
        runbench(&bench_rb3_funcs, &bench);
        runbench(&bench_stlset_funcs, &bench);
        runbench(&bench_np_rbtree_funcs, &bench);
        runbench(&bench_silavl_funcs, &bench);
        runbench(&bench_rb2_funcs, &bench);
        benchtestdata_exit(&bench);

        return 0;
}
