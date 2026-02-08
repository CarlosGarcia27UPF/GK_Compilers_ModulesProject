/*
 * -----------------------------------------------------------------------------
 * char_stream.c
 *
 * Input cursor implementation. Reads file one character at a time,
 * tracking line and column numbers.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "char_stream.h"
#include <stddef.h>  // NULL

#define CS_FIRST_LINE 1   // Initial line number.
#define CS_FIRST_COL  1   // Initial column number.
#define CS_INCREMENT  1   // Per-character line/column increment.

// Opens the input file and initializes stream state.
int cs_open(char_stream_t *cs, const char *filename) {
    if (cs == NULL || filename == NULL) {
        return -1;
    }

    cs->fp = fopen(filename, "r");
    if (cs->fp == NULL) {
        return -1;
    }
    cs->current = 0;
    cs->line = CS_FIRST_LINE;
    cs->col = CS_FIRST_COL;
    cs->has_peek = 0;
    cs->peek_ch = 0;
    return 0;
}

// Returns next character without consuming it.
int cs_peek(char_stream_t *cs) {
    if (cs == NULL || cs->fp == NULL) {
        return CS_EOF;
    }

    if (cs->has_peek) {
        return cs->peek_ch;
    }
    cs->peek_ch = fgetc(cs->fp);
    if (cs->peek_ch == EOF) {
        cs->peek_ch = CS_EOF;
    }
    cs->has_peek = 1;
    return cs->peek_ch;
}

// Consumes next character and updates line/column tracking.
int cs_get(char_stream_t *cs) {
    int ch;

    if (cs == NULL || cs->fp == NULL) {
        return CS_EOF;
    }

    if (cs->has_peek) {
        ch = cs->peek_ch;
        cs->has_peek = 0;
    } else {
        ch = fgetc(cs->fp);
        if (ch == EOF) {
            ch = CS_EOF;
        }
    }

    if (ch != CS_EOF) {
        cs->current = ch;
        if (ch == '\n') {
            cs->line += CS_INCREMENT;
            cs->col = CS_FIRST_COL;
        } else {
            cs->col += CS_INCREMENT;
        }
    }
    return ch;
}

// Returns current 1-based line.
int cs_line(const char_stream_t *cs) {
    if (cs == NULL) {
        return CS_FIRST_LINE;
    }
    return cs->line;
}

// Returns current 1-based column.
int cs_col(const char_stream_t *cs) {
    if (cs == NULL) {
        return CS_FIRST_COL;
    }
    return cs->col;
}

// Closes stream file if open.
void cs_close(char_stream_t *cs) {
    if (cs != NULL && cs->fp != NULL) {
        fclose(cs->fp);
        cs->fp = NULL;
    }
}
