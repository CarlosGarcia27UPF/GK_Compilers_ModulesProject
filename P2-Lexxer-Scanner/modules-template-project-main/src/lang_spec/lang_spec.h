/**
 * @file lang_spec.h
 * @brief Language Specification Module
 * 
 * ===============================================
 * LANG_SPEC - LANGUAGE DEFINITION
 * ===============================================
 * 
 * Single source of language constants. No dependencies.
 * 
 * This module defines:
 * - Character classification functions
 * - Keyword tables
 * - Category strings for output
 * - All language-specific constants
 * 
 * Called by: automata, out_writer
 * Calls into: — (no dependencies)
 */

#ifndef LANG_SPEC_H
#define LANG_SPEC_H

#include <stdbool.h>

/* ======================= Token Categories ======================= */

typedef enum {
    CAT_NUMBER,         /* [0-9]+ */
    CAT_IDENTIFIER,     /* [A-Za-z][A-Za-z0-9]* */
    CAT_KEYWORD,        /* if, else, while, return */
    CAT_LITERAL,        /* "[^"]*" */
    CAT_OPERATOR,       /* = > + * */
    CAT_SPECIALCHAR,    /* ( ) ; { } [ ] , */
    CAT_NONRECOGNIZED,  /* Invalid characters */
    CAT_EOF,            /* End of file */
    CAT_ERROR           /* Error token */
} TokenCategory;

/* ======================= Keyword Types ======================= */

typedef enum {
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_RETURN,
    KW_NONE  /* Not a keyword */
} KeywordType;

/* ======================= Constants ======================= */

#define MAX_LEXEME_LEN 256
#define NUM_KEYWORDS 4

/* Keyword strings */
extern const char* KEYWORDS[NUM_KEYWORDS];

/* ======================= Character Classification ======================= */

/**
 * @brief Check if character is a digit [0-9]
 * Part of Σ for NUMBER automaton
 */
bool lang_is_digit(char c);

/**
 * @brief Check if character is a letter [A-Za-z]
 * Part of Σ for IDENTIFIER automaton
 */
bool lang_is_letter(char c);

/**
 * @brief Check if character is letter or digit
 * Used in IDENTIFIER transitions
 */
bool lang_is_letter_or_digit(char c);

/**
 * @brief Check if character is an operator { =, >, +, * }
 * Σ for OPERATOR automaton
 */
bool lang_is_operator(char c);

/**
 * @brief Check if character is a special character { (, ), ;, {, }, [, ], , }
 * Σ for SPECIALCHAR automaton
 */
bool lang_is_special_char(char c);

/**
 * @brief Check if character is whitespace { space, tab, newline, carriage return }
 */
bool lang_is_whitespace(char c);

/**
 * @brief Check if character is a double quote
 * Start of LITERAL automaton
 */
bool lang_is_quote(char c);

/**
 * @brief Check if character is EOF marker
 */
bool lang_is_eof(char c);

/**
 * @brief Check if character can start a valid token
 * Used for NON_RECOGNIZED detection
 */
bool lang_is_valid_token_start(char c);

/**
 * @brief Check if character is non-recognized
 * Characters that cannot start any valid token
 */
bool lang_is_non_recognized(char c);

/* ======================= Keyword Functions ======================= */

/**
 * @brief Check if lexeme is a keyword
 * @param lexeme The string to check
 * @return KeywordType (KW_NONE if not a keyword)
 */
KeywordType lang_get_keyword_type(const char* lexeme);

/**
 * @brief Check if lexeme is a keyword (boolean)
 * @param lexeme The string to check
 * @return true if lexeme is a keyword
 */
bool lang_is_keyword(const char* lexeme);

/* ======================= Category Strings ======================= */

/**
 * @brief Get string representation of token category
 * @param category The token category
 * @return String name (e.g., "NUMBER", "IDENTIFIER")
 */
const char* lang_category_to_string(TokenCategory category);

/**
 * @brief Get string representation of keyword type
 * @param kw The keyword type
 * @return Keyword string (e.g., "if", "while")
 */
const char* lang_keyword_to_string(KeywordType kw);

#endif /* LANG_SPEC_H */
