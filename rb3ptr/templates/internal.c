/*
 * ---------------------------------------------------------------------------
 * Internal API
 *
 * These functions expose some of the more stable implementation details that
 * might be useful in other places. They are generally unsafe to use. Make
 * sure to read the assumptions that must hold before calling them.
 * ---------------------------------------------------------------------------
 */

/**
 * Like rb3_find_datacmp() but starts at the given subtree represented by a
 * link head (which may be NULL) instead of at the root of the tree.
 */
RB3_API
struct rb3_head *rb3_find_in_subtree_datacmp(struct rb3_head *subtree, rb3_datacmp cmp, void *data);

/**
 * Find suitable insertion point for a new node in a subtree, directed by the
 * rb3_datacmp search. The subtree is given by its parent node `parent` and
 * child direction `dir`. The insertion point and its child direction are
 * returned in `parent_out` and `dir_out`.
 *
 * If the searched node is already in the tree (the compare function returns
 * 0), it is returned. In this case `parent_out` and `dir_out` are left
 * untouched.
 */
RB3_API
struct rb3_head *rb3_find_parent_in_subtree_datacmp(struct rb3_head *parent, int dir, rb3_datacmp cmp, void *data, struct rb3_head **parent_out, int *dir_out);

/**
 * Insert a node below another node in the given direction (RB3_LEFT or
 * RB3_RIGHT). The new node must replace a leaf. You can use rb3_find_parent()
 * to find the insertion point.
 *
 * Time complexity: O(log n)
 */
RB3_API
void rb3_insert_below(struct rb3_head *head, struct rb3_head *parent, int dir);

/**
 * Delete a node that is known to be linked in a tree.
 *
 * This is more efficient than using rb3_find() because no search is needed.
 * On the downside, the user must guarantee that the node really is linked in
 * some tree.
 *
 * Time complexity: O(log n)
 */
RB3_API
void rb3_delete_head(struct rb3_head *head);

/**
 * Given a node that is known to be linked in _some_ tree, find that tree.
 */
RB3_API
struct rb3_tree *rb3_get_containing_tree(struct rb3_head *head);

/*
 * Inline implementations
 */


/* this interface is meant for code generation, not for casual consumption */
RB3_API_STATIC_INLINE
struct rb3_head *rb3_INLINE_find_in_subtree(struct rb3_head *subtree, rb3_datacmp cmp, void *data)
{
        struct rb3_head *head;
        int dir;
        int r;

        head = subtree;
        while (head) {
                r = cmp(head, data);
                if (r < 0)
                        dir = RB3_RIGHT;
                else if (r > 0)
                        dir = RB3_LEFT;
                else
                        return head;
                head = rb3_get_child(head, dir);
        }
        return RB3_NULL;
}

/* this interface is meant for code generation, not for casual consumption */
RB3_API_STATIC_INLINE
struct rb3_head *rb3_INLINE_find_parent_in_subtree(struct rb3_head *parent, int dir, rb3_datacmp cmp, void *data, struct rb3_head **parent_out, int *dir_out)
{
        struct rb3_head *head;
        int r;

        RB3_ASSERT(parent != RB3_NULL);
        do {
                head = rb3_get_child(parent, dir);
                if (!head)
                        break;
                r = cmp(head, data);
                if (r < 0)
                        dir = RB3_RIGHT;
                else if (r > 0)
                        dir = RB3_LEFT;
                else
                        return head;
                parent = head;
        } while (head);
        *parent_out = parent;
        *dir_out = dir;
        return RB3_NULL;
}
