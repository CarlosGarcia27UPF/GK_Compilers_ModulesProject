#ifndef PP_CONTEXT_H
#define PP_CONTEXT_H

#include "cli/cli.h"
#include "spec/pp_spec.h"

typedef struct {
    cli_options_t opt;

    const char *current_file;
    int current_line;

    int error_count;

    // later:
    // macro_table_t macros;
    // if_stack_t ifs;
} pp_context_t;

#endif
