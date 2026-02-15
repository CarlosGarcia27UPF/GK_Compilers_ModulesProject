/*
 * -----------------------------------------------------------------------------
 * lang_spec.c
 *
 * Implementation of language specification helpers.
 * All language-dependent logic is concentrated here.
 * Exposes character classification helpers consumed by the scanner DFA.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "lang_spec.h"

// Operator table.
static const char operators[NUM_OPERATORS] = {
    OP_ASSIGN, OP_GT, OP_PLUS, OP_STAR
};

// Special-character table.
static const char specials[NUM_SPECIALS] = {
    SC_LPAREN, SC_RPAREN, SC_SEMICOLON, SC_LBRACE,
    SC_RBRACE, SC_LBRACKET, SC_RBRACKET, SC_COMMA
};

// Returns 1 for supported operators.
int ls_is_operator(char ch) {
    int i;
    for (i = 0; i < NUM_OPERATORS; i++) {
        if (ch == operators[i]) {
            return 1;
        }
    }
    return 0;
}

// Returns 1 for supported special characters.
int ls_is_special_char(char ch) {
    int i;
    for (i = 0; i < NUM_SPECIALS; i++) {
        if (ch == specials[i]) {
            return 1;
        }
    }
    return 0;
}

// Returns 1 for [A-Za-z].
int ls_is_letter(char ch) {
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

// Returns 1 for [0-9].
int ls_is_digit(char ch) {
    return (ch >= '0' && ch <= '9');
}

// Returns 1 for literal quote delimiter.
int ls_is_quote(char ch) {
    return (ch == LIT_QUOTE);
}
