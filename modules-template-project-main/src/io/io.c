/**
 * io.c
 * 
 * Input/output file operations
 *  Reads a specified file into a buffer and writes a buffer to a new file that ends in _pp.
 * 
 * author: Emil Svensson
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "io.h"
#include "buffer/buffer.h"

// Reads the entire contents of a file into a buffer
// Returns 0 on success, 1 if file cannot be opened, 2 if buffer append fails
int io_read_file(const char *path, buffer_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return 1;

    char tmp[4096];
    size_t n;

    while ((n = fread(tmp, 1, sizeof(tmp), f)) > 0) {
        if (buffer_append_n(out, tmp, (long)n) != 0) {
            fclose(f);
            return 2;
        }
    }

    fclose(f);
    return 0;
}

// Writes the contents of a buffer to a file
// Returns 0 on success, 1 if file cannot be opened
int io_write_file(const char *path, const buffer_t *in)
{
    FILE *f = fopen(path, "wb");
    if (!f) return 1;

    if (in->len > 0) fwrite(in->data, 1, (size_t)in->len, f);

    fclose(f);
    return 0;
}

// Creates an output filename by inserting "_pp" before the file extension
// Returns 0 on success, 1 if buffer operations fail
int io_make_output_name(const char *input, buffer_t *out_name)
{
    /* rule: insert _pp before extension */
    const char *dot = strrchr(input, '.');
    if (!dot) return buffer_append_str(out_name, input);

    long base_len = (long)(dot - input);

    if (buffer_append_n(out_name, input, base_len) != 0) return 1;
    if (buffer_append_str(out_name, "_pp") != 0) return 1;
    if (buffer_append_str(out_name, dot) != 0) return 1;

    return 0;
}

