RB3_API_STATIC_INLINE
struct rb3_head *GET_HEAD(NODE_TYPE *node)
{
	return &HEAD_FROM_NODE(node)->head;
}

RB3_API_STATIC_INLINE
NODE_TYPE *GET_NODE(struct rb3_head *head)
{
	if (head)
	        return NODE_FROM_HEAD((OUTER_HEAD_TYPE *) head);
	return (NODE_TYPE *) 0;
}

RB3_API_STATIC_INLINE
struct rb3_tree *INNER_TREE(OUTER_TREE_TYPE *tree)
{
        return (struct rb3_tree *) tree;
}

RB3_API_STATIC_INLINE
OUTER_TREE_TYPE *OUTER_TREE(struct rb3_tree *tree)
{
        return (OUTER_TREE_TYPE *) tree;
}

RB3_API_STATIC_INLINE
void init(OUTER_TREE_TYPE *tree)
{
        rb3_init(INNER_TREE(tree));
}

RB3_API_STATIC_INLINE
void exit(OUTER_TREE_TYPE *tree)
{
        rb3_exit(INNER_TREE(tree));
}

RB3_API_STATIC_INLINE
int isempty(OUTER_TREE_TYPE *tree)
{
        return rb3_isempty(INNER_TREE(tree));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_min(OUTER_TREE_TYPE *tree)
{
        return GET_NODE(rb3_get_min(INNER_TREE(tree)));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_max(OUTER_TREE_TYPE *tree)
{
        return GET_NODE(rb3_get_max(INNER_TREE(tree)));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_prev(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_prev(GET_HEAD(node)));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_next(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_next(GET_HEAD(node)));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_root(OUTER_TREE_TYPE *tree)
{
        return GET_NODE(rb3_get_root(INNER_TREE(tree)));
}

RB3_API_STATIC_INLINE
int has_child(NODE_TYPE *node, int dir)
{
        return rb3_has_child(GET_HEAD(node), dir);
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_child(NODE_TYPE *node, int dir)
{
        return GET_NODE(rb3_get_child(GET_HEAD(node), dir));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_parent(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_parent(GET_HEAD(node)));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_prev_ancestor(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_prev_ancestor(GET_HEAD(node)));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_next_ancestor(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_next_ancestor(GET_HEAD(node)));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_prev_descendant(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_prev_descendant(GET_HEAD(node)));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_next_descendant(NODE_TYPE *node)
{
        return GET_NODE(rb3_get_next_descendant(GET_HEAD(node)));
}

RB3_API_STATIC_INLINE
void insert_below(NODE_TYPE *node, NODE_TYPE *parent, int dir)
{
        rb3_insert_below(GET_HEAD(node), GET_HEAD(parent), dir);
}

RB3_API_STATIC_INLINE
void delete_head(NODE_TYPE *node)
{
        rb3_delete_head(GET_HEAD(node));
}

RB3_API_STATIC_INLINE
int get_parent_dir(NODE_TYPE *node)
{
        return rb3_get_parent_dir(GET_HEAD(node));
}

RB3_API_STATIC_INLINE
NODE_TYPE *get_base(OUTER_TREE_TYPE *tree)
{
        return GET_NODE(rb3_get_base(INNER_TREE(tree)));
}

RB3_API_STATIC_INLINE
OUTER_TREE_TYPE *get_containing_tree(NODE_TYPE *node)
{
        return OUTER_TREE(rb3_get_containing_tree(GET_HEAD(node)));
}
