/*
 * ---------------------------------------------------------------------------
 * Implementation
 *
 * This file provides the (key)type-agnostic tree code that can be shared
 * across all tree instances (with differing key types).
 * ---------------------------------------------------------------------------
 */

static RB3_INLINE
struct rb3_head *rb3_get_black_child(struct rb3_head *head, int dir)
{
        return (struct rb3_head *) head->child[dir];
}

static RB3_INLINE
int rb3_get_color_bit(struct rb3_head *head, int dir)
{
        return head->child[dir] & RB3_COLOR_BIT;
}

static RB3_INLINE
int rb3_is_red(struct rb3_head *head, int dir)
{
        return rb3_get_color_bit(head, dir) != 0;
}

static RB3_INLINE
void rb3_set_red(struct rb3_head *head, int dir)
{
        head->child[dir] |= RB3_COLOR_BIT;
}

static RB3_INLINE
void rb3_set_black(struct rb3_head *head, int dir)
{
        head->child[dir] &= ~RB3_COLOR_BIT;
}

static RB3_INLINE
void rb3_connect(struct rb3_head *head, int dir, struct rb3_head *child, int color)
{
        head->child[dir] = RB3_CHILD_PTR(child, color);
        child->parent = RB3_PARENT_PTR(head, dir);
}

static RB3_INLINE
void rb3_connect_null(struct rb3_head *head, int dir, struct rb3_head *child, int color)
{
        head->child[dir] = RB3_CHILD_PTR(child, color);
        if (child)
                child->parent = RB3_PARENT_PTR(head, dir);
}

RB3_API
struct rb3_tree *rb3_get_containing_tree(struct rb3_head *head)
{
        while (rb3_get_parent(head))
                head = rb3_get_parent(head);
        return (struct rb3_tree *) ((char *) head - (RB3_offsetof(struct rb3_head, child[0])));
}

RB3_API
struct rb3_head *rb3_get_min(struct rb3_tree *tree)
{
        struct rb3_head *head;

        head = rb3_get_root(tree);
        if (!head)
                return RB3_NULL;
        while (rb3_has_child(head, RB3_LEFT))
                head = rb3_get_child(head, RB3_LEFT);
        return head;
}

RB3_API
struct rb3_head *rb3_get_max(struct rb3_tree *tree)
{
        struct rb3_head *head;

        head = rb3_get_root(tree);
        if (!head)
                return RB3_NULL;
        while (rb3_has_child(head, RB3_RIGHT))
                head = rb3_get_child(head, RB3_RIGHT);
        return head;
}

RB3_API
struct rb3_head *rb3_get_prev(struct rb3_head *head)
{
        if (rb3_has_child(head, RB3_LEFT))
                return rb3_get_prev_descendant(head);
        else
                return rb3_get_prev_ancestor(head);
}

RB3_API
struct rb3_head *rb3_get_next(struct rb3_head *head)
{
        if (rb3_has_child(head, RB3_RIGHT))
                return rb3_get_next_descendant(head);
        else
                return rb3_get_next_ancestor(head);
}

RB3_API
struct rb3_head *rb3_get_prev_ancestor(struct rb3_head *head)
{
        while (head && rb3_get_parent_dir(head) == RB3_LEFT)
                head = rb3_get_parent(head);
        if (head)
                head = rb3_get_parent(head);
        return head;
}

RB3_API
struct rb3_head *rb3_get_next_ancestor(struct rb3_head *head)
{
        while (head && rb3_get_parent_dir(head) == RB3_RIGHT)
                head = rb3_get_parent(head);
        head = rb3_get_parent(head);
        if (head && !rb3_get_parent(head))
                /* base fake element */
                head = RB3_NULL;
        return head;
}

RB3_API
struct rb3_head *rb3_get_prev_descendant(struct rb3_head *head)
{
        head = rb3_get_child(head, RB3_LEFT);
        if (!head)
                return RB3_NULL;
        while (rb3_has_child(head, RB3_RIGHT))
                head = rb3_get_child(head, RB3_RIGHT);
        return head;
}

RB3_API
struct rb3_head *rb3_get_next_descendant(struct rb3_head *head)
{
        head = rb3_get_child(head, RB3_RIGHT);
        if (!head)
                return RB3_NULL;
        while (rb3_has_child(head, RB3_LEFT))
                head = rb3_get_child(head, RB3_LEFT);
        return head;
}

/* insert implementation */

static RB3_NEVERINLINE
void rb3_insert_rebalance(struct rb3_head *head)
{
        struct rb3_head *pnt;
        struct rb3_head *gpnt;
        struct rb3_head *ggpnt;
        int left;
        int right;
        int gdir;
        int ggdir;

        if (!rb3_get_parent(rb3_get_parent(head))) {
                rb3_set_black(rb3_get_parent(head), RB3_LEFT);
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
                rb3_connect_null(pnt, left, rb3_get_black_child(head, right), RB3_BLACK);
                rb3_connect_null(gpnt, right, rb3_get_black_child(head, left), RB3_BLACK);
                rb3_connect(head, left, gpnt, RB3_RED);
                rb3_connect(head, right, pnt, RB3_RED);
                rb3_connect(ggpnt, ggdir, head, RB3_BLACK);
        } else {
                rb3_connect_null(gpnt, left, rb3_get_black_child(pnt, right), RB3_BLACK);
                rb3_connect(pnt, right, gpnt, RB3_RED);
                rb3_connect(ggpnt, ggdir, pnt, RB3_BLACK);
        }
}

RB3_API RB3_NEVERINLINE
void rb3_insert_below(struct rb3_head *head, struct rb3_head *parent, int dir)
{
        RB3_ASSERT(dir == RB3_LEFT || dir == RB3_RIGHT);
        RB3_ASSERT(rb3_get_child(parent, dir));

        parent->child[dir] = RB3_CHILD_PTR(head, RB3_RED);
        head->parent = RB3_PARENT_PTR(parent, dir);
        head->child[RB3_LEFT] = RB3_CHILD_PTR(RB3_NULL, RB3_BLACK);
        head->child[RB3_RIGHT] = RB3_CHILD_PTR(RB3_NULL, RB3_BLACK);
        rb3_insert_rebalance(head);
}

/* delete implementation */

static RB3_NEVERINLINE
void rb3_delete_rebalance(struct rb3_head *head)
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
                return;

        if (!rb3_get_parent(rb3_get_parent(head)))
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
                rb3_connect_null(pnt, right, sleft, rb3_get_color_bit(sibling, left));
                rb3_connect(sibling, left, pnt, RB3_BLACK);
                rb3_connect(gpnt, gdir, sibling, rb3_get_color_bit(gpnt, gdir));
                rb3_set_black(sibling, right);
        } else if (rb3_is_red(sibling, left)) {
                /* inner child of sibling is red */
                sleftleft = rb3_get_child(sleft, left);
                sleftright = rb3_get_child(sleft, right);
                rb3_connect_null(pnt, right, sleftleft, RB3_BLACK);
                rb3_connect_null(sibling, left, sleftright, RB3_BLACK);
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

static RB3_NEVERINLINE
void rb3_delete_noninternal(struct rb3_head *head)
{
        struct rb3_head *pnt;
        struct rb3_head *cld;
        int pdir;
        int dir;

        dir = rb3_get_child(head, RB3_RIGHT) ? RB3_RIGHT : RB3_LEFT;

        pnt = rb3_get_parent(head);
        cld = rb3_get_child(head, dir);
        pdir = rb3_get_parent_dir(head);

        if (!rb3_is_red(pnt, pdir) && !rb3_is_red(head, dir))
                /* To be deleted node is black (and child cannot be repainted)
                 * => height decreased */
                rb3_delete_rebalance(head);

        rb3_connect_null(pnt, pdir, cld, RB3_BLACK);
}

static RB3_NEVERINLINE
void rb3_delete_internal(struct rb3_head *head)
{
        struct rb3_head *subst;
        struct rb3_head *left;
        struct rb3_head *right;
        struct rb3_head *parent;
        int pdir;
        int pcol;

        subst = rb3_get_next_descendant(head);
        rb3_delete_noninternal(subst);
        *subst = *head;

        left = rb3_get_child(head, RB3_LEFT);
        right = rb3_get_child(head, RB3_RIGHT);
        parent = rb3_get_parent(head);
        pdir = rb3_get_parent_dir(head);
        pcol = rb3_get_color_bit(parent, pdir);

        if (left)
                left->parent = RB3_PARENT_PTR(subst, RB3_LEFT);
        if (right)
                right->parent = RB3_PARENT_PTR(subst, RB3_RIGHT);
        parent->child[pdir] = RB3_CHILD_PTR(subst, pcol);
}

RB3_API RB3_NEVERINLINE
void rb3_delete_head(struct rb3_head *head)
{
        if (rb3_has_child(head, RB3_LEFT) && rb3_has_child(head, RB3_RIGHT))
                rb3_delete_internal(head);
        else
                rb3_delete_noninternal(head);

        /* We would really like to write
            *head = (struct rb3_head) {0};
          how it's supposed to be, but compilers spew warnings without end.
          
          So we go with the slower and bigger-in-codesize version */
        head->child[RB3_LEFT] = 0;
        head->child[RB3_RIGHT] = 0;
        head->parent = 0;
}

/* node-find implementations using code from inline header functions */

RB3_API RB3_NEVERINLINE
struct rb3_head *rb3_find_in_subtree_datacmp(struct rb3_head *subtree, rb3_datacmp cmp, void *data)
{
        return rb3_INLINE_find_in_subtree(subtree, cmp, data);
}

RB3_API RB3_NEVERINLINE
struct rb3_head *rb3_find_parent_in_subtree_datacmp(struct rb3_head *parent, int dir, rb3_datacmp cmp, void *data, struct rb3_head **parent_out, int *dir_out)
{
        return rb3_INLINE_find_parent_in_subtree(parent, dir, cmp, data, parent_out, dir_out);
}

/* find, insert, delete with rb3_datacmp */

RB3_API RB3_NEVERINLINE
struct rb3_head *rb3_insert_datacmp(struct rb3_head *head, struct rb3_tree *tree, rb3_datacmp cmp, void *data)
{
        struct rb3_head *found;
        struct rb3_head *parent;
        int dir;

        parent = rb3_get_base(tree);
        dir = RB3_LEFT;
        found = rb3_find_parent_in_subtree_datacmp(parent, dir, cmp, data, &parent, &dir);
        if (found)
                return found;
        rb3_insert_below(head, parent, dir);
        return RB3_NULL;
}

RB3_API RB3_NEVERINLINE
struct rb3_head *rb3_delete_datacmp(struct rb3_tree *tree, rb3_datacmp cmp, void *data)
{
        struct rb3_head *found;
        
        found = rb3_find_in_subtree_datacmp(rb3_get_root(tree), cmp, data);
        if (found) {
                rb3_delete_head(found);
                return found;
        }
        return RB3_NULL;
}

RB3_API RB3_NEVERINLINE
struct rb3_head *rb3_find_datacmp(struct rb3_tree *tree, rb3_datacmp cmp, void *data)
{
        return rb3_find_in_subtree_datacmp(rb3_get_root(tree), cmp, data);
}

/* find, insert, delete with rb3_cmp */

RB3_API RB3_NEVERINLINE
struct rb3_head *rb3_insert(struct rb3_head *head, struct rb3_tree *tree, rb3_cmp cmp)
{
        return rb3_insert_datacmp(head, tree, (rb3_datacmp) cmp, head);
}

RB3_API RB3_NEVERINLINE
struct rb3_head *rb3_delete(struct rb3_tree *tree, rb3_cmp cmp, struct rb3_head *head)
{
        return rb3_delete_datacmp(tree, (rb3_datacmp) cmp, head);
}

RB3_API RB3_NEVERINLINE
struct rb3_head *rb3_find(struct rb3_tree *tree, rb3_cmp cmp, struct rb3_head *head)
{
        return rb3_find_datacmp(tree, (rb3_datacmp) cmp, head);
}

/* */

RB3_API RB3_NEVERINLINE RB3_COLD
void rb3_init(struct rb3_tree *tree)
{
        /* We would really like to write
            *head = (struct rb3_head) {0};
          how it's supposed to be, but compilers spew warnings without end.
          
          So we go with the slower and bigger-in-codesize version */
        tree->base.child[RB3_LEFT] = 0;
        tree->base.child[RB3_RIGHT] = 0;
        tree->base.parent = 0;
}

RB3_API RB3_NEVERINLINE RB3_COLD
void rb3_exit(struct rb3_tree *tree)
{
        /* No resources allocated */
        (void) tree;
}
