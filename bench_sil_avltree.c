#include <stdio.h>

#include "bench.h"
#include "bench_sil_avltree.h"

/************************************
 * SIL AVLTREE bench implementation *
 ************************************/

/* include a first time to get the head definition */
#define SIL_AVL_ARG_NAME SIL_AVLtree
#define SIL_AVL_ARG_TYPE struct sil_node
#define SIL_AVL_ARG_MEMBER head
#define SIL_AVL_ARG_COMPARE compare_sil_node
#define SIL_AVL_ARG_PACKHEAD 1 /* XXX */
#include "avltree/sil_avltree.h"

struct sil_node {
        struct SIL_AVLhead __attribute((aligned(sizeof (void *)))) head;
        struct benchpayload payload;
};

int compare_sil_node(struct sil_node *a, struct sil_node *b)
{
        return compare_benchpayload(&a->payload, &b->payload);
}

/* include a second time to generate */
#include "avltree/sil_avltree.h"

struct sil_state {
        struct SIL_AVLtree tree;
        struct sil_node *nodes;
        size_t nnodes;
};

void *sil_init(void)
{
        struct sil_state *state;
        state = xcalloc(1, sizeof *state);
        SIL_AVLtree_init(&state->tree);
        return state;
}

void sil_exit(void *self)
{
        struct sil_state *state = self;
        SIL_AVLtree_exit(&state->tree);
        xfree(state->nodes);
        xfree(state);
}

static void sil_bench(void *self, struct benchpayload *data, struct action *action, size_t ndata, size_t naction, unsigned *result)
{
        struct sil_state *state;
        struct sil_node *datanodes;
        struct sil_node *node;
        size_t i;
        unsigned r;

        (void) ndata;

        datanodes = xcalloc(naction, sizeof *datanodes);

        state = self;
        for (i = 0; i < naction; i++) {
                switch(action[i].action) {
                case BENCH_ACTION_ADD:
                        datanodes[i].payload = data[action[i].index];
			r = !!SIL_AVLtree_insert(&state->tree, &datanodes[i]);
                        break;
                case BENCH_ACTION_FIND:
			r = !!SIL_AVLtree_find(&state->tree, &datanodes[i]);
                        break;
                case BENCH_ACTION_REMOVE:
			node = SIL_AVLtree_find(&state->tree, &datanodes[i]);
			if (node)
				SIL_AVLtree_erase(node);
			r = !!node;
                        break;
                case BENCH_ACTION_HASHSUM:
			/* TODO */
                        r = 0;
                        break;
                default:
                        r = 0;
                }
                result[i] = r;
        }

        xfree(datanodes);
}

struct treebenchfuncs bench_silavl_funcs = {
        "SIL AVLtree",
        sil_init,
        sil_exit,
        sil_bench,
};

#undef NAME
#undef TYPE
#undef MEMBER
#undef COMPARE
