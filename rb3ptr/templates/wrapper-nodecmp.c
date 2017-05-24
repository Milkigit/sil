/*
 * ---------------------------------------------------------------------------
 * Nodecmp
 *
 * This file implements the typesafe wrapper for comparison related functions
 * (find, insert, delete).
 * ---------------------------------------------------------------------------
 */

_RB3_API_STATIC_INLINE
int nodecmp(struct rb3_head *a, NODE_TYPE *b)
{
        return COMPARE_NODE(GET_NODE(a), b);
}

_RB3_API _RB3_NEVERINLINE
NODE_TYPE *find_in_subtree(NODE_TYPE *subtree, NODE_TYPE *node)
{
        return GET_NODE(rb3_INLINE_find_in_subtree(GET_HEAD(subtree), (rb3_cmp) nodecmp, GET_HEAD(node)));
}

_RB3_API _RB3_NEVERINLINE
NODE_TYPE *find_parent_in_subtree(struct rb3_head *parent, int dir, NODE_TYPE *node, struct rb3_head **parent_out, int *dir_out)
{
        return GET_NODE(rb3_INLINE_find_parent_in_subtree(parent, dir, (rb3_cmp) nodecmp, node, parent_out, dir_out));
}

_RB3_API
NODE_TYPE *find(OUTER_TREE_TYPE *tree, NODE_TYPE *node)
{
        return find_in_subtree(get_root(tree), node);
}

_RB3_API
NODE_TYPE *delete(OUTER_TREE_TYPE *tree, NODE_TYPE *node)
{
        NODE_TYPE *found;

        found = find_in_subtree(get_root(tree), node);
        if (found)
                rb3_unlink_node(GET_HEAD(found));
        return found;
}

_RB3_API
NODE_TYPE *insert(OUTER_TREE_TYPE *tree, NODE_TYPE *node)
{
        NODE_TYPE *found;
        struct rb3_head *parent;
        int dir;

        found = find_parent_in_subtree(&INNER_TREE(tree)->base, RB3_LEFT, node, &parent, &dir);
        if (!found)
                rb3_link_node(GET_HEAD(node), parent, dir);
        return found;
}
