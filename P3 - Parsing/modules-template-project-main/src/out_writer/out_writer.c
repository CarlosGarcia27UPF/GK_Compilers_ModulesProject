/*
 * =============================================================================
 * out_writer.c
 * Implementation of the SRA debug-output file writer.
 *
 * Author: [Team]
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>
#include "out_writer.h"

/* ---- Filename helpers ---- */

/* Copy the base name (no directory, no extension) of path into buf. */
static void get_basename_no_ext(const char *path, char *buf, int len)
{
    const char *last_slash = strrchr(path, '/');
    const char *start = last_slash ? last_slash + 1 : path;
    const char *dot   = strrchr(start, '.');
    int copy_len = dot ? (int)(dot - start) : (int)strlen(start);
    if (copy_len >= len) copy_len = len - 1;
    strncpy(buf, start, (size_t)copy_len);
    buf[copy_len] = '\0';
}

/* Build the debug filename: <basename>_p3dbg.txt */
void ow_build_dbg_filename(const char *input_file, char *out_buf, int len)
{
    char base[OW_BASE_BUF_SIZE];
    get_basename_no_ext(input_file, base, OW_BASE_BUF_SIZE);
    snprintf(out_buf, (size_t)len, OW_DBG_PATH_FMT, base, OW_DBG_SUFFIX);
}

/* Open (create/overwrite) the debug output file. */
FILE *ow_open_dbg(const char *input_file)
{
    char dbg_name[OW_DBG_PATH_SIZE];
    ow_build_dbg_filename(input_file, dbg_name, (int)sizeof(dbg_name));
    FILE *f = fopen(dbg_name, OW_FOPEN_WRITE_MODE);
    if (!f) fprintf(stderr, "ow_open_dbg: cannot open '%s'\n", dbg_name);
#if TRACE_OUT_WRITER
    fprintf(stderr, "[OUT_WRITER] debug file: %s\n", dbg_name);
#endif
    return f;
}

/* ---- Step writers ---- */

/* Write a SHIFT step. */
void ow_write_shift(FILE *f, int pos, int prev_state, int next_state,
                    const char *term_name, const char *lexeme,
                    const parser_stack_t *stack, const lang_spec_t *ls)
{
    if (!f) return;
    fprintf(f, OW_FMT_SHIFT,
            pos, prev_state, next_state, term_name, lexeme);
    ps_print(stack, ls, f);
    fprintf(f, OW_LINE_END);
    fflush(f);
}

/* Write a REDUCE step. */
void ow_write_reduce(FILE *f, int pos, int rule_num, int prev_state,
                     int new_state, const char *rule_desc,
                     const parser_stack_t *stack, const lang_spec_t *ls)
{
    if (!f) return;
    fprintf(f, OW_FMT_REDUCE,
            rule_num, pos, prev_state, new_state, rule_desc);
    ps_print(stack, ls, f);
    fprintf(f, OW_LINE_END);
    fflush(f);
}

/* Write an ACCEPT step. */
void ow_write_accept(FILE *f, int pos, int state,
                     const parser_stack_t *stack, const lang_spec_t *ls)
{
    if (!f) return;
    fprintf(f, OW_FMT_ACCEPT, pos, state);
    ps_print(stack, ls, f);
    fprintf(f, OW_LINE_END);
    fflush(f);
}

/* Write an ERROR step. */
void ow_write_error(FILE *f, int pos, int state, const char *term_name)
{
    if (!f) return;
    fprintf(f, OW_FMT_ERROR, pos, state, term_name);
    fflush(f);
}

/* Flush and close the debug file. */
void ow_close_dbg(FILE *f)
{
    if (f) { fflush(f); fclose(f); }
}
