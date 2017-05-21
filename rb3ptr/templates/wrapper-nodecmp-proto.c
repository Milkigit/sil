/**
 * Find `node` in `tree` using the comparison function handed to the macro
 * that generates this function.
 */
_RB3_API
NODE_TYPE *find(OUTER_TREE_TYPE *tree, NODE_TYPE *node);

/**
 * Delete `node` from `tree` using the comparison function handed to the macro
 * that generates this function.
 */
_RB3_API
NODE_TYPE *delete(OUTER_TREE_TYPE *tree, NODE_TYPE *node);

/**
 * Insert `node` into `tree` using the comparison function handed to the macro
 * that generates this function.
 */
_RB3_API
NODE_TYPE *insert(OUTER_TREE_TYPE *tree, NODE_TYPE *node);


/*
 * ---------------------------------------------------------------------------
 * Internal stuff (probably not useful)
 * ---------------------------------------------------------------------------
 */

/**
 * Find a node in the subtree indicated by `subtree` (inclusively).
 */
_RB3_API
NODE_TYPE *find_in_subtree(NODE_TYPE *subtree, NODE_TYPE *node);

/**
 * Find insertion point in subtree. The subtree is indicated by its parent and
 * the child direction in which the parent links the subtree.
 *
 * Parent must be non-NULL, but it could be the base (and the latter is the
 * reason why it can't be a NODE_TYPE but must be a struct rb3_head).
 */
_RB3_API
NODE_TYPE *find_parent_in_subtree(struct rb3_head *parent, int dir, NODE_TYPE *node, struct rb3_head **parent_out, int *dir_out);
