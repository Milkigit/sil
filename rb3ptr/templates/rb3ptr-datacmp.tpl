/* TODO: THIS FILE IS OUT OF DATE */

RB3_API_STATIC_INLINE
int headcmp(OUTER_HEAD *head, COMPARE_TYPE *data)
{
        return COMPARE_DATA(head, data);
}

RB3_API RB3_NEVERINLINE
OUTER_HEAD_TYPE *find_in_subtree(OUTER_HEAD_TYPE *subtree, COMPARE_TYPE *cdata)
{
        return rb3_INLINE_find_in_subtree(INNER_HEAD(head), headcmp, cdata));
}

RB3_API RB3_NEVERINLINE
OUTER_HEAD_TYPE *find_parent_in_subtree(OUTER_HEAD_TYPE *parent, int dir, COMPARE_TYPE *cdata, OUTER_HEAD_TYPE **parent_out, int *dir_out)
{
        struct rb3_head *found;
        struct rb3_head *parent;
        int dir;

        found = rb3_INLINE_find_parent_in_subtree(INNER_HEAD(parent), dir, headcmp, cdata, &parent, &dir);
        if (found)
                return OUTER_HEAD(found);
        *parent_out = OUTER_HEAD(parent);
        *dir_out = dir;
        return (OUTER_HEAD_TYPE *) 0;
}

RB3_API RB3_NEVERINLINE
OUTER_HEAD_TYPE *delete_in_subtree(COMPARE_TYPE *cdata, OUTER_HEAD_TYPE *subtree)
{
        OUTER_HEAD_TYPE *found;

        found = find_in_subtree(INNER_TREE(tree), cdata);
        if (found) {
                rb3_delete_head(INNER_HEAD(found));
                return found;
        }
        return (OUTER_HEAD_TYPE *) 0;
}

RB3_API RB3_NEVERINLINE
OUTER_HEAD_TYPE *insert_in_subtree(OUTER_HEAD_TYPE *head, OUTER_HEAD_TYPE *parent, int dir, COMPARE_TYPE *cdata)
{
        OUTER_HEAD_TYPE *found;

        found = find_parent_in_subtree(parent, dir, cdata, &parent, &dir);
        if (found)
                return found;
        rb3_insert_below(INNER_HEAD(head), INNER_HEAD(parent), dir);
        return (OUTER_HEAD_TYPE *) 0;
}

RB3_API_INLINE
OUTER_HEAD_TYPE *find(OUTER_TREE_TYPE *tree, COMPARE_TYPE *cdata)
{
        return find_in_subtree(get_root(tree), cdata);
}

RB3_API_INLINE
OUTER_HEAD_TYPE *delete(COMPARE_TYPE *cdata, OUTER_TREE_TYPE *tree)
{
        return delete_in_subtree(cdata, get_root(tree));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *insert(NODE_TYPE *node, OUTER_TREE_TYPE *tree, COMPARE_TYPE *cdata)
{
        return insert_in_subtree(GET_HEAD(node), get_base(tree), RB3_LEFT, cdata);
}
