#define RB3_GEN_TREE_DEFINITION(BASENAME)  \
struct BASENAME {  \
        struct rb3_tree tree;  \
}

/*
 * Access tree through macros, similar to BSD API.
 *
 * Once a wrapped tree is created, say "footree", instead of
 *
 *   footree_insert(the_tree, the_node);
 *
 * we say
 *
 *   RB3_INSERT(footree, the_tree, the_node);
 *
 * and so on.
 */

#define RB3_INIT(BASENAME, tree) \
        BASENAME##_init(tree)

#define RB3_EXIT(BASENAME, tree) \
        BASENAME##_exit(tree)

#define RB3_DELETE_NODE(BASENAME, tree, elm) \
        BASENAME##_unlink_node(elm)

#define RB3_MIN(BASENAME, tree) \
        BASENAME##_get_min(tree)

#define RB3_MAX(BASENAME, tree) \
        BASENAME##_get_max(tree)

#define RB3_PREV(BASENAME, elm) \
        BASENAME##_get_prev(elm)

#define RB3_NEXT(BASENAME, elm) \
        BASENAME##_get_next(elm)

#define RB3_INSERT(BASENAME, tree, elm) \
        BASENAME##_insert(tree, elm)

#define RB3_FIND(BASENAME, tree, elm) \
        BASENAME##_find(tree, elm)

#define RB3_DELETE(BASENAME, tree, elm) \
        BASENAME##_delete(tree, elm)


/* iteration macros */

#define RB3_FOREACH_DIR(BASENAME, TREE, NODE, DIR) \
    for (NODE = BASENAME##_get_minmax(TREE, !(DIR)); \
        !!NODE; NODE = BASENAME##_get_prevnext(NODE, (DIR)))

#define RB3_FOREACH_SAFE_DIR(BASENAME, TREE, NODE, NODE1) \
    for (NODE = BASENAME##_get_minmax(TREE, !(DIR)); \
        (!!NODE ? (NODE1 = BASENAME##_get_prevnext(NODE, (DIR)), !!NODE) : !!NODE); \
        NODE = NODE1)

#define RB3_FOREACH(BASENAME, TREE, NODE) \
        RB3_FOREACH_DIR(BASENAME, (TREE), (NODE), RB3_RIGHT)

#define RB3_FOREACH_SAFE(BASENAME, TREE, NODE, NODE1) \
        RB3_FOREACH_SAFE_DIR(BASENAME, (TREE), (NODE), (NODE1), RB3_RIGHT)

#define RB3_FOREACH_REVERSE(BASENAME, TREE, NODE) \
        RB3_FOREACH_DIR(BASENAME, (TREE), (NODE), RB3_LEFT)

#define RB3_FOREACH_REVERSE_SAFE(BASENAME, TREE, NODE, NODE1) \
        RB3_FOREACH_SAFE_DIR(BASENAME, (TREE), (NODE), (NODE1), RB3_LEFT)
