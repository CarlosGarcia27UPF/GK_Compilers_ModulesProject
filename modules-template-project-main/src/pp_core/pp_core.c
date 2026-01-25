/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module provides the core preprocessing engine for lines and files.
 *
 * - `pp_run`: Executes preprocessing (comments, directives, macros) over input.
 * - `process_line`: Applies comment handling, directives, and macro expansion.
 * - `handle_directive_line`: Executes #include/#define/#ifdef handling.
 * - `handle_non_directive_line`: Handles macro expansion or raw output.
 * - `build_line_buffer`: Builds a line buffer with/without comment removal.
 *
 * Usage:
 *     Called by the main application after input is loaded into a buffer.
 *
 * Status:
 *     Active - core engine used by the preprocessor CLI.
 * -------------------------------------------------------------------------- */

#include "pp_core.h"
#include "comments/comments.h"
#include "directives/directives.h"
#include "macros/macros.h"
#include "errors/errors.h"
#include <ctype.h>

/* Check if line starts with a directive marker (first non-space is '#'). */
static int is_directive_line(const char *line, long line_len)
{
    for (long i = 0; i < line_len; i++) {
        if (line[i] == PP_CHAR_HASH) return 1;
        if (!isspace((unsigned char)line[i])) return 0;
    }
    return 0;
}

/* Append to a buffer and report out-of-memory errors. */
static int append_or_report(pp_context_t *ctx, buffer_t *dst, const char *data, long len, int err_code)
{
    if (buffer_append_n(dst, data, len) != 0) {
        error(ctx->current_line, "%s: %s", ctx->current_file, PP_ERR_OUT_OF_MEMORY);
        return err_code;
    }
    return PP_RUN_SUCCESS;
}

/* Build the current line buffer with or without comment removal. */
static int build_line_buffer(pp_context_t *ctx,
                             const char *line_data,
                             long line_len,
                             buffer_t *line_buf,
                             int err_code)
{
    if (ctx->opt.do_comments) {
        if (comments_process_line(line_data, line_len, line_buf, &ctx->comment_state) != 0) {
            error(ctx->current_line, "%s: %s", ctx->current_file, PP_ERR_COMMENTS_PROCESS);
            return err_code;
        }
        return PP_RUN_SUCCESS;
    }

    return append_or_report(ctx, line_buf, line_data, line_len, err_code);
}

/* Handle a directive line (if enabled) and append directive output if needed. */
static int handle_directive_line(pp_context_t *ctx,
                                 const buffer_t *line_buf,
                                 const char *line_data,
                                 long line_len,
                                 buffer_t *output,
                                 const char *base_dir,
                                 int start_in_block_comment,
                                 int err_code,
                                 int *handled)
{
    *handled = 0;
    if (!ctx->opt.do_directives || start_in_block_comment ||
        !is_directive_line(line_buf->data, line_buf->len)) {
        return PP_RUN_SUCCESS;
    }

    *handled = 1;
    buffer_t directive_output;
    buffer_init(&directive_output);

    int result = directives_process_line(line_buf->data, line_buf->len,
                                         base_dir,
                                         ctx->current_file, ctx->current_line,
                                         &ctx->macros, &ctx->ifdef_stack,
                                         ctx->opt.do_comments,
                                         &ctx->comment_state,
                                         &directive_output);

    if (result == 0 && directive_output.len > 0) {
        if (append_or_report(ctx, output, directive_output.data,
                             directive_output.len, err_code) != PP_RUN_SUCCESS) {
            buffer_free(&directive_output);
            return err_code;
        }
    }

    buffer_free(&directive_output);

    if (!ctx->opt.do_comments) {
        comments_update_state(line_data, line_len, &ctx->comment_state);
    }

    return PP_RUN_SUCCESS;
}

/* Handle macro expansion or raw output for non-directive lines. */
static int handle_non_directive_line(pp_context_t *ctx,
                                     const buffer_t *line_buf,
                                     const char *line_data,
                                     long line_len,
                                     buffer_t *output,
                                     int err_code)
{
    if (ctx->opt.do_directives && ifdef_should_include(&ctx->ifdef_stack)) {
        buffer_t expanded;
        buffer_init(&expanded);

        if (macros_expand_line(&ctx->macros, line_buf->data, line_buf->len, &expanded) != 0) {
            error(ctx->current_line, "%s: %s", ctx->current_file, PP_ERR_MACRO_EXPANSION);
            buffer_free(&expanded);
            return err_code;
        }

        if (append_or_report(ctx, output, expanded.data, expanded.len, err_code) != PP_RUN_SUCCESS) {
            buffer_free(&expanded);
            return err_code;
        }
        buffer_free(&expanded);
    } else if (!ctx->opt.do_directives || ifdef_should_include(&ctx->ifdef_stack)) {
        if (append_or_report(ctx, output, line_buf->data, line_buf->len, err_code) != PP_RUN_SUCCESS) {
            return err_code;
        }
    }

    if (ctx->opt.do_directives && !ifdef_should_include(&ctx->ifdef_stack) && !ctx->opt.do_comments) {
        comments_update_state(line_data, line_len, &ctx->comment_state);
    }

    return PP_RUN_SUCCESS;
}

/* Process a single logical line of input according to current options. */
static int process_line(pp_context_t *ctx,
                        const char *line_data,
                        long line_len,
                        buffer_t *output,
                        const char *base_dir,
                        int err_code)
{
    buffer_t line_buf;
    buffer_init(&line_buf);

    int start_in_block_comment = ctx->comment_state.in_block_comment;
    int rc = build_line_buffer(ctx, line_data, line_len, &line_buf, err_code);
    if (rc != PP_RUN_SUCCESS) {
        buffer_free(&line_buf);
        return rc;
    }

    int handled = 0;
    rc = handle_directive_line(ctx, &line_buf, line_data, line_len,
                               output, base_dir, start_in_block_comment, err_code, &handled);
    if (rc != PP_RUN_SUCCESS) {
        buffer_free(&line_buf);
        return rc;
    }

    if (!handled) {
        rc = handle_non_directive_line(ctx, &line_buf, line_data, line_len, output, err_code);
        if (rc != PP_RUN_SUCCESS) {
            buffer_free(&line_buf);
            return rc;
        }
    }

    buffer_free(&line_buf);
    return PP_RUN_SUCCESS;
}

/* Run preprocessing over the input buffer and write results to output. */
int pp_run(pp_context_t *ctx, const buffer_t *input, buffer_t *output, const char *base_dir)
{
    long i = 0;
    long line_start = 0;

    if (!ctx || !input || !output || !input->data) {
        return PP_RUN_ERR_INVALID_ARGS;
    }

    /* Initialize state. */
    comments_state_init(&ctx->comment_state);
    macros_init(&ctx->macros);
    ifdef_stack_init(&ctx->ifdef_stack);

    ctx->current_line = 0;

    while (i < input->len) {
        if (input->data[i] == PP_CHAR_NL) {
            long line_len = (i - line_start) + 1;
            ctx->current_line++;

            const char *line_data = input->data + line_start;
            int rc = process_line(ctx, line_data, line_len, output, base_dir, PP_RUN_ERR_PROCESSING);
            if (rc != PP_RUN_SUCCESS) {
                return rc;
            }

            i++;
            line_start = i;
        } else {
            i++;
        }
    }

    if (line_start < input->len) {
        long line_len = input->len - line_start;
        ctx->current_line++;

        const char *line_data = input->data + line_start;
        int rc = process_line(ctx, line_data, line_len, output, base_dir,
                              PP_RUN_ERR_PROCESSING_LAST_LINE);
        if (rc != PP_RUN_SUCCESS) {
            return rc;
        }
    }

    macros_free(&ctx->macros);
    return PP_RUN_SUCCESS;
}
