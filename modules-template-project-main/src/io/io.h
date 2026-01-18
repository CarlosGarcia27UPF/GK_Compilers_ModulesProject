#ifndef IO_H
#define IO_H

#include "buffer/buffer.h"

int io_read_file(const char *path, buffer_t *out);
int io_write_file(const char *path, const buffer_t *in);
int io_make_output_name(const char *input, buffer_t *out_name);

#endif
