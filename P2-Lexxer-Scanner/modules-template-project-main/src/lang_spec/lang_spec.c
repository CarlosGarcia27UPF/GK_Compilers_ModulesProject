/**
 * @file lang_spec.c
 * @brief Implementation of Language Specification Module
 * 
 * Single source of all language constants and character classification.
 */

#include "lang_spec.h"
#include <string.h>

/* ======================= Keyword Table ======================= */

const char* KEYWORDS[NUM_KEYWORDS] = {
    "if",
    "else",
    "while",
    "return"
};

/* ======================= Character Classification ======================= */

bool lang_is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool lang_is_letter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

bool lang_is_letter_or_digit(char c) {
    return lang_is_letter(c) || lang_is_digit(c);
}

bool lang_is_operator(char c) {
    return c == '=' || c == '>' || c == '<' || c == '+' || c == '-' || c == '*' || c == '/' || c == '!';
}

bool lang_is_special_char(char c) {
    return c == '(' || c == ')' || c == ';' || 
           c == '{' || c == '}' || c == '[' || 
           c == ']' || c == ',';
}

bool lang_is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool lang_is_quote(char c) {
    return c == '"';
}

bool lang_is_eof(char c) {
    return c == '\0';
}

bool lang_is_valid_token_start(char c) {
    return lang_is_digit(c) || lang_is_letter(c) || lang_is_quote(c) ||
           lang_is_operator(c) || lang_is_special_char(c) || 
           lang_is_whitespace(c) || lang_is_eof(c);
}

bool lang_is_non_recognized(char c) {
    return !lang_is_valid_token_start(c);
}

/* ======================= Keyword Functions ======================= */

KeywordType lang_get_keyword_type(const char* lexeme) {
    if (lexeme == NULL) return KW_NONE;
    
    if (strcmp(lexeme, "if") == 0)     return KW_IF;
    if (strcmp(lexeme, "else") == 0)   return KW_ELSE;
    if (strcmp(lexeme, "while") == 0)  return KW_WHILE;
    if (strcmp(lexeme, "return") == 0) return KW_RETURN;
    
    return KW_NONE;
}

bool lang_is_keyword(const char* lexeme) {
    return lang_get_keyword_type(lexeme) != KW_NONE;
}

/* ======================= Category Strings ======================= */

const char* lang_category_to_string(TokenCategory category) {
    switch (category) {
        case CAT_NUMBER:        return "NUMBER";
        case CAT_IDENTIFIER:    return "IDENTIFIER";
        case CAT_KEYWORD:       return "KEYWORD";
        case CAT_LITERAL:       return "LITERAL";
        case CAT_OPERATOR:      return "OPERATOR";
        case CAT_SPECIALCHAR:   return "SPECIALCHAR";
        case CAT_NONRECOGNIZED: return "NONRECOGNIZED";
        case CAT_EOF:           return "EOF";
        case CAT_ERROR:         return "ERROR";
        default:                return "UNKNOWN";
    }
}

const char* lang_keyword_to_string(KeywordType kw) {
    switch (kw) {
        case KW_IF:     return "if";
        case KW_ELSE:   return "else";
        case KW_WHILE:  return "while";
        case KW_RETURN: return "return";
        case KW_NONE:   return "";
        default:        return "UNKNOWN";
    }
}
