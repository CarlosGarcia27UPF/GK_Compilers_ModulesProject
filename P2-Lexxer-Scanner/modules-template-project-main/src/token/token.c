/*
 * -----------------------------------------------------------------------------
 * token.c
 *
 * Token data object implementation.
 * Copies the lexeme character-by-character (no string library for recognition).
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "token.h"
#include <stddef.h>

// Initializes token fields and copies lexeme content into owned storage.
void token_init(token_t *tok, const char *lexeme, token_category_t cat,
                int line, int col) {
    int i = 0;

    if (tok == NULL) {
        return;
    }

    if (lexeme == NULL) {
        tok->lexeme[0] = '\0';
        tok->category = cat;
        tok->line = line;
        tok->col = col;
        return;
    }

    while (lexeme[i] != '\0' && i < MAX_LEXEME_LEN - 1) {
        tok->lexeme[i] = lexeme[i];
        i++;
    }
    tok->lexeme[i] = '\0';
    tok->category = cat;
    tok->line = line;
    tok->col = col;
}
