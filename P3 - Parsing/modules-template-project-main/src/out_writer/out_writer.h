/*
 * =============================================================================
 * out_writer.h
 * Debug-output file writer for the SRA parser.
 *
 * Opens/closes the _p3dbg.txt output file and writes one formatted line
 * for each SHIFT, REDUCE, ACCEPT, or ERROR step taken by the automaton.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef OUT_WRITER_H
#define OUT_WRITER_H

#include <stdio.h>
#include "../lang_spec/lang_spec.h"
#include "../parser_stack/parser_stack.h"

/* ---- Trace flag ---- */
#define TRACE_OUT_WRITER 0  /* Set to 1 to enable out_writer debug trace output */

/* ---- Constants ---- */
#define OW_DBG_SUFFIX     "_p3dbg.txt"  /* Suffix appended to the base filename for debug output */
#define OW_BASE_BUF_SIZE  256           /* Buffer size for the basename (no extension) string    */
#define OW_DBG_PATH_SIZE  512           /* Buffer size for the full debug output file path       */
#define OW_FOPEN_WRITE_MODE "w"         /* fopen mode for writing (create/overwrite) debug file  */

/* ---- Step output format strings ---- */
#define OW_FMT_SHIFT  "[SHIFT  ] pos=%-3d state=%d->%-3d token=%s(\"%s\")  stack: "
                      /* Format: position, prev_state->next_state, token_name(lexeme), then stack */
#define OW_FMT_REDUCE "[REDUCE%d] pos=%-3d state=%d->%-3d rule=%-12s stack: "
                      /* Format: rule_num, position, prev_state->new_state, rule_desc, then stack */
#define OW_FMT_ACCEPT "[ACCEPT ] pos=%-3d state=%-4d ACCEPTED  stack: "
                      /* Format: position, state, then stack trace */
#define OW_FMT_ERROR  "[ERROR  ] pos=%-3d state=%-4d token=%s  PARSE ERROR\n"
                      /* Format: position, state, token_name */
#define OW_LINE_END   "\n"    /* Line terminator written after each stack trace       */
#define OW_DBG_PATH_FMT "%s%s" /* snprintf format to build <basename><suffix> path   */

/* ---- Function prototypes ---- */

/* Build the debug output filename: <basename>_p3dbg.txt.
 * Writes the result into out_buf (size len). */
void ow_build_dbg_filename(const char *input_file, char *out_buf, int len);

/* Open (create/overwrite) the debug output file. Returns NULL on failure. */
FILE *ow_open_dbg(const char *input_file);

/* Write a SHIFT step line. */
void ow_write_shift(FILE *f, int pos, int prev_state, int next_state,
                    const char *term_name, const char *lexeme,
                    const parser_stack_t *stack, const lang_spec_t *ls);

/* Write a REDUCE step line. */
void ow_write_reduce(FILE *f, int pos, int rule_num, int prev_state,
                     int new_state, const char *rule_desc,
                     const parser_stack_t *stack, const lang_spec_t *ls);

/* Write an ACCEPT step line. */
void ow_write_accept(FILE *f, int pos, int state,
                     const parser_stack_t *stack, const lang_spec_t *ls);

/* Write an ERROR step line. */
void ow_write_error(FILE *f, int pos, int state, const char *term_name);

/* Flush and close the debug output file. */
void ow_close_dbg(FILE *f);

#endif /* OUT_WRITER_H */
