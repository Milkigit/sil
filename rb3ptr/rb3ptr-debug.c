/*
 * Copyright (c) 2017 Jens Stimpfle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include "rb3ptr.h"

int rb3_is_valid_tree(struct rb3_head *head, int isred)
{
        struct rb3_head *left;
        struct rb3_head *right;
        int isleftred;
        int isrightred;

        if (!head)
                return 1;

        left = rb3_get_child(head, RB3_LEFT);
        right = rb3_get_child(head, RB3_RIGHT);
        isleftred = rb3_is_red(head, RB3_LEFT);
        isrightred = rb3_is_red(head, RB3_RIGHT);

        if (isred && isleftred)
                return 0;
        if (isred && isrightred)
                return 0;

        if (left && rb3_get_parent_dir(left) != RB3_LEFT)
                return 0;
        if (right && rb3_get_parent_dir(right) != RB3_RIGHT)
                return 0;

        return rb3_is_valid_tree(left, isleftred)
                && rb3_is_valid_tree(right, isrightred);
}

void rb3_inorder_traversal(struct rb3_head *head, void (*action)(struct rb3_head *, void *), void *arg)
{
        struct rb3_head *child;

        if (!head)
                return;

        action(head, arg);

        if (rb3_get_child(head, RB3_LEFT)) {
                child = rb3_get_child(head, RB3_LEFT);
                rb3_inorder_traversal(child, action, arg);
                action(head, arg);
        }

        if (rb3_get_child(head, RB3_RIGHT)) {
                child = rb3_get_child(head, RB3_RIGHT);
                rb3_inorder_traversal(child, action, arg);
                action(head, arg);
        }
}
