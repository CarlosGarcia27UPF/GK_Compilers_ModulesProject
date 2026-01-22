/*
 * -----------------------------------------------------------------------------
 * directives.h
 *
 * Module: directives - Directive detection and execution
 * Responsible for: #include, #define, #ifdef/#endif processing
 *
 * Author: Carlos García
 * -----------------------------------------------------------------------------
 */

#ifndef DIRECTIVES_H
#define DIRECTIVES_H

#include "buffer/buffer.h"
#include "macros/macros.h"
#include "spec/pp_spec.h"

/* Conditional stack for #ifdef/#endif */
typedef struct {
    int stack[PP_MAX_IF_DEPTH];  /* 1 = include code, 0 = skip code */
    int top;
} ifdef_stack_t;

/* Initialize ifdef stack */
void ifdef_stack_init(ifdef_stack_t *stack);

/* Check if we should currently include code */
int ifdef_should_include(const ifdef_stack_t *stack);

/* Process a directive line
 * Returns: 
 *   0 if directive was processed successfully
 *   1 if there was an error
 *   2 if line should be skipped (inside false #ifdef)
 */
int directives_process_line(const char *line, long line_len, 
                           const char *base_dir,
                           const char *current_file, int line_num,
                           macro_table_t *macros, 
                           ifdef_stack_t *ifdef_stack,
                           buffer_t *output);

#endif // DIRECTIVES_H
