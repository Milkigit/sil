#ifdef SIL_RBTREE_H_INCLUDED
#error SIL rbtree.h included twice!
#endif

#include <assert.h>


#define SIL_RBTREE_H_INCLUDED
/* XXX */
#define SIL_RB_UNUSED __attribute__((unused))

#define SIL_RB_PATH_STACK_SIZE 62

typedef enum {
        SIL_RB_LEFT = 0,
        SIL_RB_RIGHT = 1,
} sil_rb_dir_t;

typedef enum {
        SIL_RB_BLACK = 0,
        SIL_RB_RED = 2,
} sil_rb_color_t;

#define SIL_RB_IS_DIR(x) (!((x) & ~1))
#define SIL_RB_IS_COLOR(x) (!((x) & ~2))


/*
 * Special pointer type.
 *
 * Holds (at least) 4-byte aligned pointer and a direction and color
 * information.
 */
typedef unsigned long sil_rb_ptr_t;

/*
 * Red-black tree link node.
 *
 * Put this into the structures you want linked in a red-black tree.
 */
struct sil_rb_head {
        sil_rb_ptr_t cld[2];
};

/*
 * Tree elem. This is just a wrapper around a fake head for additional type
 * safety and future extensibility.
 */
struct sil_rb_tree {
        /* Special head. Contains the root node as fake left-child */
        struct sil_rb_head base;
};

/*
 * Path from top to some node.
 *
 * Since in this 2-pointer implemenetation there are no parent pointers, we need
 * to remember the path on which we found a node if we later whish to delete it
 * or insert a child there.
 *
 * Care must be taken: the first element on the path stack always points to the
 * tree base. So an "empty" path is actually one with one pointer in it!
 *
 * 64 pointers is enough for trees with 2^32 elements.
 */
struct sil_rb_path {
        int ptr;
        struct sil_rb_tree *tree;
        struct sil_rb_head *stack[SIL_RB_PATH_STACK_SIZE];
};


/*
 * Bit-twiddling on sil_rb_ptr_t
 */

static SIL_RB_UNUSED
sil_rb_ptr_t sil_rb_ptr_create(struct sil_rb_head *head, sil_rb_color_t color, sil_rb_dir_t dir)
{
        return (sil_rb_ptr_t) head | color | dir;
}

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_ptr_get_head(sil_rb_ptr_t ptr)
{
        return (struct sil_rb_head *)(ptr & ~3);
}

static SIL_RB_UNUSED
sil_rb_color_t sil_rb_ptr_get_color(sil_rb_ptr_t ptr)
{
        return ptr & 2;
}

static SIL_RB_UNUSED
int sil_rb_ptr_is_red(sil_rb_ptr_t ptr)
{
        return !!(ptr & 2);
}

static SIL_RB_UNUSED
int sil_rb_ptr_is_black(sil_rb_ptr_t ptr)
{
        return !(ptr & 2);
}

static SIL_RB_UNUSED
sil_rb_color_t sil_rb_ptr_get_dir(sil_rb_ptr_t ptr)
{
        return ptr & 1;
}

static SIL_RB_UNUSED
void sil_rb_ptr_set_head(sil_rb_ptr_t *ptr, struct sil_rb_head *head)
{
        *ptr = (*ptr & 3) | (sil_rb_ptr_t) head;
}

static SIL_RB_UNUSED
void sil_rb_ptr_set_color(sil_rb_ptr_t *ptr, sil_rb_color_t color)
{
        *ptr = (*ptr & ~2) | color;
}

static SIL_RB_UNUSED
void sil_rb_ptr_set_black(sil_rb_ptr_t *ptr)
{
        *ptr &= ~2;
}

static SIL_RB_UNUSED
void sil_rb_ptr_set_red(sil_rb_ptr_t *ptr)
{
        *ptr |= 2;
}


/*
 * Operations on struct sil_rb_head
 */

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_get_cld(struct sil_rb_head *parent, sil_rb_dir_t dir)
{
        return sil_rb_ptr_get_head(parent->cld[dir]);
}

static SIL_RB_UNUSED
void sil_rb_set_cld(struct sil_rb_head *parent, sil_rb_dir_t dir, struct sil_rb_head *child, sil_rb_color_t color)
{
        assert(SIL_RB_IS_DIR(dir));
        assert(SIL_RB_IS_COLOR(color));
        parent->cld[dir] = sil_rb_ptr_create(child, color, dir);
}

static SIL_RB_UNUSED
int sil_rb_cld_get_color(struct sil_rb_head *parent, sil_rb_dir_t dir)
{
        assert(SIL_RB_IS_DIR(dir));
        return sil_rb_ptr_get_color(parent->cld[dir]);
}

static SIL_RB_UNUSED
int sil_rb_cld_is_red(struct sil_rb_head *parent, sil_rb_dir_t dir)
{
        assert(SIL_RB_IS_DIR(dir));
        return sil_rb_ptr_is_red(parent->cld[dir]);
}

static SIL_RB_UNUSED
int sil_rb_cld_is_black(struct sil_rb_head *parent, sil_rb_dir_t dir)
{
        assert(SIL_RB_IS_DIR(dir));
        return sil_rb_ptr_is_black(parent->cld[dir]);
}

static SIL_RB_UNUSED
void sil_rb_cld_set_color(struct sil_rb_head *parent, sil_rb_dir_t dir, sil_rb_color_t color)
{
        assert(SIL_RB_IS_DIR(dir));
        assert(SIL_RB_IS_COLOR(color));
        sil_rb_ptr_set_color(&parent->cld[dir], color);
}

static SIL_RB_UNUSED
void sil_rb_cld_set_black(struct sil_rb_head *parent, sil_rb_dir_t dir)
{
        assert(SIL_RB_IS_DIR(dir));
        sil_rb_ptr_set_black(&parent->cld[dir]);
}

static SIL_RB_UNUSED
void sil_rb_cld_set_red(struct sil_rb_head *parent, sil_rb_dir_t dir)
{
        assert(SIL_RB_IS_DIR(dir));
        sil_rb_ptr_set_red(&parent->cld[dir]);
}

static SIL_RB_UNUSED
int sil_rb_get_direction(struct sil_rb_head *parent, struct sil_rb_head *head)
{
        assert(sil_rb_get_cld(parent, SIL_RB_LEFT) == head || sil_rb_get_cld(parent, SIL_RB_RIGHT) == head);
        return sil_rb_get_cld(parent, SIL_RB_RIGHT) == head;
}


/*
 * SIL_RB API
 */


/*
 * Path operations
 */

static SIL_RB_UNUSED
void sil_rb_path_reset(struct sil_rb_path *path, struct sil_rb_tree *tree)
{
        path->tree = tree;
        path->stack[0] = &tree->base;
        path->ptr = 1;
};

static SIL_RB_UNUSED
int sil_rb_path_get_stack_size(struct sil_rb_path *path)
{
        return path->ptr - 1;
}

static SIL_RB_UNUSED
int sil_rb_path_is_empty(struct sil_rb_path *path)
{
        assert(path->ptr >= 1);
        return path->ptr == 1;
}

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_path_get_current(struct sil_rb_path *path)
{
        assert(path->ptr > 0);
        return path->stack[path->ptr - 1];
}

/*
 * XXX: This can also be used to get the base fake-head by passing the stack
 * size as the index. If that's unintended the client must better check the
 * index.
 */
static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_path_get_stack_elem(struct sil_rb_path *path, int idx)
{
        assert(idx >= 0);
        assert(idx < path->ptr);
        return path->stack[path->ptr - 1 - idx];
}

static SIL_RB_UNUSED
void sil_rb_path_pop(struct sil_rb_path *path)
{
        assert(!sil_rb_path_is_empty(path));
        path->ptr -= 1;
}

static SIL_RB_UNUSED
void sil_rb_path_INTERNAL_add(struct sil_rb_path *path, struct sil_rb_head *head)
{
        assert(path->ptr > 0);
        assert(path->ptr < SIL_RB_PATH_STACK_SIZE);
        assert(head == sil_rb_get_cld(sil_rb_path_get_current(path), SIL_RB_LEFT)
            || head == sil_rb_get_cld(sil_rb_path_get_current(path), SIL_RB_RIGHT));
        path->stack[path->ptr++] = head;
}

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_path_visit_root(struct sil_rb_path *path)
{
        struct sil_rb_head *root;
        path->ptr = 1;  /* only base remains */
        root = sil_rb_get_cld(&path->tree->base, SIL_RB_LEFT);
        if (root)
                sil_rb_path_INTERNAL_add(path, root);
        return root;
}

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_path_visit_child(struct sil_rb_path *path, sil_rb_dir_t dir)
{
        struct sil_rb_head *head;
        struct sil_rb_head *child;

        assert(path->ptr < SIL_RB_PATH_STACK_SIZE);
        head = sil_rb_path_get_current(path);
        child = sil_rb_get_cld(head, dir);
        if (child)
                sil_rb_path_INTERNAL_add(path, child);
        return child;
}

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_path_visit_parent_successor(struct sil_rb_path *path)
{
        int i = path->ptr;
        while (i-- > 2) {
                if (sil_rb_get_cld(path->stack[i-1], SIL_RB_LEFT) == path->stack[i]) {
                        path->ptr = i;
                        return path->stack[i-1];
                }
        }
        return NULL;
}

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_path_visit_leftmost_descendant(struct sil_rb_path *path)
{
        assert(!sil_rb_path_is_empty(path));
        while (sil_rb_path_visit_child(path, SIL_RB_LEFT)) {
        }
        return sil_rb_path_get_current(path);
}

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_path_visit_min(struct sil_rb_path *path, struct sil_rb_tree *tree)
{
        sil_rb_path_reset(path, tree);
        if (sil_rb_path_visit_root(path) == NULL)
                return NULL;
        return sil_rb_path_visit_leftmost_descendant(path);
}

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_path_visit_inorder_successor(struct sil_rb_path *path)
{
        if (sil_rb_path_is_empty(path))
                return NULL;
        if (sil_rb_path_visit_child(path, SIL_RB_RIGHT))
                return sil_rb_path_visit_leftmost_descendant(path);
        else
                return sil_rb_path_visit_parent_successor(path);
}

/*
 * Tree operations
 */

static SIL_RB_UNUSED
void sil_rb_reset(struct sil_rb_tree *tree)
{
        tree->base.cld[SIL_RB_LEFT] = 0;
        tree->base.cld[SIL_RB_RIGHT] = 0;
}

static SIL_RB_UNUSED
int sil_rb_is_empty(struct sil_rb_tree *tree)
{
        assert(sil_rb_cld_get_color(&tree->base, SIL_RB_LEFT) == SIL_RB_BLACK);
        return sil_rb_get_cld(&tree->base, SIL_RB_LEFT) != NULL;
}

static SIL_RB_UNUSED
struct sil_rb_head *sil_rb_get_root(struct sil_rb_tree *tree)
{
        return sil_rb_get_cld(&tree->base, SIL_RB_LEFT);
}

/*
 * Insert a new node below the last node in "path", in direction "dir"
 */
void sil_rb_insert(struct sil_rb_path *path, int dir, struct sil_rb_head *child);

/*
 * Delete the last node in "path".
 */
void sil_rb_delete(struct sil_rb_path *path);
