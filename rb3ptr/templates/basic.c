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
_RB3_API
void rb3_reset_tree(struct rb3_tree *tree);

/**
 * Check if tree is empty.
 *
 * Time complexity: O(1)
 */
_RB3_API_STATIC_INLINE
int rb3_isempty(struct rb3_tree *tree);

/**
 * Get minimum (leftmost) element, or NULL if tree is empty.
 *
 * Time complexity: O(log n)
 */
_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_min(struct rb3_tree *tree);

/**
 * Get maximum (rightmost) element, or NULL if tree is empty
 *
 * Time complexity: O(log n)
 */
_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_max(struct rb3_tree *tree);

/**
 * Get previous in-order node (maximal node in the tree that sorts before the
 * given element) or NULL if no such element is in the tree.
 *
 * Time complexity: O(log n), amortized over sequential scan: O(1)
 */
_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_prev(struct rb3_head *head);

/**
 * Get next in-order node (minimal node in the tree that sorts after the given
 * element) or NULL if no such element is in the tree.
 *
 * Time complexity: O(log n), amortized over sequential scan: O(1)
 */
_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_next(struct rb3_head *head);

/**
 * Get minimum or maximum, depending on the value of `dir` (RB3_LEFT or
 * RB3_RIGHT)
 *
 * Time complexity: O(log n)
 */
_RB3_API
struct rb3_head *rb3_get_minmax(struct rb3_tree *tree, int dir);

/**
 * Get previous or next in-order node, depending on the value of `dir`.
 *
 * Time complexity: O(log n), amortized over sequential scan: O(1)
 */
_RB3_API
struct rb3_head *rb3_get_prevnext(struct rb3_head *head, int dir);

/**
 * Find a node in `tree` using `cmp` to direct the search. At each visited
 * node in the tree `cmp` is called with that node and `data` as arguments.
 * If a node that compares equal is found, it is returned. Otherwise, NULL is
 * returned.
 *
 * Time complexity: O(log n)
 */
_RB3_API
struct rb3_head *rb3_find(struct rb3_tree *tree, rb3_cmp cmp, void *data);

/**
 * Find a suitable insertion point for a new node in `tree` using `cmp` and
 * `data` to direct the search. At each visited node in the tree `cmp` is
 * called with that node and `data` as arguments. If a node that compares
 * equal is found, it is returned. Otherwise, NULL is returned and the
 * insertion point is returned as parent node and child direction in
 * `parent_out` and `dir_out`.
 *
 * Time complexity: O(log n)
 */
_RB3_API
struct rb3_head *rb3_find_parent(struct rb3_tree *tree, rb3_cmp cmp, void *data, struct rb3_head **parent_out, int *dir_out);

/**
 * Link `head` into `tree` below another node in the given direction (RB3_LEFT
 * or RB3_RIGHT). The new node must replace a leaf. You can use
 * rb3_find_parent() to find the insertion point.
 *
 * `head` must not be linked into another tree when this function is called.
 *
 * Time complexity: O(log n)
 */
_RB3_API
void rb3_link_and_rebalance(struct rb3_head *head, struct rb3_head *parent, int dir);

/**
 * Unlink `head` from its current tree.
 *
 * Time complexity: O(log n)
 */
_RB3_API
void rb3_unlink_and_rebalance(struct rb3_head *head);

/**
 * Replace `head` with `newhead`. `head` must be linked in a tree and
 * `newhead` must not be linked in a tree.
 */
_RB3_API
void rb3_replace(struct rb3_head *head, struct rb3_head *newhead);

/**
 * Insert `head` into `tree` using `cmp` and `data` to direct the search. At
 * each visited node in the tree `cmp` is called with that node and `data` as
 * arguments (in that order). If a node that compares equal is found, it is
 * returned. Otherwise, `head` is inserted into the tree and NULL is
 * returned.
 *
 * Time complexity: O(log n)
 */
_RB3_API
struct rb3_head *rb3_insert(struct rb3_tree *tree, struct rb3_head *head, rb3_cmp cmp, void *data);

/**
 * Find and delete a node from `tree` using `cmp` to direct the search. At
 * each visited node in the tree `cmp` is called with that node and `head` as
 * arguments (in that order). If a node that compares equal is found, it is
 * unlinked from the tree and returned. Otherwise, NULL is returned.
 *
 * Time complexity: O(log n)
 */
_RB3_API
struct rb3_head *rb3_delete(struct rb3_tree *tree, rb3_cmp cmp, void *data);

/**
 * Given a node that is known to be linked in _some_ tree, find that tree.
 *
 * This involves a little hackery with offsetof(3)
 */
_RB3_API
struct rb3_tree *rb3_get_containing_tree(struct rb3_head *head);


/*
 * Inline implementations
 */

_RB3_API_STATIC_INLINE
int rb3_isempty(struct rb3_tree *tree)
{
        return !rb3_has_child(rb3_get_base(tree), RB3_LEFT);
}

_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_min(struct rb3_tree *tree)
{
        return rb3_get_minmax(tree, RB3_LEFT);
}

_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_max(struct rb3_tree *tree)
{
        return rb3_get_minmax(tree, RB3_RIGHT);
}

_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_prev(struct rb3_head *head)
{
        return rb3_get_prevnext(head, RB3_LEFT);
}

_RB3_API_STATIC_INLINE
struct rb3_head *rb3_get_next(struct rb3_head *head)
{
        return rb3_get_prevnext(head, RB3_RIGHT);
}
