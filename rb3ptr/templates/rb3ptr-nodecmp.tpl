RB3_API_STATIC_INLINE
int nodecmp(struct rb3_head *a, struct rb3_head *b)
{
        return COMPARE_NODE(GET_NODE(a), GET_NODE(b));
}

RB3_API RB3_NEVERINLINE
NODE_TYPE *find_in_subtree(NODE_TYPE *subtree, NODE_TYPE *node)
{
	if (!subtree)
		return (NODE_TYPE *) 0;
        return GET_NODE(rb3_INLINE_find_in_subtree(GET_HEAD(subtree), (rb3_datacmp) nodecmp, GET_HEAD(node)));
}

RB3_API RB3_NEVERINLINE
NODE_TYPE *find_parent_in_subtree(NODE_TYPE *parent, int dir, NODE_TYPE *node, NODE_TYPE **parent_out, int *dir_out)
{
        struct rb3_head *found;
        struct rb3_head *pnt;

        found = rb3_INLINE_find_parent_in_subtree(GET_HEAD(parent), dir, (rb3_datacmp) nodecmp, GET_HEAD(node), &pnt, &dir);
        if (found)
                return GET_NODE(found);
        *parent_out = GET_NODE(pnt);
        *dir_out = dir;
        return (NODE_TYPE *) 0;
}

RB3_API RB3_NEVERINLINE
NODE_TYPE *delete_in_subtree(NODE_TYPE *node, NODE_TYPE *subtree)
{
        NODE_TYPE *found;

        found = find_in_subtree(subtree, node);
        if (found)
                rb3_delete_head(GET_HEAD(found));
	return found;
}

RB3_API RB3_NEVERINLINE
NODE_TYPE *insert_in_subtree(NODE_TYPE *node, NODE_TYPE *parent, int dir)
{
        NODE_TYPE *found;

        found = find_parent_in_subtree(parent, dir, node, &parent, &dir);
        if (!found)
		rb3_insert_below(GET_HEAD(node), GET_HEAD(parent), dir);
        return found;
}

RB3_API_INLINE
NODE_TYPE *find(OUTER_TREE_TYPE *tree, NODE_TYPE *node)
{
	return find_in_subtree(get_root(tree), node);
}

RB3_API_INLINE
NODE_TYPE *delete(NODE_TYPE *node, OUTER_TREE_TYPE *tree)
{
	return delete_in_subtree(node, get_root(tree));
}

RB3_API_INLINE
NODE_TYPE *insert(NODE_TYPE *node, OUTER_TREE_TYPE *tree)
{
        return insert_in_subtree(node, get_base(tree), RB3_LEFT);
}
