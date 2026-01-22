/*
 * -----------------------------------------------------------------------------
 * macros.h
 *
 * Module: macros - Macro table + replacements
 * Responsible for: Storing #define mappings, replacing tokens
 *
 * Author: [Team Member 5]
 * -----------------------------------------------------------------------------
 */

#ifndef MACROS_H
#define MACROS_H

#include "buffer/buffer.h"

/* Macro definition */
typedef struct {
    char *name;
    char *value;
} macro_t;

/* Macro table */
typedef struct {
    macro_t *items;
    int size;
    int capacity;
} macro_table_t;

/* Initialize macro table */
void macros_table_init(macro_table_t *table);

/* Free macro table */
void macros_table_free(macro_table_t *table);

/* Add or update a macro definition */
int macros_define(macro_table_t *table, const char *name, const char *value);

/* Check if a macro is defined */
int macros_is_defined(const macro_table_t *table, const char *name);

/* Get macro value (returns NULL if not found) */
const char* macros_get(const macro_table_t *table, const char *name);

/* Expand macros in a line */
int macros_expand_line(const macro_table_t *table, const char *input, long input_len, buffer_t *output);

#endif // MACROS_H
