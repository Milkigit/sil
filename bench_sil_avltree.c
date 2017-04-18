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
        printf("sizeof SIL_AVLhead struct: %zd\n", sizeof state->nodes[0].head);
        printf("position of payload in node: %zd\n", ((char *)&state->nodes[0].payload) - ((char*)&state->nodes[0]));
        printf("sizeof struct sil_node: %zd\n", sizeof state->nodes[0]);
        /*
         */
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

void sil_insertbench(void *self, struct benchpayload *data, size_t n)
{
        struct sil_state *state;
        size_t i;

        state = self;
        xfree(state->nodes);
        state->nodes = xcalloc(n, sizeof *state->nodes);
        state->nnodes = n;

        for (i = 0; i < n; i++)
                state->nodes[i].payload = data[i];

        for (i = 0; i < n; i++)
                SIL_AVLtree_insert(&state->tree, &state->nodes[i]);

        /*
        printf("Height after insertion bench: %d\n", SIL_AVLtree_getheight(&state->tree));
        */
}

void sil_retrievebench(void *self, struct benchpayload *data, size_t n)
{
        struct sil_state *state;
        struct sil_node *node;
        size_t i;

        state = self;
        for (i = 0; i < n; i++) {
                node = SIL_AVLtree_find(&state->tree, &state->nodes[i]);
                assert(node);
                (void) node;  /* suppress unused warnings with -DNDEBUG */
        }
}

void sil_removebench(void *self, struct benchpayload *data, size_t n)
{
        struct sil_state *state;
        struct sil_node *node;
        size_t i;

        state = self;

        /*
        assert_valid_parentlinks_in_tree(state->tree.base.left);
        printf("parent links before removal seem to be ok!\n"); fflush(stdout);
        */

        for (i = 0; i < n; i++) {
                node = SIL_AVLtree_find(&state->tree, &state->nodes[i]);
                if (!node)
                        /* can happen if there are duplicate elements in input
                         * data */
                        continue;

                SIL_AVLtree_erase(node);

                /*
                printf("checking parent links after removal of #%zd\n", i); fflush(stdout);
                assert_valid_parentlinks_in_tree(state->tree.base.left);
                printf("parent links after removal of #%zd seem to be ok!\n", i); fflush(stdout);
                */
        }
}

struct treebenchfuncs sil_funcs = {
        "SIL AVLtree",
        sil_init,
        sil_exit,
        sil_insertbench,
        sil_retrievebench,
        sil_removebench,
};

#undef NAME
#undef TYPE
#undef MEMBER
#undef COMPARE
