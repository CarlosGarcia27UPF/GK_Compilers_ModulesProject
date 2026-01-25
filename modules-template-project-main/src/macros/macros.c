#include "macros.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 8

/* -------------------------------------------------- */
void macros_init(macro_table_t *table)
{
    table->size = 0;
    table->capacity = INITIAL_CAPACITY;
    table->items = malloc(sizeof(macro_t) * table->capacity);
}

/* -------------------------------------------------- */
static void ensure_capacity(macro_table_t *table)
{
    if (table->size < table->capacity) return;

    table->capacity *= 2;
    table->items = realloc(table->items,
                            sizeof(macro_t) * table->capacity);
}

/* -------------------------------------------------- */
int macros_define(macro_table_t *table,
                  const char *name,
                  const char *value)
{
    if (!table || !name || !value) return 1;

    ensure_capacity(table);

    table->items[table->size].name = strdup(name);
    table->items[table->size].value = strdup(value);
    table->size++;

    return 0;
}

/* -------------------------------------------------- */
int macros_is_defined(const macro_table_t *table,
                      const char *name,
                      int name_len)
{
    if (!table) return 0;

    for (int i = 0; i < table->size; i++) {
        if ((int)strlen(table->items[i].name) == name_len &&
            strncmp(table->items[i].name, name, name_len) == 0) {
            return 1;
        }
    }
    return 0;
}

/* -------------------------------------------------- */
const char *macros_get(const macro_table_t *table,
                       const char *name,
                       int name_len)
{
    if (!table) return NULL;

    for (int i = 0; i < table->size; i++) {
        if ((int)strlen(table->items[i].name) == name_len &&
            strncmp(table->items[i].name, name, name_len) == 0) {
            return table->items[i].value;
        }
    }
    return NULL;
}

/* -------------------------------------------------- */
int macros_expand_line(const macro_table_t *table,
                       const char *line,
                       int line_num,
                       buffer_t *output)
{
    Tokenizer tk;
    Token tok;

    tokens_init(&tk, line_num, (char *)line);

    while (tokenize(&tk, &tok)) {

        /* Never expand inside strings */
        if (tok.type == STRING) {
            buffer_append(output, tok.word, tok.length);
            continue;
        }

        if (tok.type == IDENTIFIER) {
            const char *val = macros_get(table, tok.word, tok.length);
            if (val) {
                buffer_append(output, val, strlen(val));
            } else {
                buffer_append(output, tok.word, tok.length);
            }
        } else {
            buffer_append(output, tok.word, tok.length);
        }
    }

    return 0;
}

/* -------------------------------------------------- */
void macros_free(macro_table_t *table)
{
    if (!table) return;

    for (int i = 0; i < table->size; i++) {
        free(table->items[i].name);
        free(table->items[i].value);
    }
    free(table->items);

    table->size = 0;
    table->capacity = 0;
}