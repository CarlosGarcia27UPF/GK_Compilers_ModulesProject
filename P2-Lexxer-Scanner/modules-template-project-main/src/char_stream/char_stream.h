/**
 * @file char_stream.h
 * @brief Character Stream Module (Input Cursor)
 * 
 * ===============================================
 * CHAR_STREAM - INPUT CURSOR
 * ===============================================
 * 
 * Provides cursor-only functionality for reading input.
 * NEVER classifies, skips, or groups characters.
 * 
 * Functions:
 * - cs_peek()  → lookahead, no consume
 * - cs_get()   → consume character
 * - line/col tracking
 * 
 * Called by: driver (initializes), automata (uses)
 * Calls into: OS file I/O
 * 
 * IMPORTANT: This module ONLY provides cursor functionality.
 * All character classification is done by lang_spec.
 * All lexical decisions are done by automata.
 */

#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include <stdio.h>
#include <stdbool.h>

/* ======================= CharStream Structure ======================= */

/**
 * @brief Character stream for reading input
 * Provides lookahead and character consumption
 */
typedef struct {
    const char* source;     /* Source string (for string input) */
    FILE* file;             /* File handle (for file input) */
    int position;           /* Current position */
    int length;             /* Total length (for string input) */
    int line;               /* Current line number (1-based) */
    int column;             /* Current column number (1-based) */
    bool is_file;           /* True if reading from file */
    char lookahead_char;    /* Cached lookahead character */
    bool has_lookahead;     /* True if lookahead is cached */
} CharStream;

/* ======================= Initialization ======================= */

/**
 * @brief Initialize char_stream from a string
 * @param cs Pointer to CharStream
 * @param source Source string
 * @param length Length of source
 */
void cs_init_string(CharStream* cs, const char* source, int length);

/**
 * @brief Initialize char_stream from a file
 * @param cs Pointer to CharStream
 * @param file FILE pointer (must be opened)
 */
void cs_init_file(CharStream* cs, FILE* file);

/**
 * @brief Close/cleanup char_stream
 * @param cs Pointer to CharStream
 */
void cs_close(CharStream* cs);

/* ======================= Core Functions ======================= */

/**
 * @brief Peek at the next character WITHOUT consuming it (lookahead)
 * 
 * This is the lookahead function used by automata to decide
 * when to stop without consuming characters that belong to
 * the next token.
 * 
 * @param cs The character stream
 * @return Next character (or '\0' if at end)
 */
char cs_peek(CharStream* cs);

/**
 * @brief Get and consume the next character
 * 
 * ONLY the automata module should call this function.
 * All lexical decisions must be made inside automata.
 * 
 * @param cs The character stream
 * @return Consumed character (or '\0' if at end)
 */
char cs_get(CharStream* cs);

/**
 * @brief Check if at end of input
 * @param cs The character stream
 * @return true if no more characters
 */
bool cs_is_eof(CharStream* cs);

/* ======================= Position Tracking ======================= */

/**
 * @brief Get current line number
 * @param cs The character stream
 * @return Line number (1-based)
 */
int cs_get_line(const CharStream* cs);

/**
 * @brief Get current column number
 * @param cs The character stream
 * @return Column number (1-based)
 */
int cs_get_column(const CharStream* cs);

/**
 * @brief Get current position
 * @param cs The character stream
 * @return Position in input
 */
int cs_get_position(const CharStream* cs);

#endif /* CHAR_STREAM_H */
