/*
 * ---------------------------------------------------------------------------
 * Types
 *
 * This file provides the typedefs and struct definitions that are required
 * for the type-agnostic API.
 * ---------------------------------------------------------------------------
 */

/*
 * This type is used to efficiently store a pointer (at least 4-byte aligned)
 * and some more information in the unused low bits.
 */
typedef unsigned long rb3_ptr;

/*
 * Directions for navigation in the tree.
 */
enum {
        RB3_LEFT = 0,
        RB3_RIGHT = 1,
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
        rb3_ptr child[2];
        rb3_ptr parent;
};

/*
 * Tree type. It's just a fake base head that is wrapped for type safety and
 * future extensibility.
 */
struct rb3_tree {
        struct rb3_head base;
};

/*
 * User-provided comparison function. It must first cast (and offset) the
 * pointers to compare the client-side structures that embed `a` and `b`.
 *
 * If a < b, returns some value < 0.
 * If a > b, returns some value > 0.
 * If a == b, returns 0.
 *
 * This function type is used by the implementation for deciding which child
 * direction to take to find insertion or deletion points in a tree.
 *
 * In some cases, non-strictly monotonic comparison functions (with regards to
 * the ordering of the nodes in a tree) make sense. An example is
 * rb3_find_first() which needs only a monotonic "predicate" to return the
 * first node in the tree that "compares" equal (i.e. where the "predicate"
 * returns 0).
 */
typedef int (*rb3_cmp)(struct rb3_head *a, struct rb3_head *b);

/*
 * User-provided search function. This is a more general variant of rb3_cmp
 * that supports more specialized searches in a tree.
 *
 * If head_in_tree is "smaller" than data, returns some value < 0.
 * If head_in_tree is "greater" than data, returns some value > 0.
 * If head_in_tree is "equal" to data, returns 0.
 *
 * As stated above this is a generalization of rb3_cmp. Conversely, rb3_cmp is
 * a specialization of rb3_pred that expects another struct rb3_head pointer
 * as `data` argument.
 *
 * This function type is required for some complex applications where
 * additional context is needed to make decisions. For example, in Fortune's
 * algorithm, the order of any two given intersection points is parameterized
 * by the moving beachlines. (The client code must make sure that nodes are
 * deleted from the tree before their ordering with respect to other elements
 * in the tree changes (rendering the tree invalid)).
 *
 * Another use case of rb3_datacmp is looking for nodes in a tree without any
 * reference node at all. An obvious example is searching the first node in a
 * tree of 2D coordinates that has x-value greater than a given reference
 * value.
 */
typedef int (*rb3_datacmp)(struct rb3_head *head_in_tree, void *data);

/*
 * Get direction from parent to child by testing the direction
 *
 * Return RB3_LEFT or RB3_RIGHT, depending on whether this node is the left or
 * right child of its parent node. If the given node is the root node,
 * RB3_LEFT is returned. (Technically the root node is the left child of the
 * base node).
 *
 * This is more convenient and (in theory) more efficient than getting the
 * parent and testing its left and right child.
 */
RB3_API_INLINE
int rb3_get_parent_dir(struct rb3_head *head);

/*
 * Get parent head, or NULL if given node is the base node.
 *
 * Note that normally you don't want to visit the base node but stop already
 * at the root node.
 *
 * Time complexity: O(1)
 */
RB3_API_INLINE
struct rb3_head *rb3_get_parent(struct rb3_head *head);

/*
 * Test if a (left or right) child exists
 *
 * This is slightly more efficient than calling rb3_get_child() and comparing
 * to NULL.
 *
 * Time complexity: O(1)
 */
RB3_API_INLINE
int rb3_has_child(struct rb3_head *head, int dir);

/*
 * Get child in given direction, or NULL if there is no such child. `dir`
 * must be RB3_LEFT or RB3_RIGHT.
 *
 * Time complexity: O(1)
 */
RB3_API_INLINE
struct rb3_head *rb3_get_child(struct rb3_head *head, int dir);

/*
 * Get fake base of tree.
 *
 * Warning: the special base element is never embedded in a client payload
 * structure. It's just a link to host the real root of the tree as its left
 * child.
 */
RB3_API_INLINE
struct rb3_head *rb3_get_base(struct rb3_tree *tree);

/*
 * ---------------------------------------------------------------------------
 * Inline implementations
 * ---------------------------------------------------------------------------
 */

RB3_API_INLINE
int rb3_is_base(struct rb3_head *head)
{
	return !head->parent;
}

RB3_API_INLINE
int rb3_get_parent_dir(struct rb3_head *head)
{
        return head->parent & 1;
}

RB3_API_INLINE
struct rb3_head *rb3_get_parent(struct rb3_head *head)
{
        return (struct rb3_head *)(head->parent & ~3);
}

RB3_API_INLINE
int rb3_has_child(struct rb3_head *head, int dir)
{
        return head->child[dir] != 0;
}

RB3_API_INLINE
struct rb3_head *rb3_get_child(struct rb3_head *head, int dir)
{
        return (struct rb3_head *)((head->child[dir]) & ~3);
}

RB3_API_INLINE
struct rb3_head *rb3_get_base(struct rb3_tree *tree)
{
        return &tree->base;
}
