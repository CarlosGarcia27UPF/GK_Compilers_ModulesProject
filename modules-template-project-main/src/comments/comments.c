/* -----------------------------------------------------------------------------
 * comments.c
 *
 * Module: comments - Comment removal
 * -------------------------------------------------------------------------- */

#include "comments.h"

typedef enum {
    ST_NORMAL = 0,
    ST_LINE_COMMENT,
    ST_BLOCK_COMMENT,
    ST_STRING,
    ST_CHAR
} CommentState;

void comments_init(void) {
    /* Keep your current behavior so main works */
    fprintf(ofile, "[comments] OK\n");
}

/* Helper: write a single space when entering a comment (exactly once). */
static void write_comment_replacement_space(FILE *out) {
    fputc(' ', out);
}

int comments_remove_stream(FILE *in, FILE *out) {
    CommentState st = ST_NORMAL;
    int c;
    int prev = 0;

    /* For string/char escape handling */
    int escaped = 0;

    while ((c = fgetc(in)) != EOF) {
        switch (st) {
        case ST_NORMAL:
            if (c == '"') {
                fputc(c, out);
                st = ST_STRING;
                escaped = 0;
            } else if (c == '\'') {
                fputc(c, out);
                st = ST_CHAR;
                escaped = 0;
            } else if (c == '/') {
                int n = fgetc(in);
                if (n == '/') {
                    /* Start of line comment */
                    write_comment_replacement_space(out);
                    st = ST_LINE_COMMENT;
                } else if (n == '*') {
                    /* Start of block comment */
                    write_comment_replacement_space(out);
                    st = ST_BLOCK_COMMENT;
                    prev = 0;
                } else {
                    /* Not a comment */
                    fputc(c, out);
                    if (n != EOF) {
                        ungetc(n, in);
                    }
                }
            } else {
                fputc(c, out);
            }
            break;

        case ST_LINE_COMMENT:
            /* Skip until newline; preserve newline */
            if (c == '\n') {
                fputc('\n', out);
                st = ST_NORMAL;
            } else {
                /* discard */
            }
            break;

        case ST_BLOCK_COMMENT:
            /* Preserve newlines inside block comments */
            if (c == '\n') {
                fputc('\n', out);
            }

            /* Detect closing "*/
            if (prev == '*' && c == '/') {
                st = ST_NORMAL;
                prev = 0;
            } else {
                prev = c;
            }
            break;

        case ST_STRING:
            /* Copy everything; handle escapes */
            fputc(c, out);
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
            fputc(c, out);
            if (escaped) {
                escaped = 0;
            } else if (c == '\\') {
                escaped = 1;
            } else if (c == '\'') {
                st = ST_NORMAL;
            }
            break;

        default:
            /* Should never happen */
            st = ST_NORMAL;
            break;
        }
    }

    if (st == ST_BLOCK_COMMENT) {
        /* Unterminated block comment */
        if (ofile) {
            fprintf(ofile, "[comments] ERROR: Unterminated block comment (reached EOF)\n");
        }
        return 1;
    }

    return 0;
}

//To call the function do comments_remove_stream(input_file, output_file);