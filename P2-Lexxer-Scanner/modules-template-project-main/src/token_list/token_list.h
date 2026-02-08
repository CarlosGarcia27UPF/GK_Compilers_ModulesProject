/**
 * @file token_list.h
 * @brief Token List Module (Ordered Token Stream)
 * 
 * ===============================================
 * TOKEN_LIST - ORDERED TOKEN STREAM
 * ===============================================
 * 
 * Stores tokens only. No formatting or scanning logic.
 * 
 * Called by: driver, automata, out_writer, (future) parser
 * Calls into: token (free)
 */

#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

#include "../token/token.h"
#include <stdbool.h>

/* Initial capacity for token array */
#define TOKEN_LIST_INITIAL_CAPACITY 256

/* ======================= TokenList Structure ======================= */

/**
 * @brief Dynamic list of tokens
 */
typedef struct {
    Token* tokens;      /* Dynamic array of tokens */
    int count;          /* Number of tokens in list */
    int capacity;       /* Allocated capacity */
} TokenList;

/* ======================= TokenList Functions ======================= */

/**
 * @brief Initialize an empty token list
 * @param list Pointer to TokenList
 */
void token_list_init(TokenList* list);

/**
 * @brief Free token list resources
 * @param list Pointer to TokenList
 */
void token_list_free(TokenList* list);

/**
 * @brief Add a token to the list
 * @param list The token list
 * @param token Token to add
 * @return true if added successfully
 */
bool token_list_add(TokenList* list, Token token);

/**
 * @brief Get token at index
 * @param list The token list
 * @param index Index (0-based)
 * @return Pointer to token, or NULL if invalid index
 */
Token* token_list_get(TokenList* list, int index);

/**
 * @brief Get number of tokens
 * @param list The token list
 * @return Number of tokens
 */
int token_list_count(const TokenList* list);

/**
 * @brief Clear all tokens (keep capacity)
 * @param list The token list
 */
void token_list_clear(TokenList* list);

/**
 * @brief Print all tokens (for debugging)
 * @param list The token list
 * @param output Output stream
 */
void token_list_print(const TokenList* list, FILE* output);

#endif /* TOKEN_LIST_H */
