#include <bsd/sys/tree.h>  /* BSD Red-black tree */
#include "bench.h"


struct rbtree_node {
        RB_ENTRY(rbtree_node) entry_;
        struct benchpayload payload;
};

int compare_rbtree_node(struct rbtree_node *a, struct rbtree_node *b)
{
        return compare_benchpayload(&a->payload, &b->payload);
}

/* declare structure of head */
RB_HEAD(RBtree, rbtree_node);
RB_GENERATE(RBtree, rbtree_node, entry_, compare_rbtree_node);

struct rbtree_state {
        struct RBtree tree;
        struct rbtree_node *nodes;
        size_t nnodes;
};

void *rbtree_init(void)
{
        struct rbtree_state *state;
        state = xcalloc(1, sizeof *state);
        state->tree = (struct RBtree) RB_INITIALIZER(&state->tree);
        return state;
}

void rbtree_exit(void *self)
{
        struct rbtree_state *state = self;
        /* TODO: how to release resources? */
        xfree(state->nodes);
        xfree(state);
}

void rbtree_insertbench(void *self, struct benchpayload *data, size_t n)
{
        struct rbtree_state *state;
        size_t i;

        state = self;
        xfree(state->nodes);
        state->nodes = xcalloc(n, sizeof *state->nodes);
        state->nnodes = n;

        for (i = 0; i < n; i++)
                state->nodes[i].payload = data[i];

        for (i = 0; i < n; i++)
                RB_INSERT(RBtree, &state->tree, &state->nodes[i]);
}

void rbtree_retrievebench(void *self, struct benchpayload *data, size_t n)
{
        struct rbtree_state *state;
        size_t i;

        state = self;

        for (i = 0; i < n; i++)
                RB_FIND(RBtree, &state->tree, &state->nodes[i]);
}

void rbtree_removebench(void *self, struct benchpayload *data, size_t n)
{
        struct rbtree_state *state;
        size_t i;

        state = self;

        for (i = 0; i < n; i++) {
                /*
                 * If we can't guarantee that there are no duplicates
                 * in data we have to do an exists check first.
                 */
                /*
                if (RB_FIND(RBtree, &state->tree, &state->nodes[i]))
                */
                        RB_REMOVE(RBtree, &state->tree, &state->nodes[i]);
        }

}

struct treebenchfuncs rbtree_funcs = {
        "BSD RB tree",
        rbtree_init,
        rbtree_exit,
        rbtree_insertbench,
        rbtree_retrievebench,
        rbtree_removebench,
};
