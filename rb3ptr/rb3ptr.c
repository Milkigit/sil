#include <assert.h>
#include <stddef.h>  /* NULL */
#include "rb3ptr.h"

#define RB3_DIR_BIT (1<<0)
#define RB3_COLOR_BIT (1<<1)
#define RB3_BLACK (0)
#define RB3_RED (RB3_COLOR_BIT)
#define RB3_CHILD_PTR(head, color) ((rb3_ptr)(head) | color)
#define RB3_PARENT_PTR(head, dir) ((rb3_ptr)(head) | dir)
#define RB3_BLACK_CHILD(head, dir) ((struct rb3_head *)(head)->ptr[dir])
#define rb3_get_parent(head) ((struct rb3_head *)((head)->ptr[RB3_PARENT] & ~RB3_DIR_BIT))

static int rb3_get_color_bit(struct rb3_head *head, int dir)
{
        return (head)->ptr[dir] & RB3_COLOR_BIT;
}

static int rb3_is_red(struct rb3_head *head, int dir)
{
        return rb3_get_color_bit(head, dir) != 0;
}

static void rb3_set_red(struct rb3_head *head, int dir)
{
        head->ptr[dir] |= RB3_COLOR_BIT;
}

static void rb3_set_black(struct rb3_head *head, int dir)
{
        head->ptr[dir] &= ~RB3_COLOR_BIT;
}

static void rb3_connect(struct rb3_head *head, int dir, struct rb3_head *child, int color)
{
        head->ptr[dir] = RB3_CHILD_PTR(child, color);
        child->ptr[RB3_PARENT] = RB3_PARENT_PTR(head, dir);
}

static void rb3_connect_if_child(struct rb3_head *head, int dir, struct rb3_head *child, int color)
{
        head->ptr[dir] = RB3_CHILD_PTR(child, color);
        if (child)
                child->ptr[RB3_PARENT] = RB3_PARENT_PTR(head, dir);
}

static void rb3_insert_rebalance(struct rb3_head *head)
{
        struct rb3_head *pnt;
        struct rb3_head *gpnt;
        struct rb3_head *ggpnt;
        int left;
        int right;
        int gdir;
        int ggdir;

        if (!rb3_get_parent(rb3_get_parent(head))) {
                /* We are root */
                rb3_get_parent(head)->ptr[RB3_LEFT] &= ~RB3_COLOR_BIT;
                return;
        }

        if (!rb3_is_red(rb3_get_parent(rb3_get_parent(head)), rb3_get_parent_dir(rb3_get_parent(head))))
                /* parent is black */
                return;

        /*
         * Since parent is red parent can't be the root.
         * So we have at least a grandparent node, and grand-grandparent
         * is either a real node or the base head.
         */
        pnt = rb3_get_parent(head);
        gpnt = rb3_get_parent(pnt);
        ggpnt = rb3_get_parent(gpnt);
        left = rb3_get_parent_dir(head);
        right = !rb3_get_parent_dir(head);
        gdir = rb3_get_parent_dir(pnt);
        ggdir = rb3_get_parent_dir(gpnt);

        if (rb3_is_red(gpnt, !gdir)) {
                /* uncle and parent are both red */
                rb3_set_red(ggpnt, ggdir);
                rb3_set_black(gpnt, RB3_LEFT);
                rb3_set_black(gpnt, RB3_RIGHT);
                rb3_insert_rebalance(gpnt);
        } else if (gdir == right) {
                rb3_connect_if_child(pnt, left, RB3_BLACK_CHILD(head, right), RB3_BLACK);
                rb3_connect_if_child(gpnt, right, RB3_BLACK_CHILD(head, left), RB3_BLACK);
                rb3_connect(head, left, gpnt, RB3_RED);
                rb3_connect(head, right, pnt, RB3_RED);
                rb3_connect(ggpnt, ggdir, head, RB3_BLACK);
        } else {
                rb3_connect_if_child(gpnt, left, RB3_BLACK_CHILD(pnt, right), RB3_BLACK);
                rb3_connect(pnt, right, gpnt, RB3_RED);
                rb3_connect(ggpnt, ggdir, pnt, RB3_BLACK);
        }
}

static void rb3_delete_rebalance(struct rb3_head *head)
{
        struct rb3_head *pnt;
        struct rb3_head *gpnt;
        struct rb3_head *sibling;
        struct rb3_head *sleft;
        struct rb3_head *sleftleft;
        struct rb3_head *sleftright;
        int left;
        int right;
        int gdir;

        if (!rb3_get_parent(head))
                /* head is base fake head */
                return;

        if (!rb3_get_parent(rb3_get_parent(head)))
                /* head is root */
                return;

        pnt = rb3_get_parent(head);
        left = rb3_get_parent_dir(head);
        right = !rb3_get_parent_dir(head);
        gpnt = rb3_get_parent(pnt);
        gdir = rb3_get_parent_dir(pnt);
        sibling = rb3_get_child(pnt, right);
        sleft = rb3_get_child(sibling, left);

        if (rb3_is_red(pnt, right)) {
                /* sibling is red */
                rb3_connect(pnt, right, sleft, RB3_BLACK);
                rb3_connect(sibling, left, pnt, RB3_RED);
                rb3_connect(gpnt, gdir, sibling, RB3_BLACK);
                rb3_delete_rebalance(head);
        } else if (rb3_is_red(sibling, right)) {
                /* outer child of sibling is red */
                rb3_connect_if_child(pnt, right, sleft, rb3_get_color_bit(sibling, left));
                rb3_connect(sibling, left, pnt, RB3_BLACK);
                rb3_connect(gpnt, gdir, sibling, rb3_get_color_bit(gpnt, gdir));
                rb3_set_black(sibling, right);
        } else if (rb3_is_red(sibling, left)) {
                /* inner child of sibling is red */
                sleftleft = rb3_get_child(sleft, left);
                sleftright = rb3_get_child(sleft, right);
                rb3_connect_if_child(pnt, right, sleftleft, RB3_BLACK);
                rb3_connect_if_child(sibling, left, sleftright, RB3_BLACK);
                rb3_connect(sleft, left, pnt, RB3_BLACK);
                rb3_connect(sleft, right, sibling, RB3_BLACK);
                rb3_connect(gpnt, gdir, sleft, rb3_get_color_bit(gpnt, gdir));
        } else if (rb3_is_red(gpnt, gdir)) {
                /* parent is red */
                rb3_set_red(pnt, right);
                rb3_set_black(gpnt, gdir);
        } else {
                /* all relevant nodes are black */
                rb3_set_red(pnt, right);
                rb3_delete_rebalance(pnt);
        }
}

static void rb3_delete_noninternal(struct rb3_head *head)
{
        struct rb3_head *pnt;
        struct rb3_head *cld;
        rb3_ptr pdir;
        rb3_ptr headred;
        rb3_ptr childred;
        int dir;

        dir = rb3_get_child(head, RB3_RIGHT) ? RB3_RIGHT : RB3_LEFT;

        pnt = rb3_get_parent(head);
        cld = rb3_get_child(head, dir);
        pdir = rb3_get_parent_dir(head);
        headred = rb3_is_red(pnt, pdir);
        childred = rb3_is_red(head, dir);

        if (!headred && !childred)
                /* To be deleted node is black (and child cannot be repainted)
                 * => height decreased */
                rb3_delete_rebalance(head);

        rb3_connect_if_child(pnt, pdir, cld, RB3_BLACK);
}

static void rb3_delete_internal(struct rb3_head *head)
{
        struct rb3_head *subst;
        struct rb3_head *left;
        struct rb3_head *right;
        struct rb3_head *parent;
        int pdir;
        int pcol;

        subst = rb3_get_descendant_successor(head);
        rb3_delete_noninternal(subst);
        *subst = *head;

        left = rb3_get_child(head, RB3_LEFT);
        right = rb3_get_child(head, RB3_RIGHT);
        parent = rb3_get_parent(head);
        pdir = rb3_get_parent_dir(head);
        pcol = rb3_get_color_bit(parent, pdir);

        if (left)
                left->ptr[RB3_PARENT] = RB3_PARENT_PTR(subst, RB3_LEFT);
        if (right)
                right->ptr[RB3_PARENT] = RB3_PARENT_PTR(subst, RB3_RIGHT);
        parent->ptr[pdir] = RB3_CHILD_PTR(subst, pcol);
}

void rb3_init(struct rb3_tree *tree)
{
        tree->base.ptr[RB3_LEFT] = RB3_CHILD_PTR(NULL, RB3_BLACK);
        tree->base.ptr[RB3_RIGHT] = RB3_CHILD_PTR(NULL, RB3_BLACK);
        tree->base.ptr[RB3_PARENT] = RB3_PARENT_PTR(NULL, RB3_BLACK);;
}

void rb3_exit(struct rb3_tree *tree)
{
        /* ? */
        (void) tree;
}

void rb3_insert(struct rb3_head *head, struct rb3_head *parent, int dir)
{
        assert(dir == RB3_LEFT || dir == RB3_RIGHT);

        parent->ptr[dir] = RB3_CHILD_PTR(head, RB3_RED);
        head->ptr[RB3_PARENT] = RB3_PARENT_PTR(parent, dir);
        head->ptr[RB3_LEFT] = RB3_CHILD_PTR(NULL, RB3_BLACK);
        head->ptr[RB3_RIGHT] = RB3_CHILD_PTR(NULL, RB3_BLACK);
        rb3_insert_rebalance(head);
}

void rb3_delete(struct rb3_head *head)
{
        if (rb3_get_child(head, RB3_LEFT) && rb3_get_child(head, RB3_RIGHT))
                rb3_delete_internal(head);
        else
                rb3_delete_noninternal(head);

        /* Better zero it... someone will thank us in the future */
        head->ptr[RB3_LEFT] = 0;
        head->ptr[RB3_RIGHT] = 0;
        head->ptr[RB3_PARENT] = 0;
}

/*
 * DEBUG
 */
#include <stdio.h>

int rb3_is_valid_tree(struct rb3_head *head, int isred)
{
        struct rb3_head *left;
        struct rb3_head *right;
        int isleftred;
        int isrightred;

        if (!head)
                return 1;

        left = rb3_get_child(head, RB3_LEFT);
        right = rb3_get_child(head, RB3_RIGHT);
        isleftred = rb3_is_red(head, RB3_LEFT);
        isrightred = rb3_is_red(head, RB3_RIGHT);

        if (isred && isleftred)
                return 0;
        if (isred && isrightred)
                return 0;

        if (left && rb3_get_parent_dir(left) != RB3_LEFT)
                return 0;
        if (right && rb3_get_parent_dir(right) != RB3_RIGHT)
                return 0;

        return rb3_is_valid_tree(left, isleftred)
                && rb3_is_valid_tree(right, isrightred);
}

void rb3_inorder_traversal(struct rb3_head *head, void (*action)(struct rb3_head *, void *), void *arg)
{
        struct rb3_head *child;

        if (!head)
                return;

        action(head, arg);

        if (rb3_get_child(head, RB3_LEFT)) {
                child = rb3_get_child(head, RB3_LEFT);
                rb3_inorder_traversal(child, action, arg);
                action(head, arg);
        }

        if (rb3_get_child(head, RB3_RIGHT)) {
                child = rb3_get_child(head, RB3_RIGHT);
                rb3_inorder_traversal(child, action, arg);
                action(head, arg);
        }
}
