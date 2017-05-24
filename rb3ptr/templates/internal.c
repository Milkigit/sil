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
 * Like rb3_find() but starts at the given subtree represented by a link head
 * (which may be NULL) instead of at the root of the tree.
 */
_RB3_API
struct rb3_head *rb3_find_in_subtree(struct rb3_head *subtree, rb3_cmp, void *data);

/**
 * Find suitable insertion point for a new node in a subtree, directed by the
 * given search function. The subtree is given by its parent node `parent` and
 * child direction `dir`. The insertion point and its child direction are
 * returned in `parent_out` and `dir_out`.
 *
 * If the searched node is already in the tree (the compare function returns
 * 0), it is returned. In this case `parent_out` and `dir_out` are left
 * untouched. Otherwise NULL is returned.
 */
_RB3_API
struct rb3_head *rb3_find_parent_in_subtree(struct rb3_head *parent, int dir, rb3_cmp cmp, void *data, struct rb3_head **parent_out, int *dir_out);


/*
 * Inline implementations
 */

/* this interface is meant for code generation, not for casual consumption */
_RB3_API_STATIC_INLINE
struct rb3_head *rb3_INLINE_find_in_subtree(struct rb3_head *subtree, rb3_cmp cmp, void *data)
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
        return _RB3_NULL;
}

/* this interface is meant for code generation, not for casual consumption */
_RB3_API_STATIC_INLINE
struct rb3_head *rb3_INLINE_find_parent_in_subtree(struct rb3_head *parent, int dir, rb3_cmp cmp, void *data, struct rb3_head **parent_out, int *dir_out)
{
        struct rb3_head *head;
        int r;

        _RB3_ASSERT(parent != _RB3_NULL);
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
        return _RB3_NULL;
}
