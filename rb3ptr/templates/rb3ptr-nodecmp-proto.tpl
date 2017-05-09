RB3_API
NODE_TYPE *find_in_subtree(NODE_TYPE *subtree, NODE_TYPE *node);

RB3_API
NODE_TYPE *find_parent_in_subtree(NODE_TYPE *parent, int dir, NODE_TYPE *node, NODE_TYPE **parent_out, int *dir_out);

RB3_API
NODE_TYPE *delete_in_subtree(NODE_TYPE *node, NODE_TYPE *subtree);

RB3_API
NODE_TYPE *insert_in_subtree(NODE_TYPE *node, NODE_TYPE *parent, int dir);

RB3_API
NODE_TYPE *find(OUTER_TREE_TYPE *tree, NODE_TYPE *node);

RB3_API
NODE_TYPE *delete(NODE_TYPE *node, OUTER_TREE_TYPE *tree);

RB3_API
NODE_TYPE *insert(NODE_TYPE *node, OUTER_TREE_TYPE *tree);
