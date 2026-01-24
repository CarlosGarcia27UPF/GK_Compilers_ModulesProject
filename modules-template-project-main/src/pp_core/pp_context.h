/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module defines the shared preprocessing context structure.
 *
 * - `pp_context_t`: Stores options, current file/line, error count, and state.
 *
 * Usage:
 *     Included by core, directives, and macro modules to share run state.
 *
 * Status:
 *     Active - central context type for preprocessing.
 * -------------------------------------------------------------------------- */

#ifndef PP_CONTEXT_H
#define PP_CONTEXT_H

#include "cli/cli.h"
#include "spec/pp_spec.h"

/* Shared state for a preprocessing run. */
typedef struct {
    /* Parsed CLI options for this run. */
    cli_options_t opt;

    /* Current input file path being processed. */
    const char *current_file;
    /* Current 1-based line number within current_file. */
    int current_line;

    /* Comment processing state. */
    comment_state_t comment_state;

    /* Macro table for #define storage and expansion. */
    macro_table_t macros;
    
    /* Conditional compilation stack for #ifdef/#endif. */
    ifdef_stack_t ifdef_stack;
} pp_context_t;

#endif
