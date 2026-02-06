/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module provides the buffer data structure and public API.
 *
 * - `buffer_init`: Initializes a buffer to an empty, NUL-terminated state.
 * - `buffer_free`: Releases buffer storage and resets its fields.
 * - `buffer_append_char`: Appends a single character to the buffer.
 * - `buffer_append_n`: Appends n bytes from a source pointer.
 * - `buffer_append_str`: Appends a NUL-terminated string.
 *
 * Usage:
 *     Include this header in modules that need growable text buffers.
 *
 * Status:
 *     Active - interface used across the preprocessing engine.
 * -------------------------------------------------------------------------- */

#ifndef BUFFER_H
#define BUFFER_H

/* Initial capacity allocated when a buffer is first grown. */
#define BUFFER_INITIAL_CAPACITY 64
/* Factor used to grow buffer capacity when more space is needed. */
#define BUFFER_GROWTH_FACTOR 2
/* Minimum capacity to allocate so data pointer is always valid. */
#define BUFFER_MIN_CAPACITY 1
/* Space required for a NUL terminator in the buffer. */
#define BUFFER_NUL_TERMINATOR_SIZE 1
/* Space required to append one char plus a NUL terminator. */
#define BUFFER_APPEND_CHAR_ROOM 2
/* NUL terminator character used by the buffer. */
#define BUFFER_CHAR_NUL '\0'

/* Growable buffer used to accumulate preprocessing output. */
typedef struct {
    /* Pointer to heap storage for bytes. */
    char *data;
    /* Current number of valid bytes in data (excluding NUL). */
    long len;
    /* Allocated capacity in bytes for data. */
    long cap;
} buffer_t;

/* Initialize a buffer to an empty, NUL-terminated state. */
void buffer_init(buffer_t *b);
/* Release heap memory held by the buffer and reset fields. */
void buffer_free(buffer_t *b);

/* Append a single character to the buffer (keeps NUL terminator). */
int buffer_append_char(buffer_t *b, char c);
/* Append exactly n bytes from s to the buffer (keeps NUL terminator). */
int buffer_append_n(buffer_t *b, const char *s, long n);
/* Append a NUL-terminated string to the buffer (keeps NUL terminator). */
int buffer_append_str(buffer_t *b, const char *s);

#endif
