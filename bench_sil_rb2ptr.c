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
        sil_rb_reset(&state->tree);

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

static void silrb_bench(void *self, struct benchpayload *data, struct action *action, size_t ndata, size_t naction, unsigned *result)
{
        struct sil_rb_path path;
        struct silrb_state *state;
        struct silrb_node *datanodes;
        struct sil_rb_head *head;
        int dir;
        size_t i;
        unsigned r;

        (void) ndata;

        datanodes = xcalloc(naction, sizeof *datanodes);

        state = self;
        for (i = 0; i < naction; i++) {
                switch(action[i].action) {
                case BENCH_ACTION_ADD:
                        findpath(&state->tree, &data[i], &path, &dir);
                        if (dir == 0)
                                sil_rb_insert(&path, dir < 0 ? SIL_RB_LEFT : SIL_RB_RIGHT, &state->nodes[i].head);
                        r = !dir;
                        break;
                case BENCH_ACTION_FIND:
                        head = find(&state->tree, &data[i]);
                        r = !!head;
                        break;
                case BENCH_ACTION_REMOVE:
                        findpath(&state->tree, &data[i], &path, &dir);
                        if (dir == 0)
                                sil_rb_delete(&path);
                        r = !dir;
                        break;
                case BENCH_ACTION_HASHSUM:
                        r = 0;
                        sil_rb_path_reset(&path, &state->tree);
                        for (head = sil_rb_path_visit_min(&path, &state->tree);
                             head != NULL;
                             head = sil_rb_path_visit_inorder_successor(&path)) {
                                r += hash_benchdata(&((struct silrb_node *) head)->payload);
                        }
                        break;
                default:
                        r = 0;
                }
                result[i] = r;
        }

        xfree(datanodes);
}

struct treebenchfuncs bench_rb2_funcs = {
        "SIL 2-pointer Red-black tree",
        silrb_init,
        silrb_exit,
        silrb_bench,
};
