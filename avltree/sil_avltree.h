/*
 * Copyright (C) 2017 Jens Stimpfle
 *
 * License: MIT
 *
 * Usage: You need to define some SIL_AVL_ARG_* variables before including this
 * file. The benchmark code in bench.c has an example.
 */

#include <assert.h>
#include <stddef.h>  /* NULL */

#ifndef SIL_AVL_ARG_NAME
#error "SIL_AVL_ARG_NAME" must be defined before including this file
#endif

#ifndef SIL_AVL_ARG_TYPE
#error "SIL_AVL_ARG_TYPE" must be defined before including this file
#endif

#ifndef SIL_AVL_ARG_MEMBER
#error "SIL_AVL_ARG_MEMBER" must be defined before including this file
#endif

#ifndef SIL_AVL_ARG_COMPARE
#error "SIL_AVL_ARG_COMPARE" must be defined before including this file
#endif

/* you can also define SIL_AVL_ARG_PACKHEAD, see below */

#ifndef SIL_AVL_SECOND_PASS
#define SIL_AVL_SECOND_PASS /* go to the other branch next time */

#if SIL_AVL_ARG_PACKHEAD
/*
 * NOTE (2017-04): I have tested the effects of saving 6 bytes in the link
 * structure by making the child heights lh and rh unsigned chars (2*1byte) and
 * specifiying __attribute__((packed)). On my x86-64 it gives a considerable
 * performance advantage (0.77s -> 0.72s on insert, 0.66s -> 0.62s on find).
 * However I don't know how portable or safe it is.
 */
#define SIL_AVL_HEADATTRIBUTES __attribute__((packed))
#else
#define SIL_AVL_HEADATTRIBUTES
#endif

#define SIL_AVL_TYPE SIL_AVL_ARG_TYPE
#define SIL_AVL_NAME SIL_AVL_ARG_NAME
#define SIL_AVL_MEMBER SIL_AVL_ARG_MEMBER
#define SIL_AVL_COMPARE SIL_AVL_ARG_COMPARE

#define SIL_AVL_HEAD(data) (&(node)->SIL_AVL_MEMBER)
#define SIL_AVL_DATA(head) ((SIL_AVL_TYPE *)((char *)(head) - offsetof(SIL_AVL_TYPE, SIL_AVL_MEMBER)))
#define SIL_AVL_MAYBEDATA(head) ((head) ? SIL_AVL_DATA(head) : NULL)
#define SIL_AVL_PARENTS_CHILDHEIGHT(head) (*SIL_AVL_get_parents_childheightptr(head))

#define SIL_AVL_REALPASTE(a,b) a##b
#define SIL_AVL_PASTE(a,b) SIL_AVL_REALPASTE(a,b)
#define SIL_AVL_UNUSED __attribute__((unused))

#define SIL_AVL_INIT SIL_AVL_PASTE(SIL_AVL_NAME,_init)
#define SIL_AVL_EXIT SIL_AVL_PASTE(SIL_AVL_NAME,_exit)
#define SIL_AVL_ISEMPTY (SIL_AVL_PASTE(SIL_AVL_NAME,_isempty))
#define SIL_AVL_GETHEIGHT (SIL_AVL_PASTE(SIL_AVL_NAME,_getheight))
#define SIL_AVL_INSERT SIL_AVL_PASTE(SIL_AVL_NAME,_insert)
#define SIL_AVL_FIND SIL_AVL_PASTE(SIL_AVL_NAME,_find)
#define SIL_AVL_ERASE (SIL_AVL_PASTE(SIL_AVL_NAME,_erase))
#define SIL_AVL_GETROOT SIL_AVL_PASTE(SIL_AVL_NAME,_get_root)
#define SIL_AVL_GETLEFTMOSTDESCENDANT SIL_AVL_PASTE(SIL_AVL_NAME,_get_leftmost_descendant)
#define SIL_AVL_GETRIGHTMOSTDESCENDANT SIL_AVL_PASTE(SIL_AVL_NAME,_get_rightmost_descendant)

/*
 * Linkage and height information for AVL nodes.
 *
 * The client code must include this structure in its SIL_AVL_ARG_TYPE
 * structure.
 */
struct SIL_AVL_HEADATTRIBUTES SIL_AVLhead {
        struct SIL_AVLhead *left;
        struct SIL_AVLhead *right;
        struct SIL_AVLhead *parent;
        /*
         * Heights of the left and right childs. The idea is to be cache
         * friendly. We store these here for cache-efficiency.
         */
        unsigned char lh;
        unsigned char rh;
};

#else /* defined(SIL_AVL_SECOND_PASS) */

struct SIL_AVL_NAME {
        /*
         * special head with convention:
         *   parent = NULL
         *   left = root of tree
         *   right = NULL
         */
        struct SIL_AVLhead base;
};

static int SIL_AVL_is_empty(struct SIL_AVL_NAME *tree)
{
        return !tree->base.left;
}

static int SIL_AVL_is_base(struct SIL_AVLhead *head)
{
        return !head->parent;
}

static int SIL_AVL_UNUSED SIL_AVL_is_leaf(struct SIL_AVLhead *head)
{
        return !head->left && !head->right;
}

/*
static struct SIL_AVLhead *SIL_AVL_get_root(struct SIL_AVLhead *head)
{
        while (!SIL_AVL_is_base(head->parent))
                head = head->parent;
        return head;
}
*/

/*
static struct SIL_AVLhead *SIL_AVL_get_leftmost_descendant(struct SIL_AVLhead *head)
{
        while (head->left)
                head = head->left;
        return head;
}
*/

static struct SIL_AVLhead *SIL_AVL_get_rightmost_descendant(struct SIL_AVLhead *head)
{
        while (head->right)
                head = head->right;
        return head;
}

static int SIL_AVL_get_height(struct SIL_AVLhead *head)
{
        if (!head)
                return 0;
        if (head->lh < head->rh)
                return 1 + head->rh;
        else
                return 1 + head->lh;
}

static int SIL_AVL_get_balance(struct SIL_AVLhead *head)
{
        /*return SIL_AVL_get_height(head->right) - SIL_AVL_get_height(head->left);
         */
        int b = head->rh - head->lh;
        assert(-2 <= b && b <= 2);
        return b;
}

static unsigned char *SIL_AVL_get_parents_childheightptr(struct SIL_AVLhead *head)
{
        if (head->parent->left == head)
                return &head->parent->lh;
        assert(head->parent->right == head);
        return &head->parent->rh;
}

static void assert_valid_parentlink(struct SIL_AVLhead *head)
{
        if (!head)
                return;

        struct SIL_AVLhead *SIL_AVL_UNUSED parent = head->parent;
        assert(parent->right == head || parent->left == head);
}

static void SIL_AVL_UNUSED assert_valid_parentlinks_in_tree(struct SIL_AVLhead *head)
{
        assert_valid_parentlink(head);
        if (head->left)
                assert_valid_parentlinks_in_tree(head->left);
        if (head->right)
                assert_valid_parentlinks_in_tree(head->right);
}

/*
 * Rotation case 1 ("single rotation"), in right direction
 */
static struct SIL_AVLhead *SIL_AVL_rotate_right_1(struct SIL_AVLhead *head)
{
        struct SIL_AVLhead *parent = head->parent;
        struct SIL_AVLhead *b = head->left;
        struct SIL_AVLhead *c = head->left->right;
        struct SIL_AVLhead *d = head;

        assert_valid_parentlink(b);
        assert_valid_parentlink(c);
        assert_valid_parentlink(d);

        d->left = c;
        d->parent = b;
        d->lh = b->rh;

        b->right = d;
        b->parent = parent;
        b->rh += 1;

        if (c)
                c->parent = d;

        if (parent->left == head)
                parent->left = b;
        else {
                assert(parent->right == head);
                parent->right = b;
        }

        assert_valid_parentlink(b);
        assert_valid_parentlink(c);
        assert_valid_parentlink(d);

        return b;
}

/*
 * Rotation case 1 ("single rotation"), in left direction
 */
static struct SIL_AVLhead *SIL_AVL_rotate_left_1(struct SIL_AVLhead *head)
{
        struct SIL_AVLhead *parent = head->parent;
        struct SIL_AVLhead *b = head->right;
        struct SIL_AVLhead *c = head->right->left;
        struct SIL_AVLhead *d = head;

        assert_valid_parentlink(b);
        assert_valid_parentlink(c);
        assert_valid_parentlink(d);

        d->right = c;
        d->parent = b;
        d->rh = b->lh;

        b->left = d;
        b->parent = parent;
        b->lh += 1;

        if (c)
                c->parent = d;

        if (parent->left == head)
                parent->left = b;
        else {
                assert(parent->right == head);
                parent->right = b;
        }

        assert_valid_parentlink(b);
        assert_valid_parentlink(c);
        assert_valid_parentlink(d);

        return b;
}

/*
 * Rotation case 2 ("rotation after child rotation"), in right direction
 */
static struct SIL_AVLhead *SIL_AVL_rotate_right_2(struct SIL_AVLhead *head)
{
        struct SIL_AVLhead *parent = head->parent;
        struct SIL_AVLhead *b = head->left;
        struct SIL_AVLhead *c = head->left->right;
        struct SIL_AVLhead *cx = head->left->right->left;
        struct SIL_AVLhead *cy = head->left->right->right;
        struct SIL_AVLhead *d = head;

        assert_valid_parentlink(b);
        assert_valid_parentlink(c);
        assert_valid_parentlink(cx);
        assert_valid_parentlink(cy);
        assert_valid_parentlink(d);

        b->right = cx;
        b->parent = c;
        b->rh = c->lh;

        if (cx)
                cx->parent = b;

        d->left = cy;
        d->parent = c;
        d->lh = c->rh;

        if (cy)
                cy->parent = d;

        c->left = b;
        c->right = d;
        c->parent = parent;
        c->lh = b->lh + 1;
        c->rh = d->rh + 1;

        if (parent->left == head)
                parent->left = c;
        else {
                assert(parent->right == head);
                parent->right = c;
        }

        assert_valid_parentlink(b);
        assert_valid_parentlink(c);
        assert_valid_parentlink(cx);
        assert_valid_parentlink(cy);
        assert_valid_parentlink(d);

        return c;
}

/*
 * Rotation case 2 ("rotation after child rotation"), in left direction
 */
static struct SIL_AVLhead *SIL_AVL_rotate_left_2(struct SIL_AVLhead *head)
{
        struct SIL_AVLhead *parent = head->parent;
        struct SIL_AVLhead *b = head->right;
        struct SIL_AVLhead *c = head->right->left;
        struct SIL_AVLhead *cx = head->right->left->right;
        struct SIL_AVLhead *cy = head->right->left->left;
        struct SIL_AVLhead *d = head;

        assert_valid_parentlink(b);
        assert_valid_parentlink(c);
        assert_valid_parentlink(cx);
        assert_valid_parentlink(cy);
        assert_valid_parentlink(c);
        assert_valid_parentlink(d);

        b->left = cx;
        b->parent = c;
        b->lh = c->rh;

        if (cx)
                cx->parent = b;

        d->right = cy;
        d->parent = c;
        d->rh = c->lh;

        if (cy)
                cy->parent = d;

        c->right = b;
        c->left = d;
        c->parent = parent;
        c->rh = b->rh + 1;
        c->lh = d->lh + 1;

        if (parent->right == head)
                parent->right = c;
        else {
                assert(parent->left == head);
                parent->left = c;
        }

        assert_valid_parentlink(b);
        assert_valid_parentlink(c);
        assert_valid_parentlink(cx);
        assert_valid_parentlink(cy);
        assert_valid_parentlink(d);

        return c;
}

static void SIL_AVL_child_height_changed(struct SIL_AVLhead *head)
{
        int bal;
        int height;

        while (!SIL_AVL_is_base(head)) {
                bal = SIL_AVL_get_balance(head);
                if (bal == -2) {
                        if (SIL_AVL_get_balance(head->left) <= 0) {
                                head = SIL_AVL_rotate_right_1(head);
                        } else {
                                assert(SIL_AVL_get_balance(head->left) == 1);
                                head = SIL_AVL_rotate_right_2(head);
                        }
                        assert_valid_parentlink(head);
                } else if (bal == 2) {
                        if (SIL_AVL_get_balance(head->right) >= 0) {
                                head = SIL_AVL_rotate_left_1(head);
                        } else {
                                assert(SIL_AVL_get_balance(head->right) == -1);
                                head = SIL_AVL_rotate_left_2(head);
                        }
                        assert_valid_parentlink(head);
                } else {
                        assert(-1 <= bal && bal <= 1);
                }
                height = SIL_AVL_get_height(head);
                if (SIL_AVL_PARENTS_CHILDHEIGHT(head) == height)
                        break;
                SIL_AVL_PARENTS_CHILDHEIGHT(head) = height;
                head = head->parent;
        }
}

/*
 * Replace a linked node with an unlinked node.
 */
static void SIL_AVL_replace_node(struct SIL_AVLhead *head, struct SIL_AVLhead *subst)
{
        assert(head != subst);

        subst->left = head->left;
        subst->right = head->right;
        subst->parent = head->parent;
        subst->lh = head->lh;
        subst->rh = head->rh;

        if (head->left)
                head->left->parent = subst;
        if (head->right)
                head->right->parent = subst;

        if (head->parent->left == head)
                head->parent->left = subst;
        else {
                assert(head->parent->right == head);
                head->parent->right = subst;
        }

        /*
        head->left = NULL;
        head->right = NULL;
        head->parent = NULL;
        head->lh = 0;
        head->rh = 0;
        */
}

/*
 * Erase a leaf
 */
void SIL_AVL_erase_leaf(struct SIL_AVLhead *head)
{
        struct SIL_AVLhead *parent;

        parent = head->parent;
        if (parent->left == head) {
                parent->left = NULL;
                parent->lh = 0;
        } else {
                assert(parent->right == head);
                parent->right = NULL;
                parent->rh = 0;
        }
        SIL_AVL_child_height_changed(head->parent);
}

static void SIL_AVL_erase_lefthalfleaf(struct SIL_AVLhead *head)
{
        struct SIL_AVLhead *parent = head->parent;
        struct SIL_AVLhead *child = head->right;
        int childheight = head->rh;

        if (parent->left == head) {
                parent->left = child;
                parent->lh = childheight;
        } else {
                assert(parent->right == head);
                parent->right = child;
                parent->rh = childheight;
        }
        if (child)
                child->parent = parent;
        SIL_AVL_child_height_changed(parent);
}

static void SIL_AVL_erase_righthalfleaf(struct SIL_AVLhead *head)
{
        struct SIL_AVLhead *parent = head->parent;
        struct SIL_AVLhead *child = head->left;
        int childheight = head->lh;

        if (parent->left == head) {
                parent->left = child;
                parent->lh = childheight;
        } else {
                assert(parent->right == head);
                parent->right = child;
                parent->rh = childheight;
        }
        if (child)
                child->parent = parent;
        SIL_AVL_child_height_changed(parent);
}

/*
 * Erase an internal node
 */
static void SIL_AVL_erase_internal(struct SIL_AVLhead *head)
{
        struct SIL_AVLhead *subst;

        subst = SIL_AVL_get_rightmost_descendant(head->left);
        SIL_AVL_erase_righthalfleaf(subst);

        SIL_AVL_replace_node(head, subst);
}

static void SIL_AVL_erase(struct SIL_AVLhead *head)
{
        if (!head->left && !head->right) {
                SIL_AVL_erase_leaf(head);
        } else if (!head->left && head->right) {
                assert(SIL_AVL_is_leaf(head->right));
                SIL_AVL_erase_lefthalfleaf(head);
        } else if (head->left && !head->right) {
                assert(SIL_AVL_is_leaf(head->left));
                SIL_AVL_erase_righthalfleaf(head);
        } else if (head->left && head->right) {
                SIL_AVL_erase_internal(head);
        }
}

static struct SIL_AVLhead *SIL_AVL_find(struct SIL_AVLhead *head, struct SIL_AVLhead *parent)
{
        int r;

        while (parent) {
                r = SIL_AVL_COMPARE(SIL_AVL_DATA(parent), SIL_AVL_DATA(head));
                if (r < 0)
                        parent = parent->left;
                else if (r > 0)
                        parent = parent->right;
                else
                        return parent;
        }
        return NULL;
}

/*
 * Find the location where "head" must be linked in a subtree.
 *
 * *parent must be the parent of the subtree where head belongs (according to
 * the tree ordering), and *child must be that parent's child pointer (either
 * &(*parent)->left or &(*parent)->right).
 *
 * When this function returns, the parent and location of linkage of head is
 * contained in *parent and *child.
 */
static void SIL_AVL_findbest(struct SIL_AVLhead *head, struct SIL_AVLhead **parent, struct SIL_AVLhead ***child)
{
        int r;

        struct SIL_AVLhead *p = *parent;
        struct SIL_AVLhead **c = *child;

        assert(p != NULL);
        assert(c == &p->left || c == &p->right);

        while (*c != NULL) {
                p = *c;
                r = SIL_AVL_COMPARE(SIL_AVL_DATA(p), SIL_AVL_DATA(head));
                if (r < 0)
                        c = &p->left;
                else if (r > 0)
                        c = &p->right;
                else
                        break;
        }

        *parent = p;
        *child = c;
}

static struct SIL_AVLhead *insert(struct SIL_AVL_NAME *tree, struct SIL_AVLhead *head)
{
        struct SIL_AVLhead *parent = &tree->base;
        struct SIL_AVLhead **child = &tree->base.left;

        SIL_AVL_findbest(head, &parent, &child);

        if (*child)
                /* Element found. Return without insertion */
                return *child;

        head->left = NULL;
        head->right = NULL;
        head->parent = parent;
        head->lh = 0;
        head->rh = 0;

        *child = head;
        SIL_AVL_PARENTS_CHILDHEIGHT(head) = 1;
        SIL_AVL_child_height_changed(parent);

        /* indicate that head was inserted */
        return NULL;
}


/*
 * VISIBLE GENERATED FUNCTIONS
 */

void SIL_AVL_INIT(struct SIL_AVL_NAME *tree)
{
        tree->base.left = NULL;
        tree->base.right = NULL;
        tree->base.parent = NULL;
        tree->base.lh = 0;
        tree->base.rh = 0;
}

void SIL_AVL_EXIT(struct SIL_AVL_NAME *tree)
{
        /* ? */
}

int SIL_AVL_ISEMPTY(struct SIL_AVL_NAME *tree)
{
        return SIL_AVL_is_empty(tree);
}

int SIL_AVL_GETHEIGHT(struct SIL_AVL_NAME *tree)
{
        return tree->base.lh;
}

SIL_AVL_TYPE *SIL_AVL_INSERT(struct SIL_AVL_NAME *tree, SIL_AVL_TYPE *node)
{
        struct SIL_AVLhead *hit;
        
        hit = insert(tree, SIL_AVL_HEAD(node));
        return SIL_AVL_MAYBEDATA(hit);
}

SIL_AVL_TYPE *SIL_AVL_FIND(struct SIL_AVL_NAME *tree, SIL_AVL_TYPE *node)
{
        struct SIL_AVLhead *hit;
        struct SIL_AVLhead *head = SIL_AVL_HEAD(node);

        hit = SIL_AVL_find(head, tree->base.left);
        return SIL_AVL_MAYBEDATA(hit);
}

void SIL_AVL_ERASE(SIL_AVL_TYPE *node)
{
        SIL_AVL_erase(SIL_AVL_HEAD(node));
}

#endif
