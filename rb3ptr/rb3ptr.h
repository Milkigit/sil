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

#ifndef _STDDEF_H
#error stddef.h must be included first!
#endif

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

/*
 * Node type for 3-pointer Red-black trees.
 */
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
 * Initialize a rb3_tree.
 */
void rb3_init(struct rb3_tree *tree);

/*
 * Free resources allocated by an rb3_tree (currently none, but this could
 * change).
 */
void rb3_exit(struct rb3_tree *tree);

/*
 * Insert a node below another node in the given direction (RB3_LEFT or
 * RB3_RIGHT). The new node must replace a leaf.
 */
void rb3_insert(struct rb3_head *head, struct rb3_head *parent, int dir);

/*
 * Delete a node.
 */
void rb3_delete(struct rb3_head *head);

/*
 * Get minimum (lefmost) element, or NULL if tree is empty
 */
struct rb3_head *rb3_get_min(struct rb3_tree *tree);

/*
 * Get maximum (rightmost) element, or NULL if tree is empty
 */
struct rb3_head *rb3_get_max(struct rb3_tree *tree);

/*
 * Get in-order ascendant successor (minimal ancestor node that sorts after
 * the given element) or NULL if no such element is in the tree.
 */
struct rb3_head *rb3_get_ascendant_successor(struct rb3_head *head);

/*
 * Get in-order ascendant predecessor (maximal ancestor node that sorts before
 * the given element) or NULL if no such element is in the tree.
 */
struct rb3_head *rb3_get_ascendant_predecessor(struct rb3_head *head);

/*
 * Get in-order descendant successor (minimal descendant node that sorts after
 * the given element) or NULL if no such element is in the tree.
 */
struct rb3_head *rb3_get_descendant_successor(struct rb3_head *head);

/*
 * Get in-order descendant predecessor (minimal descendant node that sorts after
 * the given element) or NULL if no such element is in the tree.
 */
struct rb3_head *rb3_get_descendant_predecessor(struct rb3_head *head);

/*
 * Get in-order successor (or NULL if given element is maximum element).
 */
struct rb3_head *rb3_get_inorder_successor(struct rb3_head *head);

/*
 * Get in-order predecessor (or NULL if given element is maximum element).
 */
struct rb3_head *rb3_get_inorder_predecessor(struct rb3_head *head);

/*
 * Test if a (left or right) child exists
 *
 * This slightly more efficient than calling rb3_get_child() and comparing to
 * NULL.
 */
static RB3_UNUSED
int rb3_has_child(struct rb3_head *head, int dir)
{
        return head->ptr[dir] != 0;
}

/*
 * Check if tree is empty
 */
static RB3_UNUSED
int rb3_isempty(struct rb3_tree *tree)
{
        return !rb3_has_child(&tree->base, RB3_LEFT);
}

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
 * Get fake base of tree
 *
 * Warning: this special element is not embedded in a payload structure. You may
 * only use it to insert an element into an empty tree (on the RB3_LEFT side of
 * this head).
 */
static RB3_UNUSED
struct rb3_head *rb3_get_base_head(struct rb3_tree *tree)
{
        return &tree->base;
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
 * Get direction from parent to child.
 *
 * Return RB3_LEFT when the parent sorts after the given element.
 * Return RB3_RIGHT when the parent sorts before the given element.
 *
 * If the given node is the root node, RB3_LEFT is returned (this is an
 * implementation detail. Client can test for the root node before calling this
 * function).
 */
static RB3_UNUSED
int rb3_get_parent_dir(struct rb3_head *head)
{
        return (head)->ptr[RB3_PARENT] & 1;
}
