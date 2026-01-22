#ifndef PP_CONTEXT_H
#define PP_CONTEXT_H

#include "cli/cli.h"
#include "spec/pp_spec.h"
#include "comments/comments.h"
#include "macros/macros.h"
#include "directives/directives.h"

typedef struct {
    cli_options_t opt;

    const char *current_file;
    int current_line;

    int error_count;
    
    /* Comment processing state */
    comment_state_t comment_state;

    /* Macro table */
    macro_table_t macros;
    
    /* Conditional compilation stack */
    ifdef_stack_t ifdef_stack;
} pp_context_t;

#endif
