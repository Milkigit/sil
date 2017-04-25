#include <assert.h>
#include <stdio.h>

#include "bench.h"
#include "bench_sil_rb3ptr.h"
#include "rb3ptr/rb3ptr.h"

/********************
 * Bench for rb3ptr *
 ********************/

struct bench_rb3_node {
        struct rb3_head head;
        struct benchpayload payload;
};

struct bench_rb3_state {
        struct rb3_tree tree;
        struct bench_rb3_node *nodes;
        size_t nnodes;
};

static struct benchpayload *bench_rb3_payload_from_head(struct rb3_head *head)
{
        return &((struct bench_rb3_node *) head)->payload;
}

static BENCH_UNUSED
void bench_rb3_node_debugprint(struct rb3_head *head, void *data)
{
        (void) data;
        printf("%p ", head);
        bench_payload_print(stdout, bench_rb3_payload_from_head(head));
        printf("\n");
}

static int compare_rb3_head(struct rb3_head *a, struct benchpayload *payload)
{
        return compare_benchpayload(bench_rb3_payload_from_head(a), payload);
}

static struct rb3_head *find(struct rb3_tree *tree, struct benchpayload *payload)
{
        struct rb3_head *head;
        int r;

        head = rb3_get_root(tree);
        while (head) {
                r = compare_rb3_head(head, payload);
                if (r < 0)
                        head = rb3_get_child(head, RB3_LEFT);
                else if (r > 0)
                        head = rb3_get_child(head, RB3_RIGHT);
                else
                        return head;
        }
        return NULL;
}

static struct rb3_head *bench_rb3_find_insertion_point(struct rb3_tree *tree, struct benchpayload *payload, struct rb3_head **parent_out, int *dir_out)
{
        struct rb3_head *parent;
        struct rb3_head *head;
        int r;
        int dir;

        parent = &tree->base;
        dir = RB3_LEFT;
        for (;;) {
                head = rb3_get_child(parent, dir);
                if (!head)
                        break;
                r = compare_rb3_head(head, payload);
                if (r < 0)
                        dir = RB3_LEFT;
                else if (r > 0)
                        dir = RB3_RIGHT;
                else
                        return head;
                parent = head;
        }
        *parent_out = parent;
        *dir_out = dir;
        return NULL;
}

static void *bench_rb3_init(void)
{
        struct bench_rb3_state *state;
        printf("sizeof rb3_head struct: %zd\n", sizeof state->nodes[0].head);
        printf("position of payload in node: %zd\n", ((char *)&state->nodes[0].payload) - ((char*)&state->nodes[0]));
        printf("sizeof struct bench_rb3_node: %zd\n", sizeof state->nodes[0]);
        /*
         */
        state = xcalloc(1, sizeof *state);
        return state;
}

static void bench_rb3_exit(void *self)
{
        struct bench_rb3_state *state = self;
        xfree(state->nodes);
        xfree(state);
}

static void bench_rb3_insertbench(void *self, struct benchpayload *data, size_t n)
{
        struct rb3_head *parent;
        struct rb3_head *found;
        struct bench_rb3_state *state;
        size_t i;
        int dir;

        state = self;
        state->nodes = xcalloc(n, sizeof *state->nodes);
        state->nnodes = n;

        rb3_init(&state->tree);

        for (i = 0; i < n; i++)
                state->nodes[i].payload = data[i];

        for (i = 0; i < n; i++) {
                found = bench_rb3_find_insertion_point(
                                &state->tree, &data[i], &parent, &dir);

                if (found != NULL)
                        /* element present */
                        continue;

                /*
                printf("inserting element %d into %p\n", data[i].a, parent);
                */

                rb3_insert(&state->nodes[i].head, parent, dir);

                /*
                printf("\nstate now:\n");
                rb3_inorder_traversal(rb3_get_root(&state->tree),
                                      bench_rb3_node_debugprint, NULL);
                printf("\n");
                */
        }
}

static void bench_rb3_retrievebench(void *self, struct benchpayload *data, size_t n)
{
        struct rb3_head *found;
        struct rb3_head BENCH_UNUSED *head;
        struct bench_rb3_state *state;
        size_t i;

        state = self;
        for (i = 0; i < n; i++) {
                found = find(&state->tree, &data[i]);
                (void) found; assert(found != NULL);
        }
}

static void bench_rb3_removebench(void *self, struct benchpayload *data, size_t n)
{
        struct rb3_head *found;
        struct bench_rb3_state *state;
        size_t i;

        state = self;
        for (i = 0; i < n; i++) {
                found = find(&state->tree, &data[i]);

                if (found == NULL)
                        /* element already deleted (duplicates?) */
                        continue;

                /*
                printf("deleting element %d\n", data[i].a);
                */

                rb3_delete(found);

                /*
                printf("\nstate now:\n");
                rb3_inorder_traversal(rb3_get_root(&state->tree),
                                      bench_rb3_node_debugprint, NULL);
                printf("\n");
                */
        }
}

static void bench_rb3_addelems(void *self, size_t *out_count, unsigned *out_sumofhashes)
{
        struct bench_rb3_state *state;
        struct rb3_head *head;
        size_t count;
        unsigned hsum;

        state = self;

        count = 0;
        hsum = 0;

        for (head = rb3_get_min(&state->tree);
             head != NULL; head = rb3_get_inorder_successor(head)) {
                count += 1;
                hsum += hash_benchdata(bench_rb3_payload_from_head(head));
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
