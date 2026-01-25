#ifndef IO_H
#define IO_H

#include <stddef.h>
#include "buffer/buffer.h"

int io_read_file(const char *path, buffer_t *out);
int io_write_file(const char *path, const buffer_t *in);
int io_make_output_name(const char *input, buffer_t *out_name);
void io_compute_base_dir(const char *path, char *out, size_t out_sz);

#endif
