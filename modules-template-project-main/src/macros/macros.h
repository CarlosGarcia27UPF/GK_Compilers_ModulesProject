#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include "../tokens/tokens.h"
#include "../buffer/buffer.h"

/* Single macro entry */
typedef struct {
    char name;    
    char value;   
} macro_t;

/* Macro table */
typedef struct {
    macro_t *items;
    int size;
    int capacity;
} macro_table_t;

/* Initialize macro table */
void macros_init(macro_table_t *table);

/* Define a macro (called by Directives) */
int macros_define(macro_table_t *table,
                  const char *name,
                  const char *value);

/* Check if macro exists */
int macros_is_defined(const macro_table_t *table,
                      const char *name,
                      int name_len);

/* Get macro value (NULL if not found) */
const char *macros_get(const macro_table_t *table,
                       const char *name,
                       int name_len);

/* Expand macros in a normal code line */
int macros_expand_line(const macro_table_t *table,
                       const char *line,
                       int line_num,
                       buffer_t *output);

/* Free all macro memory */
void macros_free(macro_table_t *table);

#endif