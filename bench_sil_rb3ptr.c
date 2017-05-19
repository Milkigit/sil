#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* for debugging */

#include "bench.h"
#include "bench_sil_rb3ptr.h"

#include "rb3ptr/build/include/rb3ptr.h"

/********************
 * Bench for rb3ptr *
 ********************/

RB3_GEN_IMPL_STATIC();

struct bench_rb3_node {
        struct rb3_head head;
        struct benchpayload payload;
};


static struct bench_rb3_node *bench_rb3_get_node(struct rb3_head *head)
{
        return (struct bench_rb3_node *) ((char *)head - offsetof(struct bench_rb3_node, head));
}

static struct rb3_head *bench_rb3_get_head(struct bench_rb3_node *node)
{
        return &node->head;
}

static int comparison(struct bench_rb3_node *a, struct bench_rb3_node *b)
{
        return compare_benchpayload(&a->payload, &b->payload);
}

RB3_GEN_TREE_DEFINITION(bench_rb3tree);
RB3_GEN_INLINE_PROTO_STATIC(bench_rb3tree, struct bench_rb3_node, bench_rb3_get_head, bench_rb3_get_node);
RB3_GEN_NODECMP_PROTO_STATIC(bench_rb3tree, /* no suffix */, struct bench_rb3_node, bench_rb3_get_head, bench_rb3_get_node, comparison);
RB3_GEN_NODECMP_STATIC(bench_rb3tree, /* no suffix */, struct bench_rb3_node, bench_rb3_get_head, bench_rb3_get_node, comparison);

struct bench_rb3_state {
        struct bench_rb3tree tree;
        struct bench_rb3_node *nodes;
        size_t nnodes;
};

static void *bench_rb3_init(void)
{
        struct bench_rb3_state *state;
        state = xcalloc(1, sizeof *state);
        bench_rb3tree_init(&state->tree);
        return state;
}

static void bench_rb3_exit(void *self)
{
        struct bench_rb3_state *state = self;
        xfree(state->nodes);
        xfree(state);
        bench_rb3tree_exit(&state->tree);
}

static void bench_rb3_bench(void *self, struct benchpayload *data, struct action *action, size_t ndata, size_t naction, unsigned *result)
{
        struct bench_rb3_state *state;
        struct bench_rb3_node *datanodes;
        struct bench_rb3_node *node;
        size_t i;
        unsigned r;

        (void) ndata;

        datanodes = xcalloc(naction, sizeof *datanodes);

        state = self;
        for (i = 0; i < naction; i++) {
                switch(action[i].action) {
                case BENCH_ACTION_ADD:
                        datanodes[i].payload = data[action[i].index];
                        r = !!bench_rb3tree_insert(&state->tree, &datanodes[i]);
                        break;
                case BENCH_ACTION_FIND:
                        datanodes[i].payload = data[action[i].index];
                        r = !!bench_rb3tree_find(&state->tree, &datanodes[i]);
                        break;
                case BENCH_ACTION_REMOVE:
                        datanodes[i].payload = data[action[i].index];
                        node = bench_rb3tree_delete(&state->tree, &datanodes[i]);
                        r = !!node;
                        break;
                case BENCH_ACTION_HASHSUM:
                        r = 0;
                        RB3_FOREACH(bench_rb3tree, &state->tree, node)
                                r += hash_benchdata(&node->payload);
                        break;
                default:
                        r = 0;
                }
                result[i] = r;
        }

        xfree(datanodes);
}

struct treebenchfuncs bench_rb3_funcs = {
        "SIL 3-pointer Red-black tree",
        bench_rb3_init,
        bench_rb3_exit,
        bench_rb3_bench,
};
