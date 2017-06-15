/*
 * Generic heap implementation. For more information, see README.txt
 *
 * Copyright (c) 2017 Jens Stimpfle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <stddef.h>  /* size_t */
#include <string.h>  /* memcpy() */
#include "heap.h"


#define HEAP_AT(n) (void *) &((unsigned char *) array)[(n) * elemsize]
#define HEAP_ASSIGN(pos, b) do { memcpy(HEAP_AT(pos), b, elemsize); if (update) update(array, pos); } while (0)


void *heap_min(void *array, size_t elemsize)
{
        return HEAP_AT(0);
}

static size_t heap_bubble_up(void *array, size_t elemsize, size_t pos, void *elem, int (*compare)(void *a, void *b), void (*update)(void *array, size_t pos))
{
        size_t pnt;

        for (;;) {
                pnt = (pos-1) / 2;
                if (pnt == 0)
                        break;
                if (compare(HEAP_AT(pnt), elem) <= 0)
                        break;
                HEAP_ASSIGN(pos, HEAP_AT(pnt));
                pos = pnt;
        }
        return pos;
}

static size_t heap_bubble_down(void *array, size_t size, size_t elemsize, size_t pos, void *elem, int (*compare)(void *a, void *b), void (*update)(void *array, size_t pos))
{
        size_t cld;

        for (;;) {
                cld = (pos*2) + 1;  /* left child of pos */
                if (cld >= size)
                        break;
                if (cld+1 < size && compare(HEAP_AT(cld), HEAP_AT(cld+1)) > 0)
                        cld = cld + 1;  /* right child of pos */
                if (compare(elem, HEAP_AT(cld)) <= 0)
                        break;
                HEAP_ASSIGN(pos, HEAP_AT(cld));
                pos = cld;
        }
        return pos;
}

void heap_put(void *array, size_t *size, size_t elemsize, void *elem, int (*compare)(void *a, void *b), void (*update)(void *array, size_t pos))
{
        size_t pos;

        pos = (*size)++;
        pos = heap_bubble_up(array, elemsize, pos, elem, compare, update);
        HEAP_ASSIGN(pos, elem);
}

void heap_cancel(void *array, size_t *size, size_t elemsize, size_t pos, int (*compare)(void *a, void *b), void (*update)(void *array, size_t pos))
{
        void *elem;

        *size = *size - 1;
        elem = HEAP_AT(*size);

        pos = heap_bubble_up(array, elemsize, pos, elem, compare, update);
        pos = heap_bubble_down(array, *size, elemsize, pos, elem, compare, update);
        HEAP_ASSIGN(pos, elem);
}

void heap_take(void *array, size_t *size, size_t elemsize, int (*compare)(void *a, void *b), void (*update)(void *array, size_t pos))
{
        return heap_cancel(array, size, elemsize, 0, compare, update);
}
