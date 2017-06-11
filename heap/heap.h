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

void *heap_min(void *array, size_t elemsize);
void heap_put(void *array, size_t *size, size_t elemsize, void *elem, int (*compare)(void *a, void *b), void (*update)(void *array, size_t pos));
void heap_cancel(void *array, size_t *size, size_t elemsize, size_t pos, int (*compare)(void *a, void *b), void (*update)(void *array, size_t pos));
void heap_take(void *array, size_t *size, size_t elemsize, int (*compare)(void *a, void *b), void (*update)(void *array, size_t pos));
