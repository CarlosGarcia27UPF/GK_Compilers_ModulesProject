#include "pp_core.h"
#include "comments/comments.h"
#include "directives/directives.h"
#include "macros/macros.h"
#include <ctype.h>

/* Check if line starts with # (is a directive) */
static int is_directive_line(const char *line, long line_len) {
    for (long i = 0; i < line_len; i++) {
        if (line[i] == '#') return 1;
        if (!isspace((unsigned char)line[i])) return 0;
    }
    return 0;
}

/* Returns 0 on success, non-zero on error */
int pp_run(pp_context_t *ctx, const buffer_t *input, buffer_t *output, const char *base_dir)
{
    long i = 0;
    long line_start = 0;

    if (!ctx || !input || !output || !input->data) {
        return 1;
    }

    /* Initialize state */
    comments_state_init(&ctx->comment_state);
    macros_table_init(&ctx->macros);
    ifdef_stack_init(&ctx->ifdef_stack);

    /* We count lines the same way humans do:
       - each '\n' ends a line
       - last line may or may not have '\n'
    */
    ctx->current_line = 0;

    while (i < input->len) {
        if (input->data[i] == '\n') {
            long line_len = (i - line_start) + 1;  /* include '\n' */
            ctx->current_line++;

            /* Process line based on options */
            buffer_t line_buf;
            buffer_init(&line_buf);
            
            const char *line_data = input->data + line_start;
            
            /* Step 1: Remove comments if enabled */
            if (ctx->opt.do_comments) {
                if (comments_process_line(line_data, line_len, &line_buf, &ctx->comment_state) != 0) {
                    buffer_free(&line_buf);
                    ctx->error_count++;
                    return 2;
                }
            } else {
                /* No comment removal - copy as is */
                if (buffer_append_n(&line_buf, line_data, line_len) != 0) {
                    buffer_free(&line_buf);
                    ctx->error_count++;
                    return 2;
                }
            }
            
            /* Step 2: Process directives if enabled */
            if (ctx->opt.do_directives && is_directive_line(line_buf.data, line_buf.len)) {
                buffer_t directive_output;
                buffer_init(&directive_output);
                
                int result = directives_process_line(line_buf.data, line_buf.len,
                                                    base_dir,
                                                    ctx->current_file, ctx->current_line,
                                                    &ctx->macros, &ctx->ifdef_stack,
                                                    &directive_output);
                
                if (result == 1) {
                    /* Error processing directive */
                    ctx->error_count++;
                } else if (result == 0) {
                    /* Directive processed successfully - append output (if any) */
                    if (directive_output.len > 0) {
                        buffer_append_n(output, directive_output.data, directive_output.len);
                    }
                }
                /* result == 2 means skip (inside false #ifdef) */
                
                buffer_free(&directive_output);
                buffer_free(&line_buf);
            } else {
                /* Step 3: Expand macros if enabled (for non-directive lines) */
                if (ctx->opt.do_directives && ifdef_should_include(&ctx->ifdef_stack)) {
                    buffer_t expanded;
                    buffer_init(&expanded);
                    
                    if (macros_expand_line(&ctx->macros, line_buf.data, line_buf.len, &expanded) != 0) {
                        buffer_free(&expanded);
                        buffer_free(&line_buf);
                        ctx->error_count++;
                        return 2;
                    }
                    
                    buffer_append_n(output, expanded.data, expanded.len);
                    buffer_free(&expanded);
                } else if (!ctx->opt.do_directives || ifdef_should_include(&ctx->ifdef_stack)) {
                    /* No directive processing or inside active ifdef - just output */
                    buffer_append_n(output, line_buf.data, line_buf.len);
                } 
                /* else: inside false ifdef and directives enabled - skip line */
                
                buffer_free(&line_buf);
            }

            i++;
            line_start = i;
        } else {
            i++;
        }
    }

    /* Handle last line if file doesn't end with '\n' */
    if (line_start < input->len) {
        long line_len = input->len - line_start;
        ctx->current_line++;

        buffer_t line_buf;
        buffer_init(&line_buf);
        
        const char *line_data = input->data + line_start;
        
        /* Step 1: Remove comments if enabled */
        if (ctx->opt.do_comments) {
            if (comments_process_line(line_data, line_len, &line_buf, &ctx->comment_state) != 0) {
                buffer_free(&line_buf);
                ctx->error_count++;
                return 3;
            }
        } else {
            /* No comment removal - copy as is */
            if (buffer_append_n(&line_buf, line_data, line_len) != 0) {
                buffer_free(&line_buf);
                ctx->error_count++;
                return 3;
            }
        }
        
        /* Step 2: Process directives if enabled */
        if (ctx->opt.do_directives && is_directive_line(line_buf.data, line_buf.len)) {
            buffer_t directive_output;
            buffer_init(&directive_output);
            
            int result = directives_process_line(line_buf.data, line_buf.len,
                                                base_dir,
                                                ctx->current_file, ctx->current_line,
                                                &ctx->macros, &ctx->ifdef_stack,
                                                &directive_output);
            
            if (result == 1) {
                ctx->error_count++;
            } else if (result == 0) {
                if (directive_output.len > 0) {
                    buffer_append_n(output, directive_output.data, directive_output.len);
                }
            }
            
            buffer_free(&directive_output);
            buffer_free(&line_buf);
        } else {
            /* Step 3: Expand macros if enabled (for non-directive lines) */
            if (ctx->opt.do_directives && ifdef_should_include(&ctx->ifdef_stack)) {
                buffer_t expanded;
                buffer_init(&expanded);
                
                if (macros_expand_line(&ctx->macros, line_buf.data, line_buf.len, &expanded) != 0) {
                    buffer_free(&expanded);
                    buffer_free(&line_buf);
                    ctx->error_count++;
                    return 3;
                }
                
                buffer_append_n(output, expanded.data, expanded.len);
                buffer_free(&expanded);
            } else if (!ctx->opt.do_directives || ifdef_should_include(&ctx->ifdef_stack)) {
                buffer_append_n(output, line_buf.data, line_buf.len);
            }
            
            buffer_free(&line_buf);
        }
    }
    
    /* Clean up */
    macros_table_free(&ctx->macros);

    return 0;
}
