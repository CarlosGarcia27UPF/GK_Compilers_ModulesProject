/*
 * =============================================================================
 * sra.h
 * Shift-Reduce Automaton (SRA) engine interface.
 *
 * sra_run drives the main parse loop: it reads tokens, consults the ACTION
 * table, and performs SHIFT, REDUCE, ACCEPT, or ERROR actions until the
 * input is fully parsed or an error is detected.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef SRA_H
#define SRA_H

#include "../lang_spec/lang_spec.h"
#include "../token_loader/token_loader.h"
#include "../parser_stack/parser_stack.h"
#include "../out_writer/out_writer.h"

/* ---- Trace flag ---- */
#define TRACE_SRA 0  /* Set to 1 to enable SRA engine debug trace output */

/* ---- Constants ---- */
#define UNKNOWN_SYMBOL_NAME "?"  /* Displayed when a terminal index is out of range */

/* Run the SRA engine.
 * - ls        : loaded language specification (ACTION/GOTO tables)
 * - tokens    : token list produced by tok_load
 * - stack     : an uninitialised parser_stack_t (will be ps_init'd)
 * - dbg_file  : open debug output file (may be NULL to suppress output)
 *
 * Returns 1 if the input is accepted, 0 if a parse error occurs. */
int sra_run(const lang_spec_t *ls, const parser_token_list_t *tokens,
            parser_stack_t *stack, FILE *dbg_file);

#endif /* SRA_H */
