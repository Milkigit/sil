/*
 * ---------------------------------------------------------------------------
 * BASIC API
 *
 * These functions provide basic usability as an abstract ordered container.
 * Often they are all you need to know.
 * ---------------------------------------------------------------------------
 */

/**
 * Initialize an rb3_tree.
 *
 * Time complexity: O(1)
 */
RB3_API
void rb3_init(struct rb3_tree *tree);

/**
 * Free resources allocated by an rb3_tree (currently none, but this could
 * change).
 */
RB3_API
void rb3_exit(struct rb3_tree *tree);

/**
 * Check if tree is empty.
 *
 * Time complexity: O(1)
 */
RB3_API_STATIC_INLINE
int rb3_isempty(struct rb3_tree *tree);

/**
 * Get minimum (leftmost) element, or NULL if tree is empty.
 *
 * Time complexity: O(log n)
 */
RB3_API
struct rb3_head *rb3_get_min(struct rb3_tree *tree);

/**
 * Get maximum (rightmost) element, or NULL if tree is empty
 *
 * Time complexity: O(log n)
 */
RB3_API
struct rb3_head *rb3_get_max(struct rb3_tree *tree);

/**
 * Get previous in-order node (maximal node in the tree that sorts before the
 * given element) or NULL if no such element is in the tree.
 *
 * Time complexity: O(log n), amortized over sequential scan: O(1)
 */
RB3_API
struct rb3_head *rb3_get_prev(struct rb3_head *head);

/**
 * Get next in-order node (minimal node in the tree that sorts after the given
 * element) or NULL if no such element is in the tree.
 *
 * Time complexity: O(log n), amortized over sequential scan: O(1)
 */
RB3_API
struct rb3_head *rb3_get_next(struct rb3_head *head);

/**
 * Insert `head` into `tree` using `cmp` to direct the search. At each visited
 * node in the tree `cmp` is called with that node and `head` as arguments (in
 * that order). If a node that compares equal is found, it is returned.
 * Otherwise, `head` is inserted into the tree and NULL is returned.
 *
 * Time complexity: O(log n)
 */
RB3_API
struct rb3_head *rb3_insert(struct rb3_head *head, struct rb3_tree *tree, rb3_cmp cmp);

/**
 * Find `head` in `tree` using `cmp` to direct the search. At each visited
 * node in the tree `cmp` is called with that node and `head` as arguments (in
 * that order). If a node that compares equal is found, it is returned.
 * Otherwise, NULL is returned.
 *
 * Time complexity: O(log n)
 */
RB3_API
struct rb3_head *rb3_find(struct rb3_tree *tree, rb3_cmp cmp, struct rb3_head *head);

/**
 * Delete `head` from `tree` using `cmp` to direct the search. At each visited
 * node in the tree `cmp` is called with that node and `head` as arguments (in
 * that order). If a node that compares equal is found, it is unlinked from
 * the tree and returned. Otherwise, NULL is returned.
 *
 * Time complexity: O(log n)
 */
RB3_API
struct rb3_head *rb3_delete(struct rb3_tree *tree, rb3_cmp cmp, struct rb3_head *head);

/**
 * Like rb3_insert() but use a rb3_datacmp comparison function and `data` to
 * direct the search. Note in particular that `head` is not used during the
 * search phase.
 */
RB3_API
struct rb3_head *rb3_insert_datacmp(struct rb3_head *head, struct rb3_tree *tree, rb3_datacmp datacmp, void *data);

/**
 * Like rb3_find() but use a rb3_datacmp comparison function and `data` to
 * direct the search.
 */
RB3_API
struct rb3_head *rb3_find_datacmp(struct rb3_tree *tree, rb3_datacmp cmp, void *data);

/**
 * Like rb3_delete() but use a rb3_datacmp comparison function and `data` to
 * direct the search.
 */
RB3_API
struct rb3_head *rb3_delete_datacmp(struct rb3_tree *tree, rb3_datacmp cmp, void *data);

/*
 * Inline implementations
 */

RB3_API_STATIC_INLINE
int rb3_isempty(struct rb3_tree *tree)
{
        return !rb3_has_child(rb3_get_base(tree), RB3_LEFT);
}
