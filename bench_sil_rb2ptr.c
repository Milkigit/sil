#include <assert.h>
#include <stdio.h>

#include "bench.h"
#include "bench_sil_rb2ptr.h"
#include "rb2ptr/sil_rb2ptr.h"

/***********************************
 * SIL RBTREE bench implementation *
 ***********************************/

struct silrb_node {
        struct sil_rb_head head;
        struct benchpayload payload;
};

struct silrb_state {
        struct sil_rb_tree tree;
        struct silrb_node *nodes;
        size_t nnodes;
};

static int compare_sil_rb_head(struct sil_rb_head *a, struct benchpayload *data)
{
        return compare_benchpayload(&((struct silrb_node *)a)->payload, data);
}

static struct sil_rb_head *find(struct sil_rb_tree *tree, struct benchpayload *data)
{
        struct sil_rb_head *head;
        int r;

        head = sil_rb_get_root(tree);
        while (head) {
                r = compare_sil_rb_head(head, data);
                if (r < 0)
                        head = sil_rb_get_cld(head, SIL_RB_LEFT);
                else if (r > 0)
                        head = sil_rb_get_cld(head, SIL_RB_RIGHT);
                else
                        break;
        }
        return head;
}

/* like find, but records path */
static void findpath(struct sil_rb_tree *tree, struct benchpayload *data, struct sil_rb_path *path, int *r_out)
{
        struct sil_rb_head *head;
        int r;

        r = -1;
        sil_rb_path_reset(path, tree);
        head = sil_rb_path_visit_root(path);
        while (head) {
                r = compare_sil_rb_head(head, data);
                if (r < 0)
                        head = sil_rb_path_visit_child(path, SIL_RB_LEFT);
                else if (r > 0)
                        head = sil_rb_path_visit_child(path, SIL_RB_RIGHT);
                else
                        break;
        }
        *r_out = r;
}

static void *silrb_init(void)
{
        struct silrb_state *state;

        state = xcalloc(1, sizeof *state);

        printf("sizeof sil_rb_head struct: %zd\n", sizeof state->nodes[0].head);
        printf("position of payload in node: %zd\n", ((char *)&state->nodes[0].payload) - ((char*)&state->nodes[0]));
        printf("sizeof struct silrb_node: %zd\n", sizeof state->nodes[0]);

        return state;
}

static void silrb_exit(void *self)
{
        struct silrb_state *state = self;
        /*SIL_AVLtree_exit(&state->tree);
         */
        xfree(state->nodes);
        xfree(state);
}

static void silrb_insertbench(void *self, struct benchpayload *data, size_t n)
{
        struct sil_rb_path path;
        struct silrb_state *state;
        size_t i;
        int r;

        state = self;
        state->nodes = xcalloc(n, sizeof *state->nodes);
        state->nnodes = n;

        sil_rb_reset(&state->tree);

        for (i = 0; i < n; i++)
                state->nodes[i].payload = data[i];

        for (i = 0; i < n; i++) {
                findpath(&state->tree, &data[i], &path, &r);
                if (r == 0) {
                        /* element present */
                        continue;
                }
                sil_rb_insert(&path, r < 0 ? SIL_RB_LEFT : SIL_RB_RIGHT, &state->nodes[i].head);
        }
}

static void silrb_retrievebench(void *self, struct benchpayload *data, size_t n)
{
        struct sil_rb_path BENCH_UNUSED path;
        struct sil_rb_head BENCH_UNUSED *head;
        struct silrb_state *state;
        size_t i;
        int BENCH_UNUSED r;

        state = self;
        for (i = 0; i < n; i++) {
                /*findpath(&state->tree, &state->nodes[i].head, &path, &r);
                assert(r == 0);
                  */
                head = find(&state->tree, &data[i]);
                assert(head);
        }
}

static void silrb_removebench(void *self, struct benchpayload *data, size_t n)
{
        struct sil_rb_path path;
        struct silrb_state *state;
        size_t i;
        int r;

        state = self;
        for (i = 0; i < n; i++) {
                findpath(&state->tree, &data[i], &path, &r);
                if (r != 0) {
                        /* Node already deleted. Can happen if there are
                         * duplicate elements in input data. */
                        continue;
                }
                sil_rb_delete(&path);
        }
}

static void silrb_addelems(void *self, size_t *out_count, unsigned *out_sumofhashes)
{
        struct sil_rb_path path;
        struct silrb_state *state;
        struct sil_rb_head *head;
        size_t count;
        unsigned sumofhashes;

        state = self;
        count = 0;
        sumofhashes = 0;

        sil_rb_path_reset(&path, &state->tree);
        for (head = sil_rb_path_visit_min(&path, &state->tree);
             head != NULL;
             head = sil_rb_path_visit_inorder_successor(&path)) {
                count += 1;
                sumofhashes += hash_benchdata(&((struct silrb_node *) head)->payload);
        }

        *out_count = count;
        *out_sumofhashes = sumofhashes;
}

struct treebenchfuncs bench_rb2_funcs = {
        "SIL 2-pointer Red-black tree",
        silrb_init,
        silrb_exit,
        silrb_insertbench,
        silrb_retrievebench,
        silrb_removebench,
        silrb_addelems,
};
