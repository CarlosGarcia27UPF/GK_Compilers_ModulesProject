/*
 * -----------------------------------------------------------------------------
 * macros.c
 *
 * Module: macros - Macro table + replacements
 * Responsible for: Storing #define mappings, replacing tokens
 *
 * Author: [Team Member 5]
 * -----------------------------------------------------------------------------
 */

#include "macros.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void macros_table_init(macro_table_t *table) {
    table->items = NULL;
    table->size = 0;
    table->capacity = 0;
}

void macros_table_free(macro_table_t *table) {
    if (!table) return;
    
    for (int i = 0; i < table->size; i++) {
        free(table->items[i].name);
        free(table->items[i].value);
    }
    free(table->items);
    table->items = NULL;
    table->size = 0;
    table->capacity = 0;
}

static int macros_grow(macro_table_t *table) {
    int new_cap = (table->capacity == 0) ? 16 : table->capacity * 2;
    macro_t *new_items = (macro_t *)realloc(table->items, new_cap * sizeof(macro_t));
    if (!new_items) return 1;
    
    table->items = new_items;
    table->capacity = new_cap;
    return 0;
}

int macros_define(macro_table_t *table, const char *name, const char *value) {
    if (!table || !name) return 1;
    
    /* Check if macro already exists - update it */
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->items[i].name, name) == 0) {
            free(table->items[i].value);
            table->items[i].value = value ? strdup(value) : strdup("");
            return 0;
        }
    }
    
    /* Add new macro */
    if (table->size >= table->capacity) {
        if (macros_grow(table) != 0) return 1;
    }
    
    table->items[table->size].name = strdup(name);
    table->items[table->size].value = value ? strdup(value) : strdup("");
    table->size++;
    return 0;
}

int macros_is_defined(const macro_table_t *table, const char *name) {
    if (!table || !name) return 0;
    
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->items[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

const char* macros_get(const macro_table_t *table, const char *name) {
    if (!table || !name) return NULL;
    
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->items[i].name, name) == 0) {
            return table->items[i].value;
        }
    }
    return NULL;
}

/* Check if character can be part of an identifier */
static int is_ident_char(int c) {
    return isalnum(c) || c == '_';
}

/* Expand macros in a line */
int macros_expand_line(const macro_table_t *table, const char *input, long input_len, buffer_t *output) {
    if (!table || !input || !output) return 1;
    
    long i = 0;
    int in_string = 0;
    int in_char = 0;
    int escaped = 0;
    
    while (i < input_len) {
        char c = input[i];
        
        /* Track string/char literals */
        if (!escaped) {
            if (c == '"' && !in_char) in_string = !in_string;
            else if (c == '\'' && !in_string) in_char = !in_char;
            else if (c == '\\' && (in_string || in_char)) escaped = 1;
        } else {
            escaped = 0;
        }
        
        /* Don't expand inside strings or char literals */
        if (in_string || in_char) {
            buffer_append_char(output, c);
            i++;
            continue;
        }
        
        /* Check if this is the start of an identifier */
        if (isalpha(c) || c == '_') {
            /* Extract identifier */
            long start = i;
            while (i < input_len && is_ident_char((unsigned char)input[i])) {
                i++;
            }
            long ident_len = i - start;
            
            /* Check if this identifier is a macro */
            char *ident = (char *)malloc(ident_len + 1);
            if (!ident) return 1;
            memcpy(ident, input + start, ident_len);
            ident[ident_len] = '\0';
            
            const char *value = macros_get(table, ident);
            if (value) {
                /* Replace with macro value */
                buffer_append_str(output, value);
            } else {
                /* Not a macro, keep as is */
                buffer_append_n(output, input + start, ident_len);
            }
            
            free(ident);
        } else {
            buffer_append_char(output, c);
            i++;
        }
    }
    
    return 0;
}
