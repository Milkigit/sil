#include <stdio.h>  /* remove when debugged */
#include <assert.h>
#include <stddef.h>  /* NULL */
#include "sil_rb2ptr.h"

/*
static int SIL_RB_UNUSED check_path_valid(struct sil_rb_path *path)
{
        size_t i;
        for (i = 1; i < path->ptr; i++)
                if(path->stack[i] != sil_rb_get_cld(path->stack[i-1], SIL_RB_LEFT)
                    && path->stack[i] != sil_rb_get_cld(path->stack[i-1], SIL_RB_RIGHT))
                        return 0;
        return 1;
}

static int SIL_RB_UNUSED check_tree_valid(struct sil_rb_head *head, int isred)
{
        if (head == NULL) {
                if (isred)
                        printf("error at leaf %p\n", head);
                return !isred;
        }
        if (isred) {
                if (sil_rb_cld_is_red(head, SIL_RB_LEFT)) {
                        printf("error at %p (RED child in dir %d)\n", head, SIL_RB_LEFT);
                        return 0;
                }
                if (sil_rb_cld_is_red(head, SIL_RB_RIGHT)) {
                        printf("error at %p (RED child in dir %d)\n", head, SIL_RB_RIGHT);
                        return 0;
                }
        }
        if (!check_tree_valid(sil_rb_get_cld(head, SIL_RB_LEFT), sil_rb_cld_get_color(head, SIL_RB_LEFT))) {
                printf("traceback: %p (%s child in dir %d)\n", head, sil_rb_cld_get_color(head, SIL_RB_LEFT) ? "RED" : "BLACK", SIL_RB_LEFT);
                return 0;
        }
        if (!check_tree_valid(sil_rb_get_cld(head, SIL_RB_RIGHT), sil_rb_cld_get_color(head, SIL_RB_RIGHT))) {
                printf("traceback: %p (%s child in dir %d)\n", head, sil_rb_cld_get_color(head, SIL_RB_RIGHT) ? "RED" : "BLACK", SIL_RB_RIGHT);
                return 0;
        }
        return 1;
}
*/


void insert_rebalance(struct sil_rb_path *path)
{
        struct sil_rb_head *head;
        struct sil_rb_head *pnt;
        struct sil_rb_head *gpnt;
        struct sil_rb_head *ggpnt;
        /*
         * Left and right could actually be right and left, but we just
         * normalize directions to have to think through only one variant.
         * So w.l.o.g, let the direction from parent to head be left.
         */
        int left;
        int right;
        /*
         * Directions from gpnt to pnt and ggpnt to gpnt
         */
        int gdir;
        int ggdir;

        assert(sil_rb_path_get_stack_size(path) >= 1);
        head = sil_rb_path_get_stack_elem(path, 0);
        pnt = sil_rb_path_get_stack_elem(path, 1);  /* might be base fake-elem */
        left = sil_rb_get_direction(pnt, head);
        right = !left;

        if (sil_rb_path_get_stack_size(path) == 1) {
                /* parent is base, head is root */
                sil_rb_cld_set_color(pnt, left, SIL_RB_BLACK);
                return;
        }

        gpnt = sil_rb_path_get_stack_elem(path, 2);
        gdir = sil_rb_get_direction(gpnt, pnt);

        if (sil_rb_cld_is_black(gpnt, gdir)) {
                /* parent is black */
                /* nothing to do */
                return;
        }

        /*
         * Have at least head, parent, grandparent, grand-grandparent.
         * grand-grandparent might be fake base-elem, though.
         */
        ggpnt = sil_rb_path_get_stack_elem(path, 3);
        ggdir = sil_rb_get_direction(ggpnt, gpnt);
        /* grandparent must be black because must hold because parent is red */
        assert(sil_rb_cld_is_black(ggpnt, ggdir));

        if (sil_rb_cld_is_red(gpnt, !gdir)) {
                /* uncle is red (as is parent) */
                sil_rb_cld_set_red(ggpnt, sil_rb_get_direction(ggpnt, gpnt));
                sil_rb_cld_set_black(gpnt, SIL_RB_LEFT);
                sil_rb_cld_set_black(gpnt, SIL_RB_RIGHT);
                /* continue rebalancing at grandparent */
                sil_rb_path_pop(path);
                sil_rb_path_pop(path);
                insert_rebalance(path);
        } else if (gdir == right) {
                pnt->cld[left] = head->cld[right];
                gpnt->cld[right] = head->cld[left];
                sil_rb_set_cld(head, left, gpnt, SIL_RB_RED);
                sil_rb_set_cld(head, right, pnt, SIL_RB_RED);
                sil_rb_set_cld(ggpnt, ggdir, head, SIL_RB_BLACK);
        } else {
                assert(sil_rb_get_cld(pnt, left) == head);
                gpnt->cld[left] = pnt->cld[right];
                sil_rb_set_cld(pnt, right, gpnt, SIL_RB_RED);
                sil_rb_set_cld(ggpnt, ggdir, pnt, SIL_RB_BLACK);
        }
}

static void delete_rebalance(struct sil_rb_path *path)
{
        struct sil_rb_head *head;
        struct sil_rb_head *pnt;
        struct sil_rb_head *sibling;
        struct sil_rb_head *gpnt;
        struct sil_rb_head *sleft;  /* inner child of sibling */
        int left;
        int right;
        int gdir;

        if (sil_rb_path_get_stack_size(path) == 1) {
                /* we are root */
                return;
        }

        assert(sil_rb_path_get_stack_size(path) >= 1);
        head = sil_rb_path_get_stack_elem(path, 0);
        pnt = sil_rb_path_get_stack_elem(path, 1);  /* could be base head */
        left = sil_rb_get_direction(pnt, head);
        right = !left;
        sibling = sil_rb_get_cld(pnt, right);
        assert(sil_rb_cld_is_black(pnt, sil_rb_get_direction(pnt, head)));

        gpnt = sil_rb_path_get_stack_elem(path, 2);  /* could be base head */
        gdir = sil_rb_get_direction(gpnt, pnt);
        sleft = sil_rb_get_cld(sibling, left);

        if (sil_rb_cld_is_red(pnt, right)) {
                assert(sil_rb_get_cld(sibling, left) && sil_rb_cld_is_black(sibling, left));
                assert(sil_rb_get_cld(sibling, right) && sil_rb_cld_is_black(sibling, right));
                /* sibling is red */
                sil_rb_set_cld(gpnt, gdir, sibling, SIL_RB_BLACK);
                sil_rb_set_cld(sibling, left, pnt, SIL_RB_RED);
                sil_rb_set_cld(pnt, right, sleft, SIL_RB_BLACK);
                sil_rb_path_pop(path);  /* pop head */
                sil_rb_path_pop(path);  /* pop parent */
                sil_rb_path_INTERNAL_add(path, sibling);
                sil_rb_path_INTERNAL_add(path, pnt);
                sil_rb_path_INTERNAL_add(path, head);
                delete_rebalance(path);
        } else if (sil_rb_cld_is_red(sibling, left)) {
                /* inner child of sibling red */
                pnt->cld[right] = sleft->cld[left];
                sibling->cld[left] = sleft->cld[right];
                sil_rb_ptr_set_head(&gpnt->cld[gdir], sleft);
                sil_rb_set_cld(sleft, left, pnt, SIL_RB_BLACK);
                sil_rb_set_cld(sleft, right, sibling, SIL_RB_BLACK);
        } else if (sil_rb_cld_is_red(sibling, right)) {
                /* outer child of sibling red */
                pnt->cld[right] = sibling->cld[left];
                sil_rb_ptr_set_head(&gpnt->cld[gdir], sibling);
                sil_rb_set_cld(sibling, left, pnt, SIL_RB_BLACK);
                sil_rb_cld_set_color(sibling, right, SIL_RB_BLACK);
        } else if (sil_rb_cld_is_red(gpnt, gdir)) {
                /* parent red */
                sil_rb_cld_set_color(gpnt, gdir, SIL_RB_BLACK);
                sil_rb_cld_set_color(pnt, right, SIL_RB_RED);
        } else {
                /* all nodes black */
                sil_rb_cld_set_color(pnt, right, SIL_RB_RED);
                sil_rb_path_pop(path);  /* pop head */
                delete_rebalance(path);
        }
}

static void insert(struct sil_rb_path *path, int dir, struct sil_rb_head *child)
{
        struct sil_rb_head *head;
        head = sil_rb_path_get_stack_elem(path, 0);
        if (sil_rb_path_is_empty(path)) {
                /* head is base elem */
                sil_rb_set_cld(head, SIL_RB_LEFT, child, SIL_RB_BLACK);
                return;
        }
        assert(sil_rb_get_cld(head, dir) == NULL);
        sil_rb_set_cld(child, SIL_RB_LEFT, NULL, SIL_RB_BLACK);
        sil_rb_set_cld(child, SIL_RB_RIGHT, NULL, SIL_RB_BLACK);
        sil_rb_set_cld(head, dir, child, SIL_RB_RED);
        sil_rb_path_INTERNAL_add(path, child);
        insert_rebalance(path);
}

/* clddir: if the non-internal node has one child, its direction. Otherwise
 * SIL_RB_LEFT
 */
static void delete_noninternal(struct sil_rb_path *path, int clddir)
{
        struct sil_rb_head *head;
        struct sil_rb_head *pnt;
        int dir;

        assert(sil_rb_path_get_stack_size(path) >= 1);
        head = sil_rb_path_get_current(path);
        pnt = sil_rb_path_get_stack_elem(path, 1);
        dir = sil_rb_get_direction(pnt, head);
        assert(sil_rb_get_cld(head, !clddir) == NULL);

        if (sil_rb_cld_is_red(pnt, dir) || sil_rb_cld_is_red(head, clddir)) {
                sil_rb_set_cld(pnt, dir, sil_rb_get_cld(head, clddir), SIL_RB_BLACK);
        } else {
                sil_rb_set_cld(pnt, dir, sil_rb_get_cld(head, clddir), SIL_RB_BLACK);
                path->stack[path->ptr-1] = sil_rb_get_cld(head, clddir);
                delete_rebalance(path);
        }
}

static void delete(struct sil_rb_path *path)
{
        struct sil_rb_head *head;
        struct sil_rb_head *subst;
        struct sil_rb_head *parent;
        int parentdir;

        assert(!sil_rb_path_is_empty(path));
        head = sil_rb_path_get_current(path);
        if (sil_rb_get_cld(head, SIL_RB_LEFT) == NULL) {
                delete_noninternal(path, SIL_RB_RIGHT);
        } else if (sil_rb_get_cld(head, SIL_RB_RIGHT) == NULL) {
                delete_noninternal(path, SIL_RB_LEFT);
        } else {
                /*printf("delete internal node\n");
                 */
                parent = path->stack[path->ptr-2];
                parentdir = sil_rb_get_direction(parent, head);
                subst = sil_rb_path_visit_inorder_successor(path);
                delete_noninternal(path, SIL_RB_RIGHT);
                *subst = *head;
                /*
                 * Look at this mess. The problem is that delete_internal()
                 * could have rotated one or two nodes between head and its old
                 * parent. For substitution we need to find the new parent!
                 */
                if (head != sil_rb_get_cld(parent, parentdir)) {
                        parent = sil_rb_get_cld(parent, parentdir);
                        parentdir = SIL_RB_RIGHT;
                        if (head != sil_rb_get_cld(parent, parentdir))
                                parent = sil_rb_get_cld(parent, parentdir);
                }
                sil_rb_ptr_set_head(&parent->cld[parentdir], subst);
        }
}

/*
 * EXTERNAL INTERFACES
 */

void sil_rb_insert(struct sil_rb_path *path, int dir, struct sil_rb_head *child)
{
        insert(path, dir, child);
}

void sil_rb_delete(struct sil_rb_path *path)
{
        delete(path);
}
