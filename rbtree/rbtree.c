#include <stdio.h>  /* remove when debugged */
#include <assert.h>
#include <stddef.h>  /* NULL */
#include "rbtree.h"

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
                if (sil_rb_is_red(head->cld[SIL_RB_LEFT])) {
                        printf("error at %p (RED child in dir %d)\n", head, SIL_RB_LEFT);
                        return 0;
                }
                if (sil_rb_is_red(head->cld[SIL_RB_RIGHT])) {
                        printf("error at %p (RED child in dir %d)\n", head, SIL_RB_RIGHT);
                        return 0;
                }
        }
        if (!check_tree_valid(sil_rb_get_cld(head, SIL_RB_LEFT), sil_rb_get_color(head, SIL_RB_LEFT))) {
                printf("traceback: %p (%s child in dir %d)\n", head, sil_rb_get_color(head, SIL_RB_LEFT) ? "RED" : "BLACK", SIL_RB_LEFT);
                return 0;
        }
        if (!check_tree_valid(sil_rb_get_cld(head, SIL_RB_RIGHT), sil_rb_get_color(head, SIL_RB_RIGHT))) {
                printf("traceback: %p (%s child in dir %d)\n", head, sil_rb_get_color(head, SIL_RB_RIGHT) ? "RED" : "BLACK", SIL_RB_RIGHT);
                return 0;
        }
        return 1;
}


void insert_rebalance(struct sil_rb_path *path)
{
        struct sil_rb_head *head;
        struct sil_rb_head *pnt;
        struct sil_rb_head *gpnt;
        struct sil_rb_head *ggpnt;
        int left;
        int right;
        int gdir;

        assert(sil_rb_path_get_stack_size(path) >= 1);
        head = sil_rb_path_get_stack_elem(path, 0);
        pnt = sil_rb_path_get_stack_elem(path, 1);  /* might be base fake-elem */
        left = sil_rb_get_direction(pnt, head);
        right = !left;

        if (sil_rb_path_get_stack_size(path) == 1) {
                /* parent is base, head is root */
                sil_rb_set_color(pnt, left, SIL_RB_BLACK);
                return;
        }

        /* might be base fake-head */
        gpnt = sil_rb_path_get_stack_elem(path, 2);
        gdir = sil_rb_get_direction(gpnt, pnt);

        if (sil_rb_get_color(gpnt, gdir) == SIL_RB_BLACK) {
                /* parent is black */
                /* nothing to do */
                return;
        }

        /* have at least head, parent, grandparent, and the base
         * node to link grandparent */
        assert(sil_rb_path_get_stack_size(path) >= 2);
        ggpnt = sil_rb_path_get_stack_elem(path, 3);
        assert(sil_rb_get_color(ggpnt, sil_rb_get_direction(ggpnt, gpnt)) == SIL_RB_BLACK);

        if (sil_rb_is_red(gpnt->cld[!gdir])) {
                /* uncle is red (as is parent) */
                sil_rb_set_color(ggpnt, sil_rb_get_direction(ggpnt, gpnt), SIL_RB_RED);
                sil_rb_set_color(gpnt, SIL_RB_LEFT, SIL_RB_BLACK);
                sil_rb_set_color(gpnt, SIL_RB_RIGHT, SIL_RB_BLACK);
                /* continue rebalancing at grandparent */
                sil_rb_path_pop(path);
                sil_rb_path_pop(path);
                insert_rebalance(path);
        } else if (gdir == right) {
                assert(sil_rb_get_color(gpnt, !gdir) == SIL_RB_BLACK); /* uncle is black */
                pnt->cld[left] = head->cld[right];
                gpnt->cld[right] = head->cld[left];
                sil_rb_set_cld(head, left, gpnt, SIL_RB_RED);
                sil_rb_set_cld(head, right, pnt, SIL_RB_RED);
                sil_rb_set_cld(ggpnt, sil_rb_get_direction(ggpnt, gpnt), head, SIL_RB_BLACK);
        } else {
                assert(sil_rb_is_red(gpn->cld[gdir])); /* parent is red */
                assert(sil_rb_is_black(gpnt, !gdir)); /* uncle is black */
                assert(sil_rb_get_cld(pnt, left) == head);
                gpnt->cld[left] = pnt->cld[right];
                sil_rb_set_cld(pnt, right, gpnt, SIL_RB_RED);
                sil_rb_set_cld(ggpnt, sil_rb_get_direction(ggpnt, gpnt), pnt, SIL_RB_BLACK);
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

        assert(sil_rb_path_get_stack_size(path) >= 1);

        head = sil_rb_path_get_stack_elem(path, 0);
        pnt = sil_rb_path_get_stack_elem(path, 1); /* might be base head */
        left = sil_rb_get_direction(pnt, head);
        right = !left;
        sibling = sil_rb_get_cld(pnt, right);

        assert(sil_rb_get_color(pnt, sil_rb_get_direction(pnt, head)) == SIL_RB_BLACK);

        if (sil_rb_path_get_stack_size(path) == 1) {
                /* we are root */
                assert(sibling == NULL);
                return;
        }

        gpnt = sil_rb_path_get_stack_elem(path, 2); /* might be base head */
        gdir = sil_rb_get_direction(gpnt, pnt);
        sleft = sil_rb_get_cld(sibling, left);

        if (sil_rb_is_red(pnt->cld[right])) {
                assert(sil_rb_get_cld(sibling, left) && sil_rb_get_color(sibling, left) == SIL_RB_BLACK);
                assert(sil_rb_get_cld(sibling, right) && sil_rb_get_color(sibling, right) == SIL_RB_BLACK);
                /* sibling is red */
                sil_rb_set_cld(gpnt, gdir, sibling, SIL_RB_BLACK);
                sil_rb_set_cld(sibling, left, pnt, SIL_RB_RED);
                sil_rb_set_cld(pnt, right, sleft, SIL_RB_BLACK);
                sil_rb_path_pop(path);  /* pop head */
                sil_rb_path_pop(path);  /* pop parent */
                /*printf("WRITE!\n");
                 */
                sil_rb_path_INTERNAL_add(path, sibling);
                sil_rb_path_INTERNAL_add(path, pnt);
                sil_rb_path_INTERNAL_add(path, head);
                delete_rebalance(path);
        } else if (sil_rb_is_red(sibling->cld[left])) {
                /* inner child of sibling red */
                pnt->cld[right] = sleft->cld[left];
                sibling->cld[left] = sleft->cld[right];
                sil_rb_set_cld(gpnt, gdir, sleft, sil_rb_get_color(gpnt, gdir));
                sil_rb_set_cld(sleft, left, pnt, SIL_RB_BLACK);
                sil_rb_set_cld(sleft, right, sibling, SIL_RB_BLACK);
        } else if (sil_rb_is_red(sibling->cld[right])) {
                /* outer child of sibling red */
                pnt->cld[right] = sibling->cld[left];
                sil_rb_set_cld(gpnt, gdir, sibling, sil_rb_get_color(gpnt, gdir));
                sil_rb_set_cld(sibling, left, pnt, SIL_RB_BLACK);
                sil_rb_set_color(sibling, right, SIL_RB_BLACK);
        } else if (sil_rb_is_red(gpnt->cld[gdir])) {
                /* parent red */
                sil_rb_set_color(gpnt, gdir, SIL_RB_BLACK);
                sil_rb_set_color(pnt, right, SIL_RB_RED);
        } else {
                /* all nodes black */
                sil_rb_set_color(pnt, right, SIL_RB_RED);
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

        if (sil_rb_is_red(pnt->cld[dir]) || sil_rb_is_red(head->cld[clddir])) {
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
        struct sil_rb_head *oldparent;
        struct sil_rb_head *newparent;
        int oldparentdir;
        int newparentdir;

        assert(!sil_rb_path_is_empty(path));
        head = sil_rb_path_get_current(path);
        if (sil_rb_get_cld(head, SIL_RB_LEFT) == NULL) {
                delete_noninternal(path, SIL_RB_RIGHT);
        } else if (sil_rb_get_cld(head, SIL_RB_RIGHT) == NULL) {
                delete_noninternal(path, SIL_RB_LEFT);
        } else {
                /*printf("delete internal node\n");
                 */
                oldparent = path->stack[path->ptr-2];
                oldparentdir = sil_rb_get_direction(oldparent, head);
                subst = sil_rb_path_visit_inorder_successor(path);
                delete_noninternal(path, SIL_RB_RIGHT);
                *subst = *head;
                /*
                 * Look at this mess. The problem is that delete_internal()
                 * might rotate one or two nodes between head and its old
                 * parent. For substitution we need to find the new parent!
                 */
                newparent = oldparent;
                newparentdir = oldparentdir;
                if (head != sil_rb_get_cld(newparent, newparentdir)) {
                        newparent = sil_rb_get_cld(newparent, newparentdir);
                        newparentdir = SIL_RB_RIGHT;
                        if (head != sil_rb_get_cld(newparent, newparentdir))
                                newparent = sil_rb_get_cld(newparent, newparentdir);
                }
                sil_rb_ptr_set_head(&newparent->cld[newparentdir], subst);
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
        /*assert(check_path_valid(path));
        assert(check_tree_valid(path->stack[0], 0));
        */
        delete(path);
}
