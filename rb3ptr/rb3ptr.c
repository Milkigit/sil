#include <assert.h>
#include <stddef.h>  /* NULL */
#include "rb3ptr.h"


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

        if (!RB3_PARENT(RB3_PARENT(head))) {
                /* We are root */
                RB3_PARENT(head)->ptr[RB3_LEFT] &= ~RB3_COLOR_BIT;
                return;
        }

        if (!RB3_GET_COLOR_BIT(RB3_PARENT(RB3_PARENT(head)), RB3_PARENT_DIR(RB3_PARENT(head))))
                /* parent is black */
                return;

        /*
         * Since parent is red parent can't be the root.
         * So we have at least a grandparent node, and grand-grandparent
         * is either a real node or the base head.
         */
        pnt = RB3_PARENT(head);
        gpnt = RB3_PARENT(pnt);
        ggpnt = RB3_PARENT(gpnt);
        left = RB3_PARENT_DIR(head);
        right = !RB3_PARENT_DIR(head);
        gdir = RB3_PARENT_DIR(pnt);
        ggdir = RB3_PARENT_DIR(gpnt);

        if (RB3_GET_COLOR_BIT(gpnt, !gdir)) {
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

        if (!RB3_PARENT(head))
                /* head is base fake head */
                return;

        if (!RB3_PARENT(RB3_PARENT(head)))
                /* head is root */
                return;

        pnt = RB3_PARENT(head);
        left = RB3_PARENT_DIR(head);
        right = !RB3_PARENT_DIR(head);
        gpnt = RB3_PARENT(pnt);
        gdir = RB3_PARENT_DIR(pnt);
        sibling = RB3_CHILD(pnt, right);
        sleft = RB3_CHILD(sibling, left);

        if (RB3_GET_COLOR_BIT(pnt, right)) {
                /* sibling is red */
                rb3_connect(pnt, right, sleft, RB3_BLACK);
                rb3_connect(sibling, left, pnt, RB3_RED);
                rb3_connect(gpnt, gdir, sibling, RB3_BLACK);
                rb3_delete_rebalance(head);
        } else if (RB3_GET_COLOR_BIT(sibling, right)) {
                /* outer child of sibling is red */
                rb3_connect_if_child(pnt, right, sleft, RB3_GET_COLOR_BIT(sibling, left));
                rb3_connect(sibling, left, pnt, RB3_BLACK);
                rb3_connect(gpnt, gdir, sibling, RB3_GET_COLOR_BIT(gpnt, gdir));
                rb3_set_black(sibling, right);
        } else if (RB3_GET_COLOR_BIT(sibling, left)) {
                /* inner child of sibling is red */
                sleftleft = RB3_CHILD(sleft, left);
                sleftright = RB3_CHILD(sleft, right);
                rb3_connect_if_child(pnt, right, sleftleft, RB3_BLACK);
                rb3_connect_if_child(sibling, left, sleftright, RB3_BLACK);
                rb3_connect(sleft, left, pnt, RB3_BLACK);
                rb3_connect(sleft, right, sibling, RB3_BLACK);
                rb3_connect(gpnt, gdir, sleft, RB3_GET_COLOR_BIT(gpnt, gdir));
        } else if (RB3_GET_COLOR_BIT(gpnt, gdir)) {
                /* parent is red */
                rb3_set_red(pnt, right);
                rb3_set_black(gpnt, gdir);
        } else {
                /* all relevant nodes are black */
                rb3_set_red(pnt, right);
                rb3_delete_rebalance(pnt);
        }
}

static void rb3_delete_leaf(struct rb3_head *head)
{
        struct rb3_head *pnt;
        rb3_ptr pdir;

        pnt = RB3_PARENT(head);
        pdir = RB3_PARENT_DIR(head);

        if (!RB3_GET_COLOR_BIT(pnt, pdir))
                /* To be deleted node is black => height decreased */
                rb3_delete_rebalance(head);

        rb3_connect_if_child(pnt, pdir, NULL, RB3_BLACK);

        /* Better zero it... someone will thank us in the future */
        head->ptr[RB3_LEFT] = 0;
        head->ptr[RB3_RIGHT] = 0;
        head->ptr[RB3_PARENT] = 0;
}

static void rb3_delete_halfleaf(struct rb3_head *head, rb3_ptr dir)
{
        struct rb3_head *pnt;
        struct rb3_head *cld;
        rb3_ptr pdir;
        rb3_ptr headred;
        rb3_ptr childred;

        pnt = RB3_PARENT(head);
        cld = RB3_CHILD(head, dir);
        pdir = RB3_PARENT_DIR(head);
        headred = RB3_GET_COLOR_BIT(pnt, pdir);
        childred = RB3_GET_COLOR_BIT(head, dir);

        if (!headred && !childred)
                /* To be deleted node is black (and child cannot be repainted)
                 * => height decreased */
                rb3_delete_rebalance(cld);

        rb3_connect(pnt, pdir, cld, RB3_BLACK);

        /* Better zero it... someone will thank us in the future */
        head->ptr[RB3_LEFT] = 0;
        head->ptr[RB3_RIGHT] = 0;
        head->ptr[RB3_PARENT] = 0;
}

static struct rb3_head *rb3_descendant_inorder_successor(struct rb3_head *head)
{
        head = RB3_CHILD(head, RB3_RIGHT);
        while (RB3_CHILD(head, RB3_LEFT))
                head = RB3_CHILD(head, RB3_LEFT);
        return head;
}

static void rb3_delete_internal(struct rb3_head *head)
{
        struct rb3_head *subst;
        struct rb3_head *left;
        struct rb3_head *right;
        struct rb3_head *parent;
        int pdir;
        int pcol;

        subst = rb3_descendant_inorder_successor(head);
        if (RB3_CHILD(subst, RB3_RIGHT))
                rb3_delete_halfleaf(subst, RB3_RIGHT);
        else
                rb3_delete_leaf(subst);
        *subst = *head;

        left = RB3_CHILD(head, RB3_LEFT);
        right = RB3_CHILD(head, RB3_RIGHT);
        parent = RB3_PARENT(head);
        pdir = RB3_PARENT_DIR(head);
        pcol = RB3_GET_COLOR_BIT(parent, pdir);

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
        if (RB3_CHILD(head, RB3_LEFT)) {
                if (RB3_CHILD(head, RB3_RIGHT))
                        rb3_delete_internal(head);
                else
                        rb3_delete_halfleaf(head, RB3_LEFT);
        } else if (RB3_CHILD(head, RB3_RIGHT)) {
                rb3_delete_halfleaf(head, RB3_RIGHT);
        } else {
                rb3_delete_leaf(head);
        }
}
