#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <rb3ptr.h>

RB3_GEN_IMPL();

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

static int foohead_compare(struct rb3_head *a, void *data)
{
        return foo_compare(get_foo(a), (struct foo *) data);
}

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
        struct rb3_tree tree;
        struct rb3_head *iter;
        struct foo *foo;
        size_t i;

        rb3_reset_tree(&tree);
        foo = xalloc(NUM_FOOS * sizeof (struct foo));
        for (i = 0; i < NUM_FOOS; i++)
                foo[i].val = NUM_FOOS - (int) i;
        for (i = 0; i < NUM_FOOS; i++)
                rb3_insert(&tree, &foo[i].head, foohead_compare, &foo[i]);
        for (iter = rb3_get_min(&tree); iter != NULL; iter = rb3_get_next(iter))
                printf("iter %d\n", get_foo(iter)->val);
        for (i = 0; i < NUM_FOOS; i++)
                rb3_delete(&tree, foohead_compare, &foo[i].head);
        xfree(foo);

        return 0;
}
