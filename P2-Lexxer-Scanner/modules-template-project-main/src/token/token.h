/**
 * @file token.h
 * @brief Token Object Module
 * 
 * ===============================================
 * TOKEN - TOKEN OBJECT + ENUM
 * ===============================================
 * 
 * Pure data object. No I/O.
 * 
 * Defines:
 * - Token structure (lexeme, category, line)
 * - Token creation function
 * 
 * Called by: automata, token_list, out_writer
 * Calls into: — (no dependencies, uses lang_spec for types)
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdbool.h>
#include "../lang_spec/lang_spec.h"

/* ======================= Token Structure ======================= */

/**
 * @brief Token object
 * Pure data structure representing a lexical token
 */
typedef struct {
    char lexeme[MAX_LEXEME_LEN];    /* The actual string value */
    TokenCategory category;          /* Token category */
    int line;                        /* Line number where token appears */
    int column;                      /* Column number (optional) */
    KeywordType keyword;             /* If CAT_KEYWORD, which one */
} Token;

/* ======================= Token Functions ======================= */

/**
 * @brief Create a new token
 * @param lexeme The lexeme string
 * @param category The token category
 * @param line Line number
 * @return New Token structure
 */
Token token_create(const char* lexeme, TokenCategory category, int line);

/**
 * @brief Create a token with column info
 * @param lexeme The lexeme string
 * @param category The token category
 * @param line Line number
 * @param column Column number
 * @return New Token structure
 */
Token token_create_full(const char* lexeme, TokenCategory category, int line, int column);

/**
 * @brief Print token for debugging
 * @param token The token to print
 * @param output Output stream
 */
void token_print(const Token* token, FILE* output);

#endif /* TOKEN_H */
