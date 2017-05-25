/*
 * ---------------------------------------------------------------------------
 * Types
 *
 * This file provides the typedefs and struct definitions that are required
 * for the type-agnostic API.
 * ---------------------------------------------------------------------------
 */

/**
 * This type is used to efficiently store a pointer (at least 4-byte aligned)
 * and some more information in the unused low bits.
 */
typedef unsigned long rb3_ptr;

/**
 * Directions for navigation in the tree.
 */
enum {
        RB3_LEFT = 0,
        RB3_RIGHT = 1,
};

/**
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

/**
 * Tree type. It's just a fake base head that is wrapped for type safety and
 * future extensibility.
 */
struct rb3_tree {
        struct rb3_head base;
};

/**
 * User-provided comparison function. It is used during tree searches.
 * At each visited node, the function is called with that node as first
 * argument and some additional user-provided data.
 *
 * It should returns a value less than, equal to, or greater than, 0,
 * depending on whether the node compares less than, equal to, or greater
 * than, the user-provided data.
 */
typedef int (*rb3_cmp)(struct rb3_head *head, void *data);

/**
 * Get fake base of tree.
 *
 * Warning: the special base element is never embedded in a client payload
 * structure. It's just a link to host the real root of the tree as its left
 * child.
 */
_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_base(struct rb3_tree *tree);

/**
 * Test if given head is fake base of tree.
 */
_RB3_API_STATIC_INLINE
int rb3_is_base(struct rb3_head *head);

/**
 * Check if a non-base head is linked in a (any) tree.
 *
 * Time complexity: O(1)
 */
_RB3_API_STATIC_INLINE
int rb3_is_node_linked(struct rb3_head *head);

/**
 * Get direction from parent to child by testing the direction.
 *
 * Return RB3_LEFT or RB3_RIGHT, depending on whether this node is the left or
 * right child of its parent node. If the given node is the root node,
 * RB3_LEFT is returned. (Technically the root node is the left child of the
 * base node).
 *
 * This is more convenient and (in theory) more efficient than getting the
 * parent and testing its left and right child.
 */
_RB3_API_STATIC_INLINE
int rb3_get_parent_dir(struct rb3_head *head);

/**
 * Get parent head, or NULL if given node is the base node.
 *
 * Note that normally you don't want to visit the base node but stop already
 * at the root node.
 *
 * Time complexity: O(1)
 */
_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_parent(struct rb3_head *head);

/*
 * Test if a (left or right) child exists.
 *
 * This is slightly more efficient than calling rb3_get_child() and comparing
 * to NULL.
 *
 * Time complexity: O(1)
 */
_RB3_API_STATIC_INLINE
int rb3_has_child(struct rb3_head *head, int dir);

/**
 * Get child in given direction, or NULL if there is no such child. `dir`
 * must be RB3_LEFT or RB3_RIGHT.
 *
 * Time complexity: O(1)
 */
_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_child(struct rb3_head *head, int dir);

/*
 * ---------------------------------------------------------------------------
 * Inline implementations
 * ---------------------------------------------------------------------------
 */

_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_base(struct rb3_tree *tree)
{
        return &tree->base;
}

_RB3_API_STATIC_INLINE
int rb3_is_base(struct rb3_head *head)
{
        /*
         * We could check for the parent pointer being null, but by having
         * a special sentinel right child value instead, we can make this
         * function distinguish the base from unlinked pointers as well.
         *
         * A side effect is that this breaks programs with trees that are not
         * initialized with rb3_init(), which could be a good or a bad thing,
         * I don't know.
         */
	return head->child[RB3_RIGHT] == 3;
}

_RB3_API_STATIC_INLINE
int rb3_is_node_linked(struct rb3_head *head)
{
        return head->parent != 0;
}

_RB3_API_STATIC_INLINE
int rb3_get_parent_dir(struct rb3_head *head)
{
        return head->parent & 1;
}

_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_parent(struct rb3_head *head)
{
        return (struct rb3_head *)(head->parent & ~3);
}

_RB3_API_STATIC_INLINE
int rb3_has_child(struct rb3_head *head, int dir)
{
        return head->child[dir] != 0;
}

_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_child(struct rb3_head *head, int dir)
{
        return (struct rb3_head *)((head->child[dir]) & ~3);
}
