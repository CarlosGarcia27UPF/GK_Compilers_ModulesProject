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

/* Extract a quoted filename from #include "file" */
static int extract_include_filename(const char *line, char *filename, int maxlen) {
    const char *p = skip_whitespace(line);
    
    if (*p != '"') return 1;  /* Must start with " */
    p++;
    
    int i = 0;
    while (*p && *p != '"' && i < maxlen - 1) {
        filename[i++] = *p++;
    }
    filename[i] = '\0';
    
    if (*p != '"') return 1;  /* Must end with " */
    return 0;
}

/* Extract macro name and value from #define NAME VALUE */
static int extract_define(const char *line, char *name, int name_max, char *value, int value_max) {
    const char *p = skip_whitespace(line);
    
    /* Extract name (identifier) */
    int i = 0;
    if (!isalpha((unsigned char)*p) && *p != '_') return 1;
    
    while (*p && (isalnum((unsigned char)*p) || *p == '_') && i < name_max - 1) {
        name[i++] = *p++;
    }
    name[i] = '\0';
    
    if (i == 0) return 1;  /* Empty name */
    
    /* Skip whitespace before value */
    p = skip_whitespace(p);
    
    /* Extract value (rest of line, trimming trailing whitespace and newline) */
    const char *value_start = p;
    const char *value_end = p + strlen(p);
    
    /* Trim trailing whitespace/newline */
    while (value_end > value_start && isspace((unsigned char)*(value_end - 1))) {
        value_end--;
    }
    
    int len = value_end - value_start;
    if (len >= value_max) len = value_max - 1;
    
    memcpy(value, value_start, len);
    value[len] = '\0';
    
    return 0;
}

/* Extract identifier from #ifdef IDENTIFIER */
static int extract_ifdef_name(const char *line, char *name, int maxlen) {
    const char *p = skip_whitespace(line);
    
    int i = 0;
    if (!isalpha((unsigned char)*p) && *p != '_') return 1;
    
    while (*p && (isalnum((unsigned char)*p) || *p == '_') && i < maxlen - 1) {
        name[i++] = *p++;
    }
    name[i] = '\0';
    
    return (i == 0) ? 1 : 0;
}

int directives_process_line(const char *line, long line_len, 
                           const char *base_dir,
                           const char *current_file, int line_num,
                           macro_table_t *macros, 
                           ifdef_stack_t *ifdef_stack,
                           buffer_t *output) {
    if (!line || !macros || !ifdef_stack) return 1;
    
    const char *p = skip_whitespace(line);
    
    /* Must start with '#' */
    if (*p != '#') return 1;
    p++;
    
    const char *directive_start = skip_whitespace(p);
    
    /* Handle #include */
    if (strncmp(directive_start, "include", 7) == 0) {
        /* Only process if we're in an active #ifdef context */
        if (!ifdef_should_include(ifdef_stack)) {
            return 2;  /* Skip this directive */
        }
        
        const char *args = directive_start + 7;
        char filename[256];
        
        if (extract_include_filename(args, filename, sizeof(filename)) != 0) {
            error_report(current_file, line_num, "Invalid #include syntax");
            return 1;
        }
        
        /* Build full path with bounds checking */
        char full_path[PP_MAX_PATH_LEN];
        int path_len;
        if (base_dir && base_dir[0]) {
            path_len = snprintf(full_path, sizeof(full_path), "%s/%s", base_dir, filename);
        } else {
            path_len = snprintf(full_path, sizeof(full_path), "%s", filename);
        }
        
        if (path_len < 0 || path_len >= (int)sizeof(full_path)) {
            error_report(current_file, line_num, "Include path too long: %s", filename);
            return 1;
        }
        
        /* Read the included file */
        buffer_t included;
        buffer_init(&included);
        
        if (io_read_file(full_path, &included) != 0) {
            error_report(current_file, line_num, "Cannot open included file: %s", full_path);
            buffer_free(&included);
            return 1;
        }
        
        /* Process the included file recursively (remove comments first) */
        comment_state_t comment_state;
        comments_state_init(&comment_state);
        
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
                
                /* Remove comments from line */
                comments_process_line(included.data + line_start, len, &line_buf, &comment_state);
                
                /* Check if it's a directive and process recursively */
                const char *lp = skip_whitespace(line_buf.data);
                if (*lp == '#') {
                    directives_process_line(line_buf.data, line_buf.len, base_dir, full_path, 
                                          included_line_num, macros, ifdef_stack, &processed);
                } else {
                    /* Expand macros if not inside false ifdef */
                    if (ifdef_should_include(ifdef_stack)) {
                        buffer_t expanded;
                        buffer_init(&expanded);
                        macros_expand_line(macros, line_buf.data, line_buf.len, &expanded);
                        buffer_append_n(&processed, expanded.data, expanded.len);
                        buffer_free(&expanded);
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
    if (strncmp(directive_start, "define", 6) == 0) {
        /* Only process if we're in an active #ifdef context */
        if (!ifdef_should_include(ifdef_stack)) {
            return 2;  /* Skip this directive */
        }
        
        const char *args = directive_start + 6;
        char name[128];
        char value[512];
        
        if (extract_define(args, name, sizeof(name), value, sizeof(value)) != 0) {
            error_report(current_file, line_num, "Invalid #define syntax");
            return 1;
        }
        
        /* Add to macro table */
        macros_define(macros, name, value);
        
        return 0;  /* Directive processed, don't output it */
    }
    
    /* Handle #ifdef */
    if (strncmp(directive_start, "ifdef", 5) == 0) {
        const char *args = directive_start + 5;
        char name[128];
        
        if (extract_ifdef_name(args, name, sizeof(name)) != 0) {
            error_report(current_file, line_num, "Invalid #ifdef syntax");
            return 1;
        }
        
        /* Push onto stack */
        if (ifdef_stack->top >= PP_MAX_IF_DEPTH - 1) {
            error_report(current_file, line_num, "#ifdef nesting too deep");
            return 1;
        }
        
        /* Only check if macro is defined if parent context is active */
        int should_include = ifdef_should_include(ifdef_stack) && macros_is_defined(macros, name);
        
        ifdef_stack->top++;
        ifdef_stack->stack[ifdef_stack->top] = should_include;
        
        return 0;  /* Directive processed */
    }
    
    /* Handle #endif */
    if (strncmp(directive_start, "endif", 5) == 0) {
        /* Only pop if we have an #ifdef on the stack */
        if (ifdef_stack->top >= 0) {
            ifdef_stack->top--;
            return 0;  /* Directive processed */
        }
        /* Otherwise, this #endif belongs to an unsupported directive - keep it */
        buffer_append_n(output, line, line_len);
        return 0;
    }
    
    /* Unknown directive - keep it in output */
    buffer_append_n(output, line, line_len);
    return 0;
}