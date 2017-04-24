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

void np_rbtree_insertbench(void *self, struct benchpayload *data, size_t n)
{
        struct np_rbtree_state *state;
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

void np_rbtree_retrievebench(void *self, struct benchpayload *data, size_t n)
{
        struct np_rbtree_state *state;
        size_t i;

        /* must use data from tree nodes, since the API is lacking */
        (void) data;
        (void) n;

        state = self;

        for (i = 0; i < n; i++)
                RB_FIND(RBtree, &state->tree, &state->nodes[i]);
}

void np_rbtree_removebench(void *self, struct benchpayload *data, size_t n)
{
        struct np_rbtree_state *state;
        size_t i;

        /* must use data from tree nodes, since the API is lacking */
        (void) data;
        (void) n;

        state = self;

        for (i = 0; i < n; i++)
                if (RB_FIND(RBtree, &state->tree, &state->nodes[i]))
                        RB_REMOVE(RBtree, &state->tree, &state->nodes[i]);
}

void np_rbtree_addelems(void *self, size_t *out_count, unsigned *out_sumofhashes)
{
        /* TODO */
        (void) self;
        *out_count = 42;
        *out_sumofhashes = 42;
}

struct treebenchfuncs np_rbtree_funcs = {
        "BSD RB tree",
        np_rbtree_init,
        np_rbtree_exit,
        np_rbtree_insertbench,
        np_rbtree_retrievebench,
        np_rbtree_removebench,
        np_rbtree_addelems,
};
