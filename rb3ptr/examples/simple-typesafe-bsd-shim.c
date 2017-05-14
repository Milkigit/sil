#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <rb3ptr.h>
#include <rb3ptr-bsd-shim.h>

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

struct foo {
        RB_ENTRY(foo) entry;
        int val;
};

static int foo_compare(struct foo *x, void *data)
{
        struct foo *y = data;
        return int_compare(x->val, y->val);
}


RB_HEAD(footree, foo);
RB_PROTOTYPE_STATIC(footree, foo, entry, foo_compare);
RB_GENERATE_STATIC(footree, foo, entry, foo_compare);

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

        RB_INIT(&tree);
        foo = xalloc(NUM_FOOS * sizeof (struct foo));
        for (i = 0; i < NUM_FOOS; i++)
                foo[i].val = NUM_FOOS - (int) i;
        for (i = 0; i < NUM_FOOS; i++)
                RB_INSERT(footree, &tree, &foo[i]);
        for (i = 0; i < NUM_FOOS; i++)
                RB_FIND(footree, &tree, &foo[i]);
        for (iter = RB_MIN(footree, &tree); iter != NULL; iter = RB_NEXT(footree, &tree, iter))
                printf("iter %d\n", iter->val);
        for (i = 0; i < NUM_FOOS; i++)
                RB_REMOVE(footree, &tree, &foo[i]);
        foo = xalloc(NUM_FOOS * sizeof (struct foo));

        return 0;
}
