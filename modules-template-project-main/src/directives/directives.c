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

/* Trim value at start of comment markers (// or block-comment) outside strings/chars. */
static const char *trim_define_value_end(const char *start, const char *end)
{
    int in_string = 0;
    int in_char = 0;
    int escaped = 0;

    for (const char *p = start; p < end; p++) {
        char c = *p;
        if (in_string) {
            if (escaped) {
                escaped = 0;
            } else if (c == '\\') {
                escaped = 1;
            } else if (c == '"') {
                in_string = 0;
            }
            continue;
        }
        if (in_char) {
            if (escaped) {
                escaped = 0;
            } else if (c == '\\') {
                escaped = 1;
            } else if (c == '\'') {
                in_char = 0;
            }
            continue;
        }

        if (c == '"') {
            in_string = 1;
            continue;
        }
        if (c == '\'') {
            in_char = 1;
            continue;
        }
        if (c == '/' && (p + 1) < end) {
            if (p[1] == '/' || p[1] == '*') {
                return p;
            }
        }
    }

    return end;
}

int directives_process_line(const char *line, long line_len, 
                           const char *base_dir,
                           const char *current_file, int line_num,
                           macro_table_t *macros, 
                           ifdef_stack_t *ifdef_stack,
                           int do_comments,
                           comment_state_t *comment_state,
                           buffer_t *output,
                           buffer_t *include_name) {
    if (!line || !macros || !ifdef_stack) return 1;
    (void)base_dir;
    (void)do_comments;
    (void)comment_state;
    
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
            return DIR_SKIP;  /* Skip this directive */
        }

        char filename[PP_MAX_INCLUDE_NAME];

        Token arg;
        if (!tokenize(&tk, &arg)) {
            error(line_num, "%s: Invalid #include syntax", current_file);
            return DIR_ERROR;
        }

        /* Per P1PP handout: #include <...> is not required; leave it unchanged */
        if (arg.type != STRING) {
            buffer_append_n(output, line, line_len);
            return DIR_OK;
        }

        int name_len = arg.length;
        const char *name_start = arg.word;
        if (name_len >= 2 && arg.word[0] == '"' && arg.word[name_len - 1] == '"') {
            name_start = arg.word + 1;
            name_len -= 2;
        }

        if (name_len <= 0 || name_len >= (int)sizeof(filename)) {
            error(line_num, "%s: Include path too long", current_file);
            return DIR_ERROR;
        }
        memcpy(filename, name_start, (size_t)name_len);
        filename[name_len] = '\0';

        if (include_name) {
            buffer_append_str(include_name, filename);
        }

        return DIR_INCLUDE;  /* Include directive handled by caller */
    }
    
    /* Handle #define */
    if (token_is_ident(&tok, "define")) {
        /* Only process if we're in an active #ifdef context */
        if (!ifdef_should_include(ifdef_stack)) {
            return DIR_SKIP;  /* Skip this directive */
        }

        char name[PP_MAX_DEFINE_NAME];
        char value[PP_MAX_DEFINE_VALUE];

        Token name_tok;
        if (!tokenize(&tk, &name_tok) || name_tok.type != IDENTIFIER) {
            error(line_num, "%s: Invalid #define syntax", current_file);
            return DIR_ERROR;
        }
        if (name_tok.length <= 0 || name_tok.length >= (int)sizeof(name)) {
            error(line_num, "%s: Invalid #define syntax", current_file);
            return DIR_ERROR;
        }
        /* Function-like macros (NAME(...)) are not supported: keep directive unchanged. */
        if (name_tok.word[name_tok.length] == '(') {
            buffer_append_n(output, line, line_len);
            return DIR_OK;
        }
        memcpy(name, name_tok.word, (size_t)name_tok.length);
        name[name_tok.length] = '\0';

        /* Value is the remaining text on the line (trimmed) */
        const char *valp = (const char *)((const char *)tk.full_line + tk.position);
        valp = skip_whitespace(valp);

        const char *value_start = valp;
        const char *value_end = value_start + strlen(value_start);
        value_end = trim_define_value_end(value_start, value_end);
        while (value_end > value_start && isspace((unsigned char)*(value_end - 1))) {
            value_end--;
        }

        int vlen = (int)(value_end - value_start);
        if (vlen < 0) vlen = 0;
        if (vlen >= (int)sizeof(value)) vlen = (int)sizeof(value) - 1;

        memcpy(value, value_start, (size_t)vlen);
        value[vlen] = '\0';
        
        /* Add to macro table */
        if (macros_define(macros, name, value) != 0) {
            error(line_num, "%s: Failed to define macro", current_file);
            return DIR_ERROR;
        }
        
        return DIR_OK;  /* Directive processed, don't output it */
    }
    
    /* Handle #ifdef */
    if (token_is_ident(&tok, "ifdef")) {
        char name[PP_MAX_DEFINE_NAME];

        Token name_tok;
        if (!tokenize(&tk, &name_tok) || name_tok.type != IDENTIFIER ||
            name_tok.length <= 0 || name_tok.length >= (int)sizeof(name)) {
            /* Not the supported form '#ifdef IDENTIFIER' -> keep unchanged (or skip if inactive) */
            if (!ifdef_should_include(ifdef_stack)) return DIR_SKIP;
            buffer_append_n(output, line, line_len);
            return DIR_OK;
        }

        /* Reject trailing tokens: only '#ifdef IDENTIFIER' is supported */
        Token extra;
        if (tokenize(&tk, &extra)) {
            if (!ifdef_should_include(ifdef_stack)) return DIR_SKIP;
            buffer_append_n(output, line, line_len);
            return DIR_OK;
        }
        memcpy(name, name_tok.word, (size_t)name_tok.length);
        name[name_tok.length] = '\0';
        
        /* Push onto stack */
        if (ifdef_stack->top >= PP_MAX_IF_DEPTH - 1) {
            error(line_num, "%s: #ifdef nesting too deep", current_file);
            return DIR_ERROR;
        }
        
        /* Only check if macro is defined if parent context is active */
        int should_include = ifdef_should_include(ifdef_stack) && macros_is_defined(macros, name, name_tok.length);
        
        ifdef_stack->top++;
        ifdef_stack->stack[ifdef_stack->top] = should_include;
        
        return DIR_OK;  /* Directive processed */
    }
    
    /* Handle #endif */
    if (token_is_ident(&tok, "endif")) {
        /* Reject trailing tokens: only '#endif' is supported */
        Token extra;
        if (tokenize(&tk, &extra)) {
            if (!ifdef_should_include(ifdef_stack)) return DIR_SKIP;
            buffer_append_n(output, line, line_len);
            return DIR_OK;
        }

        if (ifdef_stack->top >= 0) {
            ifdef_stack->top--;
            return DIR_OK;  /* Directive processed */
        }

        /* Otherwise, this #endif belongs to an unsupported directive - keep it */
        if (!ifdef_should_include(ifdef_stack)) return DIR_SKIP;
        buffer_append_n(output, line, line_len);
        return DIR_OK;
    }
    
    /* Unknown directive - keep it in output */
    if (!ifdef_should_include(ifdef_stack)) {
        return DIR_SKIP;
    }
    buffer_append_n(output, line, line_len);
    return DIR_OK;
}
