#ifdef RB3PTR_H_INCLUDED
#error rb3ptr.h included twice!
#endif

#define RB3PTR_H_INCLUDED

#define RB3_UNUSED __attribute__((unused))

#define RB3_DIR_BIT (1<<0)
#define RB3_COLOR_BIT (1<<1)
#define RB3_BLACK (0)
#define RB3_RED (RB3_COLOR_BIT)
#define RB3_MASK (~3)
#define RB3_CHILD_PTR(head, color) ((rb3_ptr)(head) | color)
#define RB3_PARENT_PTR(head, dir) ((rb3_ptr)(head) | dir)
#define RB3_SWAP_HEAD(ptr, head) (((ptr) & RB3_MASK) | (rb3_ptr)(head))
#define RB3_GET_COLOR_BIT(head, dir) ((head)->ptr[dir] & RB3_COLOR_BIT)
#define RB3_CHILD(head, dir) ((struct rb3_head *)((head)->ptr[dir] & ~RB3_COLOR_BIT))
#define RB3_BLACK_CHILD(head, dir) ((struct rb3_head *)(head)->ptr[dir])
#define RB3_PARENT(head) ((struct rb3_head *)((head)->ptr[RB3_PARENT] & ~RB3_DIR_BIT))
#define RB3_PARENT_DIR(head) ((head)->ptr[RB3_PARENT] & RB3_DIR_BIT)

typedef unsigned long rb3_ptr;

typedef enum {
        RB3_LEFT = 0,
        RB3_RIGHT = 1,
        RB3_PARENT = 2,
} rb3_dir;

struct rb3_head {
        /*
         * Left, right, and parent pointers.
         *
         * The left and right pointers have additional color bits.
         *
         * The parent pointer contains a direction bit indicating the direction
         * to this child.
         */
        rb3_ptr ptr[3];
};

/*
 * Tree type. It's just a fake base head, but we wrap that in a new type
 * for type safety and future extensibility.
 */
struct rb3_tree {
        struct rb3_head base;
};

/*
 * Get topmost element of tree (or NULL) if empty
 */
static RB3_UNUSED
struct rb3_head *rb3_get_root(struct rb3_tree *tree)
{
        return RB3_CHILD(&tree->base, RB3_LEFT);
}

/*
 * Get (left or right) child
 */
static RB3_UNUSED
struct rb3_head *rb3_get_child(struct rb3_head *head, rb3_dir dir)
{
        return RB3_CHILD(head, dir);
}

/*
 * Get parent head
 */
static RB3_UNUSED
struct rb3_head *rb3_get_parent(struct rb3_head *head)
{
        return RB3_PARENT(head);
}

static RB3_UNUSED
rb3_ptr rb3_child_ptr(struct rb3_head *child, int color)
{
        rb3_ptr result = (rb3_ptr)(child) | color;
        return result;
}

void rb3_init(struct rb3_tree *tree);
void rb3_exit(struct rb3_tree *tree);
void rb3_insert(struct rb3_head *head, struct rb3_head *parent, int dir);
void rb3_delete(struct rb3_head *head);


/* debug */
#include <stdio.h>
static RB3_UNUSED
int rb3_is_valid_tree(struct rb3_head *head, int isred)
{
        struct rb3_head *left;
        struct rb3_head *right;
        int isleftred;
        int isrightred;

        printf("c"); fflush(stdout);

        if (!head)
                return 1;

        left = RB3_CHILD(head, RB3_LEFT);
        right = RB3_CHILD(head, RB3_RIGHT);
        isleftred = RB3_GET_COLOR_BIT(head, RB3_LEFT);
        isrightred = RB3_GET_COLOR_BIT(head, RB3_RIGHT);

        if (isred && isleftred)
                return 0;
        if (isred && isrightred)
                return 0;

        if (left && RB3_PARENT_DIR(left) != RB3_LEFT)
                return 0;
        if (right && RB3_PARENT_DIR(right) != RB3_RIGHT)
                return 0;

        return rb3_is_valid_tree(left, isleftred)
                && rb3_is_valid_tree(right, isrightred);
}
