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

/* Capacidad inicial de la tabla de macros */
#define INITIAL_CAPACITY 10

/* ============================================================================
 * Función: macros_init
 * Descripción: Inicializar la tabla de macros vacía
 * Parámetros:
 *   - table: puntero a la tabla de macros a inicializar
 * ============================================================================ */
void macros_init(macro_table_t *table) {
    if (table == NULL) return;
    
    table->items = malloc(sizeof(macro_t) * INITIAL_CAPACITY);
    if (table->items == NULL) {
        fprintf(ofile, "[macros] ERROR: No hay memoria para inicializar\n");
        return;
    }
    
    table->size = 0;
    table->capacity = INITIAL_CAPACITY;
    fprintf(ofile, "[macros] Inicializado correctamente\n");
}

/* ============================================================================
 * Función: macros_add
 * Descripción: Agregar una nueva macro a la tabla
 * Parámetros:
 *   - table: tabla de macros
 *   - name: nombre de la macro (no necesariamente terminado en \0)
 *   - name_len: longitud del nombre
 *   - value: valor de la macro
 *   - value_len: longitud del valor
 * Retorna: 0 si éxito, 1 si error
 * ============================================================================ */
int macros_add(macro_table_t *table, const char *name, int name_len,
               const char *value, int value_len) {
    if (table == NULL || name == NULL || value == NULL) return 1;
    
    /* Redimensionar si es necesario */
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        macro_t *new_items = realloc(table->items, sizeof(macro_t) * table->capacity);
        if (new_items == NULL) {
            fprintf(ofile, "[macros] ERROR: No hay memoria al redimensionar\n");
            return 1;
        }
        table->items = new_items;
    }
    
    /* Copiar nombre */
    table->items[table->size].name = malloc(name_len + 1);
    if (table->items[table->size].name == NULL) {
        fprintf(ofile, "[macros] ERROR: No hay memoria para el nombre\n");
        return 1;
    }
    memcpy(table->items[table->size].name, name, name_len);
    table->items[table->size].name[name_len] = '\0';
    
    /* Copiar valor */
    table->items[table->size].value = malloc(value_len + 1);
    if (table->items[table->size].value == NULL) {
        fprintf(ofile, "[macros] ERROR: No hay memoria para el valor\n");
        free(table->items[table->size].name);
        return 1;
    }
    memcpy(table->items[table->size].value, value, value_len);
    table->items[table->size].value[value_len] = '\0';
    
    table->size++;
    fprintf(ofile, "[macros] Macro agregada: %.*s = %.*s\n", name_len, name, value_len, value);
    
    return 0;
}

/* ============================================================================
 * Función: macros_get
 * Descripción: Buscar el valor de una macro por su nombre
 * Parámetros:
 *   - table: tabla de macros
 *   - name: nombre a buscar (sin terminar en \0)
 *   - name_len: longitud del nombre
 * Retorna: puntero al valor si se encuentra, NULL si no existe
 * ============================================================================ */
const char* macros_get(const macro_table_t *table, const char *name, int name_len) {
    if (table == NULL || name == NULL) return NULL;
    
    for (int i = 0; i < table->size; i++) {
        /* Comparar longitud primero */
        if (strlen(table->items[i].name) == name_len) {
            /* Comparar contenido */
            if (strncmp(table->items[i].name, name, name_len) == 0) {
                return table->items[i].value;
            }
        }
    }
    
    return NULL;
}

/* ============================================================================
 * Función: macros_expand_line
 * Descripción: Expandir todas las macros en una línea de código
 *              Lee token por token, si encuentra un identificador que es una
 *              macro lo reemplaza por su valor, si no lo deja igual.
 * Parámetros:
 *   - table: tabla de macros
 *   - input_line: línea de entrada sin procesar
 *   - line_len: longitud de la línea
 *   - output: buffer donde escribir la línea expandida
 * Retorna: 0 si éxito, 1 si error
 * ============================================================================ */
int macros_expand_line(const macro_table_t *table, const char *input_line,
                       int line_len, buffer_t *output) {
    if (table == NULL || input_line == NULL || output == NULL) return 1;
    
    /* Inicializar tokenizador */
    Tokenizer tkz;
    tokens_init(&tkz, 1, (char *)input_line);
    
    Token token;
    
    /* Procesar token por token */
    while (tokenize(&tkz, &token)) {
        
        if (token.type == IDENTIFIER) {
            /* Es un identificador: buscar si es una macro */
            const char *macro_value = macros_get(table, token.word, token.length);
            
            if (macro_value != NULL) {
                /* Es una macro: reemplazar por su valor */
                if (buffer_append(output, macro_value, strlen(macro_value)) != 0) {
                    fprintf(ofile, "[macros] ERROR: No hay memoria en buffer\n");
                    return 1;
                }
                fprintf(ofile, "[macros] Macro expandida: %.*s -> %s\n", token.length, token.word, macro_value);
            } else {
                /* No es una macro: dejar igual */
                if (buffer_append(output, token.word, token.length) != 0) {
                    fprintf(ofile, "[macros] ERROR: No hay memoria en buffer\n");
                    return 1;
                }
            }
        } 
        else {
            /* Es cualquier otro token (número, símbolo, string): dejar igual */
            if (buffer_append(output, token.word, token.length) != 0) {
                fprintf(ofile, "[macros] ERROR: No hay memoria en buffer\n");
                return 1;
            }
        }
        
        /* Agregar espacios entre tokens excepto al final */
        /* Nota: Los espacios se preservan naturalmente por el tokenizador */
    }
    
    return 0;
}

/* ============================================================================
 * Función: macros_free
 * Descripción: Liberar toda la memoria ocupada por la tabla de macros
 * Parámetros:
 *   - table: tabla de macros a liberar
 * ============================================================================ */
void macros_free(macro_table_t *table) {
    if (table == NULL) return;
    
    for (int i = 0; i < table->size; i++) {
        free(table->items[i].name);
        free(table->items[i].value);
    }
    free(table->items);
    table->size = 0;
    table->capacity = 0;
    fprintf(ofile, "[macros] Memoria liberada\n");
}
