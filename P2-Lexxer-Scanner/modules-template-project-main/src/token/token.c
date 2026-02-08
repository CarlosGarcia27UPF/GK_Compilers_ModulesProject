/**
 * @file token.c
 * @brief Implementation of Token Module
 * 
 * Pure data object. No I/O except debug printing.
 */

#include "token.h"
#include <string.h>

/* ======================= Token Functions ======================= */

Token token_create(const char* lexeme, TokenCategory category, int line) {
    return token_create_full(lexeme, category, line, 0);
}

Token token_create_full(const char* lexeme, TokenCategory category, int line, int column) {
    Token token;
    token.category = category;
    token.line = line;
    token.column = column;
    token.keyword = KW_NONE;
    
    /* Copy lexeme safely */
    if (lexeme != NULL) {
        strncpy(token.lexeme, lexeme, MAX_LEXEME_LEN - 1);
        token.lexeme[MAX_LEXEME_LEN - 1] = '\0';
    } else {
        token.lexeme[0] = '\0';
    }
    
    /* Check if it's a keyword (for identifiers) */
    if (category == CAT_IDENTIFIER) {
        KeywordType kw = lang_get_keyword_type(lexeme);
        if (kw != KW_NONE) {
            token.category = CAT_KEYWORD;
            token.keyword = kw;
        }
    }
    
    return token;
}

void token_print(const Token* token, FILE* output) {
    if (token == NULL || output == NULL) return;
    
    fprintf(output, "[%d:%d] %s", 
            token->line, 
            token->column, 
            lang_category_to_string(token->category));
    
    if (token->category == CAT_KEYWORD) {
        fprintf(output, "(%s)", lang_keyword_to_string(token->keyword));
    }
    
    fprintf(output, ": \"%s\"\n", token->lexeme);
}
