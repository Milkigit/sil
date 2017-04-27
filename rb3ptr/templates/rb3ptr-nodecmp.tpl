RB3_API_STATIC_INLINE
int headcmp(struct rb3_head *head, struct rb3_head *data)
{
        return COMPARE_HEAD(OUTER_HEAD(head), OUTER_HEAD(data));
}

RB3_API RB3_NEVERINLINE
OUTER_HEAD_TYPE *find_in_subtree(OUTER_HEAD_TYPE *subtree, OUTER_HEAD_TYPE *head)
{
        return OUTER_HEAD(rb3_INLINE_find_in_subtree(INNER_HEAD(subtree), (rb3_datacmp) headcmp, INNER_HEAD(head)));
}

RB3_API RB3_NEVERINLINE
OUTER_HEAD_TYPE *find_parent_in_subtree(OUTER_HEAD_TYPE *parent, int dir, OUTER_HEAD_TYPE *head, OUTER_HEAD_TYPE **parent_out, int *dir_out)
{
        struct rb3_head *found;
        struct rb3_head *pnt;

        found = rb3_INLINE_find_parent_in_subtree(INNER_HEAD(parent), dir, (rb3_datacmp) headcmp, INNER_HEAD(head), &pnt, &dir);
        if (found)
                return OUTER_HEAD(found);
        *parent_out = OUTER_HEAD(pnt);
        *dir_out = dir;
        return (OUTER_HEAD_TYPE *) 0;
}

RB3_API RB3_NEVERINLINE
OUTER_HEAD_TYPE *delete_in_subtree(OUTER_HEAD_TYPE *head, OUTER_HEAD_TYPE *subtree)
{
        OUTER_HEAD_TYPE *found;

        found = find_in_subtree(subtree, head);
        if (found) {
                rb3_delete_head(INNER_HEAD(found));
                return found;
        }
        return (OUTER_HEAD_TYPE *) 0;
}

RB3_API RB3_NEVERINLINE
OUTER_HEAD_TYPE *insert_in_subtree(OUTER_HEAD_TYPE *head, OUTER_HEAD_TYPE *parent, int dir)
{
        OUTER_HEAD_TYPE *found;

        found = find_parent_in_subtree(parent, dir, head, &parent, &dir);
        if (found)
                return found;
        rb3_insert_below(INNER_HEAD(head), INNER_HEAD(parent), dir);
        return (OUTER_HEAD_TYPE *) 0;
}

RB3_API_INLINE
NODE_TYPE *find(OUTER_TREE_TYPE *tree, NODE_TYPE *node)
{
        OUTER_HEAD_TYPE *found;

	found = find_in_subtree(get_root(tree), GET_HEAD(node));
	if (found)
		return GET_NODE(found);
	return (NODE_TYPE *) 0;
}

RB3_API_INLINE
NODE_TYPE *delete(NODE_TYPE *node, OUTER_TREE_TYPE *tree)
{
        OUTER_HEAD_TYPE *found;

	found = delete_in_subtree(GET_HEAD(node), get_root(tree));
	if (found)
		return GET_NODE(found);
	return (NODE_TYPE *) 0;
}

RB3_API_INLINE
NODE_TYPE *insert(NODE_TYPE *node, OUTER_TREE_TYPE *tree)
{
	OUTER_HEAD_TYPE *found;

        found = insert_in_subtree(GET_HEAD(node), get_base(tree), RB3_LEFT);
	if (found)
		return GET_NODE(found);
	return (NODE_TYPE *) 0;
}
