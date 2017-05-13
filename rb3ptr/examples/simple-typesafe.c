#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <rb3ptr.h>

RB3_GEN_IMPL_STATIC();

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
 * Foo
 ****************/

static int int_compare(int a, int b)
{
        return (b < a) - (a < b);
}

RB3_GEN_STRUCTS(footree);

struct foo {
        struct footree_head head;
        int val;
};

static int foo_compare(struct foo *x, void *data)
{
        struct foo *y = data;
        return int_compare(x->val, y->val);
}

static struct foo *get_foo(struct footree_head *head)
{
        return (struct foo *)((char *) head - offsetof(struct foo, head));
}

static struct footree_head *get_head(struct foo *node)
{
        return &node->head;
}

RB3_GEN_INLINE_PROTO_STATIC(footree, struct foo, get_head, get_foo);
RB3_GEN_NODECMP_PROTO_STATIC(footree, /* no suffix for these compare functions */, struct foo, get_head, get_foo, foo_compare);
RB3_GEN_NODECMP_STATIC(footree, /* no suffix for these compare functions */, struct foo, get_head, get_foo, foo_compare);

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
