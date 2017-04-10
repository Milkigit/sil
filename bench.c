#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bsd/sys/tree.h>  /* BSD Red-black tree */
#include "timer/sil_timer.h"

/* Helper functions */

void *xcalloc(size_t n, size_t s)
{
        void *ptr = calloc(n, s);
        if (!ptr) {
                perror("calloc()");
                exit(1);
        }
        return ptr;
}

/* bench interface */

struct benchtestdata {
        struct benchpayload *data;
        size_t n;
};

#define PAYLOAD_TYPE 0

struct benchpayload {
        /* can be changed. Be sure to adapt compare_benchpayload() as well */

#if PAYLOAAD_TYPE == 0
        int a;
#else
        float a;
        float b;
#endif
};

struct treebenchfuncs {
        const char *benchname;
        void *(*init)(void);
        void (*exit)(void *self);
        void (*insertbench)(void *self, struct benchpayload *data, size_t n);
        void (*retrievebench)(void *self, struct benchpayload *data, size_t n);
        void (*removebench)(void *self, struct benchpayload *data, size_t n);
};

int compare_benchpayload(struct benchpayload *x, struct benchpayload *y)
{
#if PAYLOAD_TYPE == 0
        return y->a - x->a;
#else
        if (x->a != y->a)
                return y->a - x->a;
        return y->b - x->b;
#endif
}


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
        free(state->nodes);
        free(state);
}

void sil_insertbench(void *self, struct benchpayload *data, size_t n)
{
        struct sil_state *state;
        size_t i;

        state = self;
        free(state->nodes);
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

/********************************************
 * BSD tree.h Red-Black-tree implementation *
 ********************************************/

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
        free(state->nodes);
        free(state);
}

void rbtree_insertbench(void *self, struct benchpayload *data, size_t n)
{
        struct rbtree_state *state;
        size_t i;

        state = self;
        free(state->nodes);
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


/********************************************
 ********************************************/


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

        printf("Running bench %s\n", impl->benchname);
        printf("===========================\n");

        data = impl->init();

        sil_timer_reset(&timer);
        impl->insertbench(data, bench->data, bench->n);
        sil_timer_elapsed_s_ms(&timer, &s, &ms);
        printf("Inserting each element: %d.%.03lds\n", s, ms); fflush(stdout);

        sil_timer_reset(&timer);
        impl->retrievebench(data, bench->data, bench->n);
        sil_timer_elapsed_s_ms(&timer, &s, &ms);
        printf("Finding each element: %d.%.03lds\n", s, ms); fflush(stdout);

        sil_timer_reset(&timer);
        impl->removebench(data, bench->data, bench->n);
        sil_timer_elapsed_s_ms(&timer, &s, &ms);
        printf("Removing each element: %d.%.03lds\n", s, ms); fflush(stdout);

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
        runbench(&sil_funcs, &bench);
        runbench(&rbtree_funcs, &bench);
        benchtestdata_exit(&bench);

        return 0;
}
