/* debug stuff */

static void visit_inorder_helper(struct rb3_head *head, int isred)
{
        if (!head)
                return;
        printf(" (");
        visit_inorder_helper(rb3_get_child(head, RB3_LEFT), rb3_is_red(head, RB3_LEFT));
        printf("%s", isred ? "R" : "B");
        visit_inorder_helper(rb3_get_child(head, RB3_RIGHT), rb3_is_red(head, RB3_RIGHT));
        printf(")");
}

static void visit_inorder(struct rb3_tree *tree)
{
        visit_inorder_helper(rb3_get_root(tree), 0);
        printf("\n");
}

static int rb3_is_valid_tree_helper(struct rb3_head *head, int isred, int dir, int *depth)
{
        int i;
        int depths[2] = {1,1};

        *depth = 1;

        if (!head) {
                if (isred) {
                        printf("red leaf child!\n");
                        return 0;
                }
                return 1;
        }

        if (rb3_get_parent_dir(head) != dir) {
                printf("Directions messed up!\n");
                return 0;
        }

        for (i = 0; i < 2; i++) {
                if (isred && rb3_get_color_bit(head, i)) {
                        printf("two red in a row!\n");
                        return 0;
                }
                if (!rb3_is_valid_tree_helper(rb3_get_child(head, i),
                                              rb3_is_red(head, i), i, &depths[i]))
                        return 0;
        }
        if (depths[0] != depths[1]) {
                printf("Unbalanced tree! got %d and %d\n", depths[0], depths[1]);
                return 0;
        }
        *depth = depths[0] + !isred;

        return 1;
}

static int rb3_is_valid_tree(struct rb3_tree *tree)
{
        int depth;
        int valid;

        if (rb3_is_red(&tree->base, RB3_LEFT)) {
                printf("Error! root is red.\n");
                return 0;
        }

        valid = rb3_is_valid_tree_helper(rb3_get_root(tree), 0, 0, &depth);
        if (!valid)
                visit_inorder(tree);
        return valid;
}
