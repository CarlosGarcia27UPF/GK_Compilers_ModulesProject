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
#include "io/io.h"
#include <stdio.h>
#include <ctype.h>

// Check if line starts with a directive marker (first non-space is '#').
static int is_directive_line(const char *line, long line_len)
{
    // Scan through the line character by character
    for (long i = 0; i < line_len; i++) {
        // If we find a '#' before any non-whitespace, it's a directive
        if (line[i] == PP_CHAR_HASH) return 1;
        // If we hit a non-whitespace character that isn't '#', it's not a directive
        if (!isspace((unsigned char)line[i])) return 0;
    }
    // Empty or whitespace-only line is not a directive
    return 0;
}

// Process a full buffer with the current context state (no re-init).
static int pp_process_buffer(pp_context_t *ctx,
                             const buffer_t *input,
                             buffer_t *output,
                             const char *base_dir,
                             int err_code,
                             int err_code_last);

// Append to a buffer and report out-of-memory errors.
static int append_or_report(pp_context_t *ctx, buffer_t *dst, const char *data, long len, int err_code)
{
    // Try to append the data to the buffer
    if (buffer_append_n(dst, data, len) != 0) {
        // If we run out of memory, log the error with file and line info
        error(ctx->current_line, "%s: %s", ctx->current_file, PP_ERR_OUT_OF_MEMORY);
        return err_code;
    }
    // Successful append
    return PP_RUN_SUCCESS;
}

// Build the current line buffer with or without comment removal.
static int build_line_buffer(pp_context_t *ctx,
                             const char *line_data,
                             long line_len,
                             buffer_t *line_buf,
                             int err_code)
{
    // If comment processing is enabled, strip out comments from this line
    if (ctx->opt.do_comments) {
        // Call the comment processor to remove C-style and C++ comments
        if (comments_process_line(line_data, line_len, line_buf, &ctx->comment_state) != 0) {
            // Report error if comment processing fails
            error(ctx->current_line, "%s: %s", ctx->current_file, PP_ERR_COMMENTS_PROCESS);
            return err_code;
        }
        return PP_RUN_SUCCESS;
    }

    // If comment processing is disabled, just copy the line as-is
    return append_or_report(ctx, line_buf, line_data, line_len, err_code);
}

// Handle a directive line (if enabled) and append directive output if needed.
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
    // Initially mark as not handled
    *handled = 0;
    // Skip directive processing if: directives are disabled, we're in a block comment,
    // or this line doesn't start with '#'
    if (!ctx->opt.do_directives || start_in_block_comment ||
        !is_directive_line(line_buf->data, line_buf->len)) {
        return PP_RUN_SUCCESS;
    }

    // We have a directive to process, mark it as handled
    *handled = 1;
    buffer_t directive_output;
    buffer_init(&directive_output);
    buffer_t include_name;
    buffer_init(&include_name);

    // Parse and execute the directive (#include, #define, #ifdef, etc.)
    int result = directives_process_line(line_buf->data, line_buf->len,
                                         base_dir,
                                         ctx->current_file, ctx->current_line,
                                         &ctx->macros, &ctx->ifdef_stack,
                                         ctx->opt.do_comments,
                                         &ctx->comment_state,
                                         &directive_output,
                                         &include_name);

    // If this is an #include directive, we need to recursively process the included file
    if (result == DIR_INCLUDE && include_name.len > 0) {
        // Build the full path to the included file
        char full_path[PP_MAX_PATH_LEN];
        int path_len;
        // If we have a base directory, prepend it to the include filename
        if (base_dir && base_dir[0]) {
            path_len = snprintf(full_path, sizeof(full_path), "%s/%s", base_dir, include_name.data);
        } else {
            // Otherwise, use the include name as-is
            path_len = snprintf(full_path, sizeof(full_path), "%s", include_name.data);
        }
        // Check if the path is too long for our buffer
        if (path_len < 0 || path_len >= (int)sizeof(full_path)) {
            error(ctx->current_line, "%s: Include path too long: %s", ctx->current_file, include_name.data);
            buffer_free(&include_name);
            buffer_free(&directive_output);
            return err_code;
        }

        // Read the contents of the included file into a buffer
        buffer_t included;
        buffer_init(&included);
        if (io_read_file(full_path, &included) != 0) {
            // If the file can't be opened, report an error
            error(ctx->current_line, "%s: Cannot open included file: %s", ctx->current_file, full_path);
            buffer_free(&included);
            buffer_free(&include_name);
            buffer_free(&directive_output);
            return err_code;
        }

        // Extract the directory part of the included file's path for nested includes
        char included_base_dir[PP_MAX_PATH_LEN];
        io_compute_base_dir(full_path, included_base_dir, sizeof(included_base_dir));
        // Recursively preprocess the included file with the same context
        int rc = pp_process_buffer(ctx, &included, output, included_base_dir, err_code, err_code);
        buffer_free(&included);
        // Check if processing the included file failed
        if (rc != PP_RUN_SUCCESS) {
            buffer_free(&include_name);
            buffer_free(&directive_output);
            return rc;
        }
    } else if (result == DIR_OK && directive_output.len > 0) {
        // For other directives (like #define output), append their result to the output
        if (append_or_report(ctx, output, directive_output.data,
                             directive_output.len, err_code) != PP_RUN_SUCCESS) {
            buffer_free(&include_name);
            buffer_free(&directive_output);
            return err_code;
        }
    }

    buffer_free(&include_name);
    buffer_free(&directive_output);

    // If comment removal is disabled, we still need to track comment state
    // (e.g., whether we're inside a block comment) for correct directive processing
    if (!ctx->opt.do_comments) {
        comments_update_state(line_data, line_len, &ctx->comment_state);
    }

    return PP_RUN_SUCCESS;
}

// Handle macro expansion or raw output for non-directive lines.
static int handle_non_directive_line(pp_context_t *ctx,
                                     const buffer_t *line_buf,
                                     const char *line_data,
                                     long line_len,
                                     buffer_t *output,
                                     int err_code)
{
    // If directives are enabled and we're not in a skipped #ifdef block, expand macros
    if (ctx->opt.do_directives && ifdef_should_include(&ctx->ifdef_stack)) {
        // Create a buffer to hold the macro-expanded version of the line
        buffer_t expanded;
        buffer_init(&expanded);

        // Replace all macro invocations with their defined values
        if (macros_expand_line(&ctx->macros, line_buf->data, line_buf->len, &expanded) != 0) {
            error(ctx->current_line, "%s: %s", ctx->current_file, PP_ERR_MACRO_EXPANSION);
            buffer_free(&expanded);
            return err_code;
        }

        // Append the expanded line to the output
        if (append_or_report(ctx, output, expanded.data, expanded.len, err_code) != PP_RUN_SUCCESS) {
            buffer_free(&expanded);
            return err_code;
        }
        buffer_free(&expanded);
    } else if (!ctx->opt.do_directives || ifdef_should_include(&ctx->ifdef_stack)) {
        // No macro expansion needed - just output the line as processed
        if (append_or_report(ctx, output, line_buf->data, line_buf->len, err_code) != PP_RUN_SUCCESS) {
            return err_code;
        }
    }

    // If we're in a skipped #ifdef block and not removing comments,
    // we still need to track comment state for proper directive processing
    if (ctx->opt.do_directives && !ifdef_should_include(&ctx->ifdef_stack) && !ctx->opt.do_comments) {
        comments_update_state(line_data, line_len, &ctx->comment_state);
    }

    return PP_RUN_SUCCESS;
}

// Process a single logical line of input according to current options.
static int process_line(pp_context_t *ctx,
                        const char *line_data,
                        long line_len,
                        buffer_t *output,
                        const char *base_dir,
                        int err_code)
{
    // Create a temporary buffer for this line
    buffer_t line_buf;
    buffer_init(&line_buf);

    // Remember if we started this line inside a block comment
    int start_in_block_comment = ctx->comment_state.in_block_comment;
    // Build the line buffer, potentially removing comments
    int rc = build_line_buffer(ctx, line_data, line_len, &line_buf, err_code);
    if (rc != PP_RUN_SUCCESS) {
        buffer_free(&line_buf);
        return rc;
    }

    // Try to handle this line as a preprocessor directive
    int handled = 0;
    rc = handle_directive_line(ctx, &line_buf, line_data, line_len,
                               output, base_dir, start_in_block_comment, err_code, &handled);
    if (rc != PP_RUN_SUCCESS) {
        buffer_free(&line_buf);
        return rc;
    }

    // If it wasn't a directive, handle it as a regular code line (with potential macro expansion)
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

// Process a full buffer with current context state (no re-initialization).
static int pp_process_buffer(pp_context_t *ctx,
                             const buffer_t *input,
                             buffer_t *output,
                             const char *base_dir,
                             int err_code,
                             int err_code_last)
{
    // Track our position in the input buffer
    long i = 0;
    long line_start = 0;

    // Process the buffer line by line, looking for newline characters
    while (i < input->len) {
        // Found a newline - process this complete line
        if (input->data[i] == PP_CHAR_NL) {
            // Calculate the length of this line (including the newline)
            long line_len = (i - line_start) + 1;
            // Increment line counter for error reporting
            ctx->current_line++;

            // Get a pointer to the start of this line
            const char *line_data = input->data + line_start;
            // Process this line (comments, directives, macros)
            int rc = process_line(ctx, line_data, line_len, output, base_dir, err_code);
            if (rc != PP_RUN_SUCCESS) {
                return rc;
            }

            // Move past the newline and mark the start of the next line
            i++;
            line_start = i;
        } else {
            // Not a newline, continue scanning
            i++;
        }
    }

    // Handle the last line if it doesn't end with a newline
    if (line_start < input->len) {
        // Calculate length of the final line (no trailing newline)
        long line_len = input->len - line_start;
        ctx->current_line++;

        // Process the last line with a different error code
        const char *line_data = input->data + line_start;
        int rc = process_line(ctx, line_data, line_len, output, base_dir, err_code_last);
        if (rc != PP_RUN_SUCCESS) {
            return rc;
        }
    }

    return PP_RUN_SUCCESS;
}

// Run preprocessing over the input buffer and write results to output.
int pp_run(pp_context_t *ctx, const buffer_t *input, buffer_t *output, const char *base_dir)
{
    // Validate all required parameters
    if (!ctx || !input || !output || !input->data) {
        return PP_RUN_ERR_INVALID_ARGS;
    }

    // Initialize the preprocessing state: comment tracking, macro table, and #ifdef stack
    comments_state_init(&ctx->comment_state);
    macros_init(&ctx->macros);
    ifdef_stack_init(&ctx->ifdef_stack);

    // Start at line 0 (will be incremented to 1 when processing first line)
    ctx->current_line = 0;

    // Process the entire input buffer, applying all preprocessing steps
    int rc = pp_process_buffer(ctx, input, output, base_dir,
                               PP_RUN_ERR_PROCESSING,
                               PP_RUN_ERR_PROCESSING_LAST_LINE);
    if (rc != PP_RUN_SUCCESS) {
        return rc;
    }

    // Clean up the macro table before returning
    macros_free(&ctx->macros);
    return PP_RUN_SUCCESS;
}
