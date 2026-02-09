/*
 * -----------------------------------------------------------------------------
 * lang_spec.c
 *
 * Implementation of language specification helpers.
 * All language-dependent logic is concentrated here.
 * Character-by-character keyword matching is used (no string library for
 * recognition in the input stream, per handout requirement).
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "lang_spec.h"
#include <stddef.h>  // NULL

// Category name lookup table.
static const char *category_names[CAT_COUNT] = {
    CAT_NAME_NUMBER,
    CAT_NAME_IDENTIFIER,
    CAT_NAME_KEYWORD,
    CAT_NAME_LITERAL,
    CAT_NAME_OPERATOR,
    CAT_NAME_SPECIALCHAR,
    CAT_NAME_NONRECOGNIZED
};

// Keyword table.
static const char *keywords[NUM_KEYWORDS] = {
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_FOR,
    KW_RETURN,
    KW_INT,
    KW_CHAR,
    KW_VOID
};

// Operator table.
static const char operators[NUM_OPERATORS] = {
    OP_ASSIGN, OP_GT, OP_PLUS, OP_STAR
};

// Special-character table.
static const char specials[NUM_SPECIALS] = {
    SC_LPAREN, SC_RPAREN, SC_SEMICOLON, SC_LBRACE,
    SC_RBRACE, SC_LBRACKET, SC_RBRACKET, SC_COMMA
};

// Returns category display name.
const char* ls_get_category_name(token_category_t cat) {
    if (cat >= 0 && cat < CAT_COUNT) {
        return category_names[cat];
    }
    return CAT_NAME_NONRECOGNIZED;
}

// Compares two null-terminated strings char by char.
static int str_equal_charwise(const char *a, const char *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }
    return (a[i] == '\0' && b[i] == '\0');
}

// Returns 1 when lexeme matches a keyword exactly.
int ls_is_keyword(const char *lexeme) {
    int k;
    if (lexeme == NULL) {
        return 0;
    }
    for (k = 0; k < NUM_KEYWORDS; k++) {
        if (str_equal_charwise(lexeme, keywords[k])) {
            return 1;
        }
    }
    return 0;
}

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

// Returns 1 for whitespace delimiters.
int ls_is_whitespace(char ch) {
    return (ch == WS_SPACE || ch == WS_TAB || ch == WS_CR || ch == WS_NL);
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
