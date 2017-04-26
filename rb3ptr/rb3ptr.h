/*
 * Copyright (c) 2017 Jens Stimpfle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef RB3_HEADER_INCLUDED
#error rb3ptr.h included twice!
#endif

#define RB3_UNUSED __attribute__((unused))
#define RB3_HEADER_INCLUDED

typedef unsigned long rb3_ptr;

enum {
        RB3_LEFT = 0,
        RB3_RIGHT = 1,
        RB3_PARENT = 2,
};

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
 * Get (left or right) child
 */
static RB3_UNUSED
struct rb3_head *rb3_get_child(struct rb3_head *head, int dir)
{
        return (struct rb3_head *)((head->ptr[dir]) & ~3);
}

/*
 * Get parent head
 */
static RB3_UNUSED
struct rb3_head *rb3_get_parent(struct rb3_head *head)
{
        return (struct rb3_head *)(head->ptr[RB3_PARENT] & ~3);
}

/*
 * Get topmost element of tree (or NULL if empty)
 */
static RB3_UNUSED
struct rb3_head *rb3_get_root(struct rb3_tree *tree)
{
        return rb3_get_child(&tree->base, RB3_LEFT);
}

/*
 * Get minimum element in tree (or NULL if empty)
 */
static RB3_UNUSED
struct rb3_head *rb3_get_min(struct rb3_tree *tree)
{
        struct rb3_head *head;

        head = rb3_get_root(tree);
        if (!head)
                return NULL;
        while (rb3_get_child(head, RB3_LEFT))
                head = rb3_get_child(head, RB3_LEFT);
        return head;
}

/*
 * Get maximum element in tree (or NULL if empty)
 */
static RB3_UNUSED
struct rb3_head *rb3_get_max(struct rb3_tree *tree)
{
        struct rb3_head *head;

        head = rb3_get_root(tree);
        if (!head)
                return NULL;
        while (rb3_get_child(head, RB3_RIGHT))
                head = rb3_get_child(head, RB3_RIGHT);
        return head;
}

/*
 * Get direction from parent to child.
 *
 * Return RB3_LEFT when the parent sorts after the given element.
 * Return RB3_RIGHT when the parent sorts before the given element.
 *
 * If the given node is the root node, RB3_LEFT is returned (this is an
 * implementation detail. Client can test for the root node before calling this
 * function).
 */
static int rb3_get_parent_dir(struct rb3_head *head)
{
        return (head)->ptr[RB3_PARENT] & 1;
}

/*
 * Get in-order ascendant successor (minimal ancestor node that sorts after
 * the given element) or NULL if no such element is in the tree.
 */
static RB3_UNUSED
struct rb3_head *rb3_get_ascendant_successor(struct rb3_head *head)
{
        while (head && rb3_get_parent_dir(head) == RB3_RIGHT)
                head = rb3_get_parent(head);
        if (head) {
                head = rb3_get_parent(head);
                if (head && !rb3_get_parent(head))
                        /* base fake element */
                        head = NULL;
        }
        return head;
}

/*
 * Get in-order ascendant predecessor (maximal ancestor node that sorts before
 * the given element) or NULL if no such element is in the tree.
 */
static RB3_UNUSED
struct rb3_head *rb3_get_ascendant_predecessor(struct rb3_head *head)
{
        while (head && rb3_get_parent_dir(head) == RB3_LEFT)
                head = rb3_get_parent(head);
        if (head)
                head = rb3_get_parent(head);
        return head;
}

/*
 * Get in-order descendant successor (minimal descendant node that sorts after
 * the given element) or NULL if no such element is in the tree.
 */
static RB3_UNUSED
struct rb3_head *rb3_get_descendant_successor(struct rb3_head *head)
{
        head = rb3_get_child(head, RB3_RIGHT);
        if (!head)
                return NULL;
        while (rb3_get_child(head, RB3_LEFT))
                head = rb3_get_child(head, RB3_LEFT);
        return head;
}

/*
 * Get in-order descendant predecessor (minimal descendant node that sorts after
 * the given element) or NULL if no such element is in the tree.
 */
static RB3_UNUSED
struct rb3_head *rb3_get_descendant_predecessor(struct rb3_head *head)
{
        head = rb3_get_child(head, RB3_RIGHT);
        if (!head)
                return NULL;
        while (rb3_get_child(head, RB3_LEFT))
                head = rb3_get_child(head, RB3_LEFT);
        return head;
}

/*
 * Get in-order successor (or NULL if given element is maximum element).
 */
static RB3_UNUSED
struct rb3_head *rb3_get_inorder_successor(struct rb3_head *head)
{
        if (rb3_get_child(head, RB3_RIGHT))
                return rb3_get_descendant_successor(head);
        else
                return rb3_get_ascendant_successor(head);
}

/*
 * Get in-order predecessor (or NULL if given element is maximum element).
 */
static RB3_UNUSED
struct rb3_head *rb3_get_inorder_predecessor(struct rb3_head *head)
{
        if (rb3_get_child(head, RB3_RIGHT))
                return rb3_get_descendant_predecessor(head);
        else
                return rb3_get_ascendant_predecessor(head);
}

void rb3_init(struct rb3_tree *tree);
void rb3_exit(struct rb3_tree *tree);
void rb3_insert(struct rb3_head *head, struct rb3_head *parent, int dir);
void rb3_delete(struct rb3_head *head);

/* debug */
int rb3_is_valid_tree(struct rb3_head *head, int isred);
void rb3_inorder_traversal(struct rb3_head *head, void (*action)(struct rb3_head *, void *), void *arg);
