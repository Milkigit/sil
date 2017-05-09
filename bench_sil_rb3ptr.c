#include <assert.h>
#include <stdio.h>

#include "bench.h"
#include "bench_sil_rb3ptr.h"

#include "rb3ptr/build/include/rb3ptr.h"
#include "rb3ptr/build/include/rb3ptr-gen.h"

/********************
 * Bench for rb3ptr *
 ********************/

RB3_GEN_STRUCTS(bench_rb3tree);

struct bench_rb3_node {
        struct bench_rb3tree_head head;
        struct benchpayload payload;
};


static struct bench_rb3_node *bench_rb3_get_node(struct bench_rb3tree_head *head)
{
        return (struct bench_rb3_node *) ((char *)head - offsetof(struct bench_rb3_node, head));
}

static struct bench_rb3tree_head *bench_rb3_get_head(struct bench_rb3_node *node)
{
        return &node->head;
}

static int comparison(struct bench_rb3_node *a, struct bench_rb3_node *b)
{
        return compare_benchpayload(&a->payload, &b->payload);
}

RB3_GEN_INLINE_PROTO(bench_rb3tree, struct bench_rb3_node, bench_rb3_get_head, bench_rb3_get_node);
RB3_GEN_NODECMP_PROTO(bench_rb3tree, /* no suffix */, struct bench_rb3_node, bench_rb3_get_head, bench_rb3_get_node, comparison);
RB3_GEN_INLINE(bench_rb3tree, struct bench_rb3_node, bench_rb3_get_head, bench_rb3_get_node);
RB3_GEN_NODECMP(bench_rb3tree, /* no suffix */, struct bench_rb3_node, bench_rb3_get_head, bench_rb3_get_node, comparison);

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

static void bench_rb3_insertbench(void *self, struct benchpayload *data, size_t n)
{
        struct bench_rb3_state *state;
        size_t i;

        state = self;
        state->nodes = xcalloc(n, sizeof *state->nodes);
        state->nnodes = n;
        for (i = 0; i < n; i++)
                state->nodes[i].payload = data[i];
        for (i = 0; i < n; i++)
                bench_rb3tree_insert(&state->nodes[i], &state->tree);
}

static void bench_rb3_retrievebench(void *self, struct benchpayload *data, size_t n)
{
        struct bench_rb3_node *found;
        struct bench_rb3_state *state;
        size_t i;

        (void) data;
        state = self;
        for (i = 0; i < n; i++) {
                found = bench_rb3tree_find(&state->tree, &state->nodes[i]);
                (void) found; assert(found != NULL);
        }
}

static void bench_rb3_removebench(void *self, struct benchpayload *data, size_t n)
{
        struct bench_rb3_state *state;
        size_t i;

        (void) data;
        state = self;
        for (i = 0; i < n; i++)
                bench_rb3tree_delete(&state->nodes[i], &state->tree);
}

static void bench_rb3_addelems(void *self, size_t *out_count, unsigned *out_sumofhashes)
{
        struct bench_rb3_state *state;
        struct bench_rb3_node *node;
        size_t count;
        unsigned hsum;

        state = self;
        count = 0;
        hsum = 0;
        RB3_FOREACH(bench_rb3tree, &state->tree, node) {
                count += 1;
                hsum += hash_benchdata(&node->payload);
        }
        *out_count = count;
        *out_sumofhashes = hsum;
}

struct treebenchfuncs bench_rb3_funcs = {
        "SIL 3-pointer Red-black tree",
        bench_rb3_init,
        bench_rb3_exit,
        bench_rb3_insertbench,
        bench_rb3_retrievebench,
        bench_rb3_removebench,
        bench_rb3_addelems,
};
