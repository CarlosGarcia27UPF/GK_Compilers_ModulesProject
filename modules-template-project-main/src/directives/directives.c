/*
 * -----------------------------------------------------------------------------
 * directives.c
 *
 * Module: directives - Directive detection and execution
 * Responsible for: #include, #define, #ifdef/#endif processing
 *
 * Author: Carlos García 
 * -----------------------------------------------------------------------------
 */

#include "directives.h"
#include "errors/errors.h"
#include "io/io.h"
#include "comments/comments.h"
#include "tokens/tokens.h"

#include <string.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define PP_MAX_PATH_LEN 4096

/* Local helpers to align with macros API in macros.c */
static int macros_define(macro_table_t *table, const char *name, const char *value)
{
    if (!table || !name || !value) return 1;
    return macros_add(table, name, (int)strlen(name), value, (int)strlen(value));
}

static int macros_is_defined(const macro_table_t *table, const char *name)
{
    if (!table || !name) return 0;
    return macros_get(table, name, (int)strlen(name)) != NULL;
}

void ifdef_stack_init(ifdef_stack_t *stack) {
    stack->top = -1;
}

int ifdef_should_include(const ifdef_stack_t *stack) {
    /* If stack is empty, include everything */
    if (stack->top < 0) return 1;
    
    /* Check if all levels say to include */
    for (int i = 0; i <= stack->top; i++) {
        if (!stack->stack[i]) return 0;
    }
    return 1;
}

/* Skip leading whitespace */
static const char* skip_whitespace(const char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

static int token_is_symbol(const Token *t, char ch)
{
    return t && t->type == SYMBOL && t->length == 1 && t->word && t->word[0] == ch;
}

static int token_is_ident(const Token *t, const char *kw)
{
    if (!t || t->type != IDENTIFIER || !t->word || !kw) return 0;
    size_t kw_len = strlen(kw);
    if ((size_t)t->length != kw_len) return 0;
    return strncmp(t->word, kw, kw_len) == 0;
}

int directives_process_line(const char *line, long line_len, 
                           const char *base_dir,
                           const char *current_file, int line_num,
                           macro_table_t *macros, 
                           ifdef_stack_t *ifdef_stack,
                           int do_comments,
                           comment_state_t *comment_state,
                           buffer_t *output) {
    if (!line || !macros || !ifdef_stack) return 1;
    
    const char *hash = skip_whitespace(line);
    
    /* Must start with '#' */
    if (*hash != '#') return 1;

    /* Tokenize starting at '#' for robust parsing */
    Tokenizer tk;
    Token tok;
    tokens_init(&tk, line_num, (char *)hash);

    if (!tokenize(&tk, &tok) || !token_is_symbol(&tok, '#')) {
        /* Not a directive we understand; keep behavior consistent */
        buffer_append_n(output, line, line_len);
        return 0;
    }

    if (!tokenize(&tk, &tok) || tok.type != IDENTIFIER) {
        /* Something like "#\n" - keep it */
        buffer_append_n(output, line, line_len);
        return 0;
    }
    
    /* Handle #include */
    if (token_is_ident(&tok, "include")) {
        /* Only process if we're in an active #ifdef context */
        if (!ifdef_should_include(ifdef_stack)) {
            return 2;  /* Skip this directive */
        }

        char filename[256];

        Token arg;
        if (!tokenize(&tk, &arg) || arg.type != STRING) {
            error(line_num, "%s: Invalid #include syntax", current_file);
            return 1;
        }

        /* Per P1PP handout: #include <...> is not required; leave it unchanged */
        if (arg.type != STRING) {
            /* <...> form not supported → leave unchanged */
            buffer_append_n(output, line, line_len);
            return 0;
        }

        if (arg.length <= 0 || arg.length >= (int)sizeof(filename)) {
            error(line_num, "%s: Include path too long", current_file);
            return 1;
        }
        memcpy(filename, arg.word, (size_t)arg.length);
        filename[arg.length] = '\0';
        
        /* Build full path with bounds checking */
        char full_path[PP_MAX_PATH_LEN];
        int path_len;
        if (base_dir && base_dir[0]) {
            path_len = snprintf(full_path, sizeof(full_path), "%s/%s", base_dir, filename);
        } else {
            path_len = snprintf(full_path, sizeof(full_path), "%s", filename);
        }
        
        if (path_len < 0 || path_len >= (int)sizeof(full_path)) {
            error(line_num, "%s: Include path too long: %s", current_file, filename);
            return 1;
        }
        
        /* Read the included file */
        buffer_t included;
        buffer_init(&included);
        
        if (io_read_file(full_path, &included) != 0) {
            error(line_num, "%s: Cannot open included file: %s", current_file, full_path);
            buffer_free(&included);
            return 1;
        }

        /* Base dir for nested includes inside this included file */
        char included_base_dir[PP_MAX_PATH_LEN];
        {
            const char *slash = strrchr(full_path, '/');
            if (!slash) {
                snprintf(included_base_dir, sizeof(included_base_dir), ".");
            } else if (slash == full_path) {
                snprintf(included_base_dir, sizeof(included_base_dir), "/");
            } else {
                size_t n = (size_t)(slash - full_path);
                if (n >= sizeof(included_base_dir)) n = sizeof(included_base_dir) - 1;
                memcpy(included_base_dir, full_path, n);
                included_base_dir[n] = '\0';
            }
        }
        
        /* Process the included file recursively */
        comment_state_t local_comment_state;
        if (!comment_state) {
            comments_state_init(&local_comment_state);
            comment_state = &local_comment_state;
        }
        
        buffer_t processed;
        buffer_init(&processed);
        
        /* Process included file line by line */
        long i = 0, line_start = 0;
        int included_line_num = 1;
        while (i < included.len) {
            if (included.data[i] == '\n' || i == included.len - 1) {
                long len = (i - line_start) + 1;
                if (i == included.len - 1 && included.data[i] != '\n') {
                    len = included.len - line_start;
                }
                
                buffer_t line_buf;
                buffer_init(&line_buf);

                const char *raw_line = included.data + line_start;

                int start_in_block_comment = comment_state->in_block_comment;

                /* Apply comment removal only if enabled; otherwise preserve comments */
                if (do_comments) {
                    comments_process_line(raw_line, len, &line_buf, comment_state);
                } else {
                    buffer_append_n(&line_buf, raw_line, len);
                }
                /* Check if it's a directive and process recursively */
                const char *lp = skip_whitespace(line_buf.data);
                if (!start_in_block_comment && *lp == '#') {
                    directives_process_line(line_buf.data, line_buf.len, included_base_dir, full_path, 
                                          included_line_num, macros, ifdef_stack,
                                          do_comments, comment_state, &processed);

                        if (!do_comments) {
                            comments_update_state(raw_line, len, comment_state);
                        }
                } else {
                    /* Expand macros if not inside false ifdef */
                    if (ifdef_should_include(ifdef_stack)) {
                        buffer_t expanded;
                        buffer_init(&expanded);

                        macros_expand_line(macros, line_buf.data, line_buf.len, &expanded);
                        buffer_append_n(&processed, expanded.data, expanded.len);
                        buffer_free(&expanded);
                    } else {
                        /* Not including this branch, but still keep comments state consistent */
                            if (!do_comments) {
                                comments_update_state(raw_line, len, comment_state);
                            }
                    }
                }
                
                buffer_free(&line_buf);
                
                i++;
                line_start = i;
                included_line_num++;
            } else {
                i++;
            }
        }
        
        /* Append processed included content to output */
        buffer_append_n(output, processed.data, processed.len);
        
        buffer_free(&processed);
        buffer_free(&included);
        
        return 0;  /* Directive processed */
    }
    
    /* Handle #define */
    if (token_is_ident(&tok, "define")) {
        /* Only process if we're in an active #ifdef context */
        if (!ifdef_should_include(ifdef_stack)) {
            return 2;  /* Skip this directive */
        }

        char name[128];
        char value[512];

        Token name_tok;
        if (!tokenize(&tk, &name_tok) || name_tok.type != IDENTIFIER) {
            error(line_num, "%s: Invalid #define syntax", current_file);
            return 1;
        }
        if (name_tok.length <= 0 || name_tok.length >= (int)sizeof(name)) {
            error(line_num, "%s: Invalid #define syntax", current_file);
            return 1;
        }
        memcpy(name, name_tok.word, (size_t)name_tok.length);
        name[name_tok.length] = '\0';

        /* Value is the remaining text on the line (trimmed) */
        const char *valp = (const char *)((const char *)tk.full_line + tk.position);
        valp = skip_whitespace(valp);

        const char *value_start = valp;
        const char *value_end = value_start + strlen(value_start);
        while (value_end > value_start && isspace((unsigned char)*(value_end - 1))) {
            value_end--;
        }

        int vlen = (int)(value_end - value_start);
        if (vlen < 0) vlen = 0;
        if (vlen >= (int)sizeof(value)) vlen = (int)sizeof(value) - 1;

        memcpy(value, value_start, (size_t)vlen);
        value[vlen] = '\0';
        
        /* Add to macro table */
        macros_define(macros, name, value);
        
        return 0;  /* Directive processed, don't output it */
    }
    
    /* Handle #ifdef */
    if (token_is_ident(&tok, "ifdef")) {
        char name[128];

        Token name_tok;
        if (!tokenize(&tk, &name_tok) || name_tok.type != IDENTIFIER ||
            name_tok.length <= 0 || name_tok.length >= (int)sizeof(name)) {
            /* Not the supported form '#ifdef IDENTIFIER' -> keep unchanged (or skip if inactive) */
            if (!ifdef_should_include(ifdef_stack)) return 2;
            buffer_append_n(output, line, line_len);
            return 0;
        }

        /* Reject trailing tokens: only '#ifdef IDENTIFIER' is supported */
        Token extra;
        if (tokenize(&tk, &extra)) {
            if (!ifdef_should_include(ifdef_stack)) return 2;
            buffer_append_n(output, line, line_len);
            return 0;
        }
        memcpy(name, name_tok.word, (size_t)name_tok.length);
        name[name_tok.length] = '\0';
        
        /* Push onto stack */
        if (ifdef_stack->top >= PP_MAX_IF_DEPTH - 1) {
            error(line_num, "%s: #ifdef nesting too deep", current_file);
            return 1;
        }
        
        /* Only check if macro is defined if parent context is active */
        int should_include = ifdef_should_include(ifdef_stack) && macros_is_defined(macros, name);
        
        ifdef_stack->top++;
        ifdef_stack->stack[ifdef_stack->top] = should_include;
        
        return 0;  /* Directive processed */
    }
    
    /* Handle #endif */
    if (token_is_ident(&tok, "endif")) {
        /* Reject trailing tokens: only '#endif' is supported */
        Token extra;
        if (tokenize(&tk, &extra)) {
            if (!ifdef_should_include(ifdef_stack)) return 2;
            buffer_append_n(output, line, line_len);
            return 0;
        }

        if (ifdef_stack->top >= 0) {
            ifdef_stack->top--;
            return 0;  /* Directive processed */
        }

        /* Otherwise, this #endif belongs to an unsupported directive - keep it */
        if (!ifdef_should_include(ifdef_stack)) return 2;
        buffer_append_n(output, line, line_len);
        return 0;
    }
    
    /* Unknown directive - keep it in output */
    if (!ifdef_should_include(ifdef_stack)) {
        return 2;
    }
    buffer_append_n(output, line, line_len);
    return 0;
}