_RB3_API_STATIC_INLINE
struct rb3_tree *INNER_TREE(OUTER_TREE_TYPE *tree)
{
        return (struct rb3_tree *) tree;
}

_RB3_API_STATIC_INLINE
OUTER_TREE_TYPE *OUTER_TREE(struct rb3_tree *tree)
{
        return (OUTER_TREE_TYPE *) tree;
}

_RB3_API_STATIC_INLINE
void init(OUTER_TREE_TYPE *tree)
{
        rb3_init(INNER_TREE(tree));
}

_RB3_API_STATIC_INLINE
void exit(OUTER_TREE_TYPE *tree)
{
        rb3_exit(INNER_TREE(tree));
}

_RB3_API_STATIC_INLINE
int isempty(OUTER_TREE_TYPE *tree)
{
        return rb3_isempty(INNER_TREE(tree));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_min(OUTER_TREE_TYPE *tree)
{
        return GET_NODE(rb3_get_min(INNER_TREE(tree)));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_max(OUTER_TREE_TYPE *tree)
{
        return GET_NODE(rb3_get_max(INNER_TREE(tree)));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_prev(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_prev(GET_HEAD(node)));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_next(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_next(GET_HEAD(node)));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_minmax(OUTER_TREE_TYPE *tree, int dir)
{
        return GET_NODE(rb3_get_minmax(INNER_TREE(tree), dir));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_prevnext(NODE_TYPE *node, int dir)
{
        return GET_NODE(rb3_get_prevnext(GET_HEAD(node), dir));
}

/*
 * Delete a node that is known to be linked in a tree.
 *
 * This is deletion with a refined equivalence relation where nodes are only
 * equal iff their memory addresses are identical.
 */
_RB3_API_STATIC_INLINE
void unlink_and_rebalance(NODE_TYPE *node)
{
        rb3_unlink_and_rebalance(GET_HEAD(node));
}

/*
 * --------------------------
 * Binary Search Tree stuff
 * --------------------------
 */

_RB3_API_STATIC_INLINE
NODE_TYPE *get_root(OUTER_TREE_TYPE *tree)
{
        return GET_NODE(rb3_get_root(INNER_TREE(tree)));
}

_RB3_API_STATIC_INLINE
int has_child(NODE_TYPE *node, int dir)
{
        return rb3_has_child(GET_HEAD(node), dir);
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_child(NODE_TYPE *node, int dir)
{
        return GET_NODE(rb3_get_child(GET_HEAD(node), dir));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_parent(NODE_TYPE *node)
{
        struct rb3_head *parent;

        parent = rb3_get_parent(GET_HEAD(node));
        if (rb3_is_base(parent))
                return (NODE_TYPE *) 0;
        return GET_NODE(parent);
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_prev_ancestor(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_prev_ancestor(GET_HEAD(node)));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_next_ancestor(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_next_ancestor(GET_HEAD(node)));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_prev_descendant(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_prev_descendant(GET_HEAD(node)));
}

_RB3_API_STATIC_INLINE
NODE_TYPE *get_next_descendant(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_next_descendant(GET_HEAD(node)));
}

_RB3_API_STATIC_INLINE
int get_parent_dir(NODE_TYPE *node)
{
        return rb3_get_parent_dir(GET_HEAD(node));
}

_RB3_API_STATIC_INLINE
OUTER_TREE_TYPE *get_containing_tree(NODE_TYPE *node)
{
        return OUTER_TREE(rb3_get_containing_tree(GET_HEAD(node)));
}
