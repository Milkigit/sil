#include <bsd/sys/tree.h>  /* BSD Red-black tree */
#include "bench.h"


struct np_rbtree_node {
        RB_ENTRY(np_rbtree_node) entry_;
        struct benchpayload payload;
};

int compare_rbtree_node(struct np_rbtree_node *a, struct np_rbtree_node *b)
{
        return compare_benchpayload(&a->payload, &b->payload);
}

/* declare structure of head */
RB_HEAD(RBtree, np_rbtree_node);
RB_GENERATE(RBtree, np_rbtree_node, entry_, compare_rbtree_node);

struct np_rbtree_state {
        struct RBtree tree;
        struct np_rbtree_node *nodes;
        size_t nnodes;
};

void *np_rbtree_init(void)
{
        struct np_rbtree_state *state;
        state = xcalloc(1, sizeof *state);
        state->tree = (struct RBtree) RB_INITIALIZER(&state->tree);
        return state;
}

void np_rbtree_exit(void *self)
{
        struct np_rbtree_state *state = self;
        /* TODO: how to release resources? */
        xfree(state->nodes);
        xfree(state);
}

static void np_rbtree_bench(void *self, struct benchpayload *data, struct action *action, size_t ndata, size_t naction, unsigned *result)
{
        struct np_rbtree_state *state;
        struct np_rbtree_node *datanodes;
        struct np_rbtree_node *node;
        size_t i;
        unsigned r;

        (void) ndata;

        datanodes = xcalloc(naction, sizeof *datanodes);

        state = self;
        for (i = 0; i < naction; i++) {
                switch(action[i].action) {
                case BENCH_ACTION_ADD:
                        datanodes[i].payload = data[action[i].index];
                        r = !!RB_INSERT(RBtree, &state->tree, &datanodes[i]);
                        break;
                case BENCH_ACTION_FIND:
                        datanodes[i].payload = data[action[i].index];
                        r = !!RB_FIND(RBtree, &state->tree, &datanodes[i]);
                        break;
                case BENCH_ACTION_REMOVE:
                        datanodes[i].payload = data[action[i].index];
                        node = RB_FIND(RBtree, &state->tree, &datanodes[i]);
                        if (node)
                                RB_REMOVE(RBtree, &state->tree, node);
                        r = !!node;
                        break;
                case BENCH_ACTION_HASHSUM:
                        r = 0;
                        RB_FOREACH(node, RBtree, &state->tree)
                                r += hash_benchdata(&node->payload);
                        break;
                default:
                        r = 0;
                }
                result[i] = r;
        }

        xfree(datanodes);
}

struct treebenchfuncs bench_np_rbtree_funcs = {
        "BSD RB tree",
        np_rbtree_init,
        np_rbtree_exit,
        np_rbtree_bench,
};
