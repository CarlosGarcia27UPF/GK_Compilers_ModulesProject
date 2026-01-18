#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
    char *data;
    long len;
    long cap;
} buffer_t;

void buffer_init(buffer_t *b);
void buffer_free(buffer_t *b);

int buffer_append_char(buffer_t *b, char c);
int buffer_append_n(buffer_t *b, const char *s, long n);
int buffer_append_str(buffer_t *b, const char *s);

#endif
