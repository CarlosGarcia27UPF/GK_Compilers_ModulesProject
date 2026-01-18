#include <stdlib.h>
#include <string.h>

#include "buffer.h"

static int buffer_grow(buffer_t *b, long min_capacity)
{
    if (b->cap >= min_capacity) return 0;

    long new_cap = (b->cap == 0) ? 64 : b->cap;
    while (new_cap < min_capacity) {
        new_cap *= 2;
    }

    char *new_data = (char *)realloc(b->data, (size_t)new_cap);
    if (!new_data) return 1;

    b->data = new_data;
    b->cap = new_cap;
    return 0;
}

void buffer_init(buffer_t *b)
{
    b->data = NULL;
    b->len = 0;
    b->cap = 0;

    /* allocate initial so data is always valid */
    buffer_grow(b, 1);
    if (b->data) b->data[0] = '\0';
}

void buffer_free(buffer_t *b)
{
    if (!b) return;
    free(b->data);
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
}

int buffer_append_char(buffer_t *b, char c)
{
    if (!b) return 1;

    /* +1 for char, +1 for '\0' */
    if (buffer_grow(b, b->len + 2) != 0) return 1;

    b->data[b->len] = c;
    b->len++;
    b->data[b->len] = '\0';
    return 0;
}

int buffer_append_n(buffer_t *b, const char *s, long n)
{
    if (!b || !s || n < 0) return 1;

    if (n == 0) return 0;

    /* +n for content, +1 for '\0' */
    if (buffer_grow(b, b->len + n + 1) != 0) return 1;

    memcpy(b->data + b->len, s, (size_t)n);
    b->len += n;
    b->data[b->len] = '\0';
    return 0;
}

int buffer_append_str(buffer_t *b, const char *s)
{
    if (!s) return 1;
    return buffer_append_n(b, s, (long)strlen(s));
}
