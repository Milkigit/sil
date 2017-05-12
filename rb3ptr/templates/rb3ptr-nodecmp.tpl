RB3_API_STATIC_INLINE
int nodecmp(struct rb3_head *a, NODE_TYPE *b)
{
        return COMPARE_NODE(GET_NODE(a), b);
}

RB3_API RB3_NEVERINLINE
NODE_TYPE *find_in_subtree(NODE_TYPE *subtree, NODE_TYPE *node)
{
        return GET_NODE(rb3_INLINE_find_in_subtree(GET_HEAD(subtree), (rb3_datacmp) nodecmp, GET_HEAD(node)));
}

RB3_API RB3_NEVERINLINE
NODE_TYPE *find_parent_in_subtree(struct rb3_head *parent, int dir, NODE_TYPE *node, struct rb3_head **parent_out, int *dir_out)
{
        return GET_NODE(rb3_INLINE_find_parent_in_subtree(parent, dir, (rb3_datacmp) nodecmp, node, parent_out, dir_out));
}

RB3_API
NODE_TYPE *find(OUTER_TREE_TYPE *tree, NODE_TYPE *node)
{
        return find_in_subtree(get_root(tree), node);
}

RB3_API
NODE_TYPE *delete(OUTER_TREE_TYPE *tree, NODE_TYPE *node)
{
        NODE_TYPE *found;

        found = find_in_subtree(get_root(tree), node);
        if (found)
                rb3_delete_head(GET_HEAD(found));
        return found;
}

RB3_API
NODE_TYPE *insert(OUTER_TREE_TYPE *tree, NODE_TYPE *node)
{
        NODE_TYPE *found;
        struct rb3_head *parent;
        int dir;

        parent = &INNER_TREE(tree)->base;
        dir = RB3_LEFT;
        found = find_parent_in_subtree(parent, dir, node, &parent, &dir);
        if (!found)
                rb3_insert_below(GET_HEAD(node), parent, dir);
        return found;
}
