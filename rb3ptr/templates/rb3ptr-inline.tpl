RB3_API_INLINE
OUTER_HEAD_TYPE *GET_HEAD(NODE_TYPE *node)
{
	return HEAD_FROM_NODE(node);
}

RB3_API_INLINE
NODE_TYPE *GET_NODE(OUTER_HEAD_TYPE *head)
{
	return NODE_FROM_HEAD(head);
}

RB3_API_INLINE
struct rb3_head *INNER_HEAD(OUTER_HEAD_TYPE *head)
{
        return (struct rb3_head *) head;
}

RB3_API_INLINE
OUTER_HEAD_TYPE *OUTER_HEAD(struct rb3_head *head)
{
        return (OUTER_HEAD_TYPE *) head;
}

RB3_API_INLINE
struct rb3_tree *INNER_TREE(OUTER_TREE_TYPE *tree)
{
        return (struct rb3_tree *) tree;
}

RB3_API_INLINE
OUTER_TREE_TYPE *OUTER_TREE(struct rb3_tree *tree)
{
        return (OUTER_TREE_TYPE *) tree;
}

RB3_API_INLINE
void init(OUTER_TREE_TYPE *tree)
{
        rb3_init(INNER_TREE(tree));
}

RB3_API_INLINE
void exit(OUTER_TREE_TYPE *tree)
{
        rb3_exit(INNER_TREE(tree));
}

RB3_API_INLINE
void isempty(OUTER_TREE_TYPE *tree)
{
        rb3_isempty(INNER_TREE(tree));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_min(OUTER_TREE_TYPE *tree)
{
        return OUTER_HEAD(rb3_get_min(INNER_TREE(tree)));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_max(OUTER_TREE_TYPE *tree)
{
        return OUTER_HEAD(rb3_get_max(INNER_TREE(tree)));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_prev(OUTER_HEAD_TYPE *node)
{
        return OUTER_HEAD(rb3_get_prev(INNER_HEAD(node)));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_next(OUTER_HEAD_TYPE *node)
{
        return OUTER_HEAD(rb3_get_next(INNER_HEAD(node)));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_root(OUTER_TREE_TYPE *tree)
{
        return OUTER_HEAD(rb3_get_root(INNER_TREE(tree)));
}

RB3_API_INLINE
int has_child(OUTER_HEAD_TYPE *node, int dir)
{
        return rb3_has_child(INNER_HEAD(node), dir);
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_child(OUTER_HEAD_TYPE *node, int dir)
{
        return OUTER_HEAD(rb3_get_child(INNER_HEAD(node), dir));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_parent(OUTER_HEAD_TYPE *node)
{
        return OUTER_HEAD(rb3_get_parent(INNER_HEAD(node)));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_prev_ancestor(OUTER_HEAD_TYPE *node)
{
        return OUTER_HEAD(rb3_get_prev_ancestor(INNER_HEAD(node)));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_next_ancestor(OUTER_HEAD_TYPE *node)
{
        return OUTER_HEAD(rb3_get_next_ancestor(INNER_HEAD(node)));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_prev_descendant(OUTER_HEAD_TYPE *node)
{
        return OUTER_HEAD(rb3_get_prev_descendant(INNER_HEAD(node)));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_next_descendant(OUTER_HEAD_TYPE *node)
{
        return OUTER_HEAD(rb3_get_next_descendant(INNER_HEAD(node)));
}

RB3_API_INLINE
void insert_below(OUTER_HEAD_TYPE *node, OUTER_HEAD_TYPE *parent, int dir)
{
        rb3_insert_below(INNER_HEAD(node), INNER_HEAD(parent), dir);
}

RB3_API_INLINE
void delete_head(OUTER_HEAD_TYPE *node)
{
        rb3_delete_head(INNER_HEAD(node));
}

RB3_API_INLINE
int get_parent_dir(OUTER_HEAD_TYPE *node)
{
        return rb3_get_parent_dir(INNER_HEAD(node));
}

RB3_API_INLINE
OUTER_HEAD_TYPE *get_base(OUTER_TREE_TYPE *tree)
{
        return OUTER_HEAD(rb3_get_base(INNER_TREE(tree)));
}

RB3_API_INLINE
OUTER_TREE_TYPE *get_containing_tree(OUTER_HEAD_TYPE *node)
{
        return OUTER_TREE(rb3_get_containing_tree(INNER_HEAD(node)));
}
