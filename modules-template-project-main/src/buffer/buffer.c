/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module provides functionality to build and grow a dynamic text buffer.
 *
 * - `buffer_init`: Initializes a buffer to an empty, NUL-terminated state.
 * - `buffer_free`: Releases buffer storage and resets its fields.
 * - `buffer_append_char`: Appends a single character to the buffer.
 * - `buffer_append_n`: Appends n bytes from a source pointer.
 * - `buffer_append_str`: Appends a NUL-terminated string.
 *
 * Usage:
 *     Called by preprocessing modules to accumulate output and intermediate lines.
 *
 * Status:
 *     Active - core utility used by multiple modules.
 * -------------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "buffer.h"

/* Ensure the buffer can hold at least min_capacity bytes. */
static int buffer_grow(buffer_t *b, long min_capacity)
{
    if (b->cap >= min_capacity) return 0;

    long new_cap = (b->cap == 0) ? BUFFER_INITIAL_CAPACITY : b->cap;
    while (new_cap < min_capacity) {
        new_cap *= BUFFER_GROWTH_FACTOR;
    }

    char *new_data = (char *)realloc(b->data, (size_t)new_cap);
    if (!new_data) return 1;

    b->data = new_data;
    b->cap = new_cap;
    return 0;
}

/* Initialize a buffer and allocate minimal storage to keep data valid. */
void buffer_init(buffer_t *b)
{
    b->data = NULL;
    b->len = 0;
    b->cap = 0;

    buffer_grow(b, BUFFER_MIN_CAPACITY);
    if (b->data) b->data[0] = BUFFER_CHAR_NUL;
}

/* Free buffer storage and reset all fields to empty state. */
void buffer_free(buffer_t *b)
{
    if (!b) return;
    free(b->data);
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
}

/* Append a single character to the buffer. */
int buffer_append_char(buffer_t *b, char c)
{
    if (!b) return 1;

    if (buffer_grow(b, b->len + BUFFER_APPEND_CHAR_ROOM) != 0) return 1;

    b->data[b->len] = c;
    b->len++;
    b->data[b->len] = BUFFER_CHAR_NUL;
    return 0;
}

/* Append n bytes from s to the buffer. */
int buffer_append_n(buffer_t *b, const char *s, long n)
{
    if (!b || !s || n < 0) return 1;

    if (n == 0) return 0;

    if (buffer_grow(b, b->len + n + BUFFER_NUL_TERMINATOR_SIZE) != 0) return 1;

    memcpy(b->data + b->len, s, (size_t)n);
    b->len += n;
    b->data[b->len] = BUFFER_CHAR_NUL;
    return 0;
}

/* Append a NUL-terminated string to the buffer. */
int buffer_append_str(buffer_t *b, const char *s)
{
    if (!s) return 1;
    return buffer_append_n(b, s, (long)strlen(s));
}
