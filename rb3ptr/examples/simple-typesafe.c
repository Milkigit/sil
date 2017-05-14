#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <rb3ptr.h>

RB3_GEN_IMPL_STATIC();

/****************
 * Foo
 ****************/

struct foo {
        struct rb3_head head;
        int val;
};

static int foo_compare(struct foo *x, struct foo *y)
{
        return (x->val > y->val) - (x->val < y->val);
}

static struct foo *get_foo(struct rb3_head *head)
{
        return (struct foo *)((char *) head - offsetof(struct foo, head));
}

static struct rb3_head *get_head(struct foo *node)
{
        return &node->head;
}

RB3_GEN_TREE_DEFINITION(footree);
RB3_GEN_INLINE_PROTO_STATIC(footree, struct foo, get_head, get_foo);
RB3_GEN_NODECMP_PROTO_STATIC(footree, /* no suffix for these compare functions */, struct foo, get_head, get_foo, foo_compare);
RB3_GEN_NODECMP_STATIC(footree, /* no suffix for these compare functions */, struct foo, get_head, get_foo, foo_compare);

/****************
 * Memory
 ****************/

void *xalloc(size_t n)
{
        void *p = malloc(n);
        if (!p) {
                fprintf(stderr, "out of memory!\n");
                exit(1);
        }
        return p;
}

void xfree(void *p)
{
        free(p);
}

/****************
 * MAIN
 ****************/

#define NUM_FOOS 1024

int main(void)
{
        struct footree tree;
        struct foo *iter;
        struct foo *foo;
        size_t i;

        footree_init(&tree);
        foo = xalloc(NUM_FOOS * sizeof (struct foo));
        for (i = 0; i < NUM_FOOS; i++)
                foo[i].val = NUM_FOOS - (int) i;
        for (i = 0; i < NUM_FOOS; i++)
                footree_insert(&tree, &foo[i]);
        for (iter = footree_get_min(&tree); iter != NULL; iter = footree_get_next(iter))
                printf("iter %d\n", iter->val);
        for (i = 0; i < NUM_FOOS; i++)
                footree_delete(&tree, &foo[i]);
        foo = xalloc(NUM_FOOS * sizeof (struct foo));

        return 0;
}
