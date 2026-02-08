/*
 * -----------------------------------------------------------------------------
 * token.h
 *
 * Token data object definition. A token is <lexeme, category> with
 * optional metadata (source line, column).
 * Pure data object — no I/O in this module.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "../lang_spec/lang_spec.h"

// Token payload and source coordinates.
typedef struct {
    char lexeme[MAX_LEXEME_LEN]; // Token lexeme string.
    token_category_t category;   // Token category.
    int line;                    // Source 1-based line.
    int col;                     // Source 1-based column.
} token_t;

// Initializes one token.
void token_init(token_t *tok, const char *lexeme, token_category_t cat,
                int line, int col);

#endif /* TOKEN_H */
