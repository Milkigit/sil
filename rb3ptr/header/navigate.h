/*
 * ---------------------------------------------------------------------------
 * Navigational API
 *
 * These functions provide advanced functionality for navigation in a
 * binary search tree.
 * ---------------------------------------------------------------------------
 */

/*
 * Get topmost element of tree (or NULL if empty)
 *
 * Time complexity: O(1)
 */
RB3_API_INLINE
struct rb3_head *rb3_get_root(struct rb3_tree *tree);

/*
 * Get previous in-order ancestor (maximal ancestor node that sorts before the
 * given element) or NULL if no such element is in the tree.
 *
 * Time complexity: O(log n)
 */
RB3_API
struct rb3_head *rb3_get_prev_ancestor(struct rb3_head *head);

/*
 * Get next in-order ancestor (minimal ancestor node that sorts after the
 * given element) or NULL if no such element is in the tree.
 *
 * Time complexity: O(log n)
 */
RB3_API
struct rb3_head *rb3_get_next_ancestor(struct rb3_head *head);

/*
 * Get previous in-order descendant (maximal descendant node that sorts before
 * the given element) or NULL if no such element is in the tree.
 *
 * Time complexity: O(log n)
 */
RB3_API
struct rb3_head *rb3_get_prev_descendant(struct rb3_head *head);

/*
 * Get next in-order descendant (minimal descendant node that sorts after the
 * given element) or NULL if no such element is in the tree.
 *
 * Time complexity: O(log n)
 */
RB3_API
struct rb3_head *rb3_get_next_descendant(struct rb3_head *head);

/*
 * Inline implementations
 */

RB3_API_INLINE
struct rb3_head *rb3_get_root(struct rb3_tree *tree)
{
        return rb3_get_child(&tree->base, RB3_LEFT);
}
