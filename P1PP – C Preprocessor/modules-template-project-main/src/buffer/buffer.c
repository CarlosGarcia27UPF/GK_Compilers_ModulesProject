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

// Ensure the buffer can hold at least min_capacity bytes.
static int buffer_grow(buffer_t *b, long min_capacity)
{
    // If the buffer already has enough space, we're done
    if (b->cap >= min_capacity) return 0;

    // Start with either the initial capacity or the current capacity
    long new_cap = (b->cap == 0) ? BUFFER_INITIAL_CAPACITY : b->cap;
    // Keep doubling the capacity until it's big enough
    while (new_cap < min_capacity) {
        new_cap *= BUFFER_GROWTH_FACTOR;
    }

    // Try to reallocate the memory to the new size
    char *new_data = (char *)realloc(b->data, (size_t)new_cap);
    if (!new_data) return 1;  // Out of memory

    // Update the buffer with the new allocation
    b->data = new_data;
    b->cap = new_cap;
    return 0;  // Success
}

// Initialize a buffer and allocate minimal storage to keep data valid.
void buffer_init(buffer_t *b)
{
    // Start with an empty buffer - no data, no length, no capacity
    b->data = NULL;
    b->len = 0;
    b->cap = 0;

    // Allocate a small initial chunk of memory so the buffer is ready to use
    buffer_grow(b, BUFFER_MIN_CAPACITY);
    // Make sure the buffer is properly null-terminated so it's a valid C string
    if (b->data) b->data[0] = BUFFER_CHAR_NUL;
}

// Free buffer storage and reset all fields to empty state.
void buffer_free(buffer_t *b)
{
    // Safety check - don't try to free a null pointer
    if (!b) return;
    // Release the dynamically allocated memory
    free(b->data);
    // Reset all fields to a safe empty state to prevent use-after-free bugs
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
}

// Append a single character to the buffer.
int buffer_append_char(buffer_t *b, char c)
{
    // Validate the buffer pointer
    if (!b) return 1;

    // Make sure we have room for the character plus the null terminator
    if (buffer_grow(b, b->len + BUFFER_APPEND_CHAR_ROOM) != 0) return 1;

    // Add the character at the end of the current content
    b->data[b->len] = c;
    // Update the length to include the new character
    b->len++;
    // Keep the buffer null-terminated so it's always a valid C string
    b->data[b->len] = BUFFER_CHAR_NUL;
    return 0;  // Success
}

// Append n bytes from s to the buffer.
int buffer_append_n(buffer_t *b, const char *s, long n)
{
    // Validate all inputs - buffer, source pointer, and length must be valid
    if (!b || !s || n < 0) return 1;

    // Nothing to do if there are no bytes to append
    if (n == 0) return 0;

    // Ensure we have enough space for the new bytes plus the null terminator
    if (buffer_grow(b, b->len + n + BUFFER_NUL_TERMINATOR_SIZE) != 0) return 1;

    // Copy the bytes from the source into the buffer at the current end position
    memcpy(b->data + b->len, s, (size_t)n);
    // Update the length to include the newly appended bytes
    b->len += n;
    // Maintain null termination for C string compatibility
    b->data[b->len] = BUFFER_CHAR_NUL;
    return 0;  // Success
}

// Append a NUL-terminated string to the buffer.
int buffer_append_str(buffer_t *b, const char *s)
{
    // Make sure the string pointer is valid
    if (!s) return 1;
    // Calculate the string length and append those bytes to the buffer
    return buffer_append_n(b, s, (long)strlen(s));
}
