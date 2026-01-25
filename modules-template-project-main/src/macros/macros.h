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

#include <stdio.h>
#include "../spec/pp_spec.h"
#include "../tokens/tokens.h"
#include "../buffer/buffer.h"

extern FILE* ofile;

/* Estructura para almacenar una macro: nombre y valor */
typedef struct {
    char *name;     
    char *value;     
} macro_t;

/* Tabla de macros: array dinámico */
typedef struct {
    macro_t *items;  /* Array de macros */
    int size;        /* Cantidad de macros actualmente almacenadas */
    int capacity;    /* Capacidad actual del array */
} macro_table_t;

/* Inicializar la tabla de macros vacía */
void macros_init(macro_table_t *table);

/* Agregar una macro a la tabla (nombre y valor) */
int macros_add(macro_table_t *table, const char *name, int name_len, 
               const char *value, int value_len);

/* Buscar una macro por nombre */
const char* macros_get(const macro_table_t *table, const char *name, int name_len);

/* Expandir macros en una línea de código */
int macros_expand_line(const macro_table_t *table, const char *input_line, 
                       int line_len, buffer_t *output);

/* Liberar memoria de la tabla de macros */
void macros_free(macro_table_t *table);

#endif 
