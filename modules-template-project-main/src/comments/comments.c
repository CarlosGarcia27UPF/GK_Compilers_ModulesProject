/* -----------------------------------------------------------------------------
 * comments.c
 *
 * Module: comments - Comment removal
 * -------------------------------------------------------------------------- */

#include "comments.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

typedef enum {
    ST_NORMAL = 0,
    ST_LINE_COMMENT,
    ST_BLOCK_COMMENT,
    ST_STRING,
    ST_CHAR
} CommentState;

void comments_state_init(comment_state_t *state) {
    state->in_block_comment = 0;
    state->prev_char = 0;
}

void comments_update_state(const char *input, long input_len, comment_state_t *state)
{
    if (!input || !state) return;

    /* Reuse the main comment-processing logic and discard output. */
    buffer_t sink;
    buffer_init(&sink);
    comments_process_line(input, input_len, &sink, state);
    buffer_free(&sink);
}

/* Process a single line removing comments while preserving state */
int comments_process_line(const char *input, long input_len, buffer_t *output, comment_state_t *state) {
    if (!input || !output || !state) return 1;
    
    CommentState st = state->in_block_comment ? ST_BLOCK_COMMENT : ST_NORMAL;
    int prev = state->prev_char;
    int escaped = 0;
    int wrote_space = 0;  /* Track if we already wrote space for current comment */
    
    for (long i = 0; i < input_len; i++) {
        int c = (unsigned char)input[i];
        
        switch (st) {
        case ST_NORMAL:
            if (c == '"') {
                buffer_append_char(output, c);
                st = ST_STRING;
                escaped = 0;
            } else if (c == '\'') {
                buffer_append_char(output, c);
                st = ST_CHAR;
                escaped = 0;
            } else if (c == '/' && i + 1 < input_len) {
                int n = (unsigned char)input[i + 1];
                if (n == '/') {
                    /* Start of line comment */
                    buffer_append_char(output, ' ');
                    st = ST_LINE_COMMENT;
                    wrote_space = 1;
                    i++;  /* Skip the second '/' */
                } else if (n == '*') {
                    /* Start of block comment */
                    buffer_append_char(output, ' ');
                    st = ST_BLOCK_COMMENT;
                    wrote_space = 1;
                    prev = 0;
                    i++;  /* Skip the '*' */
                } else {
                    buffer_append_char(output, c);
                }
            } else {
                buffer_append_char(output, c);
            }
            break;

        case ST_LINE_COMMENT:
            /* Skip until newline; preserve newline */
            if (c == '\n') {
                buffer_append_char(output, '\n');
                st = ST_NORMAL;
                wrote_space = 0;
            }
            /* discard other characters */
            break;

        case ST_BLOCK_COMMENT:
            /* Preserve newlines inside block comments */
            if (c == '\n') {
                buffer_append_char(output, '\n');
            }
            
            /* Detect closing star-slash */
            if (prev == '*' && c == '/') {
                st = ST_NORMAL;
                wrote_space = 0;
                prev = 0;
            } else {
                prev = c;
            }
            break;

        case ST_STRING:
            /* Copy everything; handle escapes */
            buffer_append_char(output, c);
            if (escaped) {
                escaped = 0;
            } else if (c == '\\') {
                escaped = 1;
            } else if (c == '"') {
                st = ST_NORMAL;
            }
            break;

        case ST_CHAR:
            /* Copy everything; handle escapes */
            buffer_append_char(output, c);
            if (escaped) {
                escaped = 0;
            } else if (c == '\\') {
                escaped = 1;
            } else if (c == '\'') {
                st = ST_NORMAL;
            }
            break;
        }
    }
    
    /* Save state for next line */
    state->in_block_comment = (st == ST_BLOCK_COMMENT);
    state->prev_char = prev;
    
    return 0;
}