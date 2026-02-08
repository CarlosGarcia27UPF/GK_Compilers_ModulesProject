/*
 * -----------------------------------------------------------------------------
 * char_stream.h
 *
 * Input cursor module. Provides character-by-character access to the
 * input file with line/column tracking and lookahead (peek).
 * This module only reads characters — it never classifies, skips, or
 * groups them.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include <stdio.h>

// Sentinel returned at end of file.
#define CS_EOF (-1)

// Cursor state for input stream.
typedef struct {
    FILE *fp;      // Input file handle.
    int current;   // Most recently consumed character.
    int line;      // Current 1-based line number.
    int col;       // Current 1-based column number.
    int has_peek;  // 1 when lookahead is buffered.
    int peek_ch;   // Buffered lookahead character.
} char_stream_t;

// Opens and initializes stream from filename.
int cs_open(char_stream_t *cs, const char *filename);

// Returns next character without consuming it.
int cs_peek(char_stream_t *cs);

// Consumes next character, updating line and column.
int cs_get(char_stream_t *cs);

// Returns current line.
int cs_line(const char_stream_t *cs);

// Returns current column.
int cs_col(const char_stream_t *cs);

// Closes input file if open.
void cs_close(char_stream_t *cs);

#endif /* CHAR_STREAM_H */
