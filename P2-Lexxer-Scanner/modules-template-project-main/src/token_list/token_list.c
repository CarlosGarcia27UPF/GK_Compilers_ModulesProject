/**
 * @file token_list.c
 * @brief Implementation of Token List Module
 * 
 * Stores tokens only. No formatting or scanning logic.
 */

#include "token_list.h"
#include <stdlib.h>
#include <string.h>

/* ======================= TokenList Functions ======================= */

void token_list_init(TokenList* list) {
    if (list == NULL) return;
    
    list->tokens = NULL;
    list->count = 0;
    list->capacity = 0;
}

void token_list_free(TokenList* list) {
    if (list == NULL) return;
    
    if (list->tokens != NULL) {
        free(list->tokens);
        list->tokens = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}

bool token_list_add(TokenList* list, Token token) {
    if (list == NULL) return false;
    
    /* Allocate initial capacity if needed */
    if (list->tokens == NULL) {
        list->tokens = (Token*)malloc(sizeof(Token) * TOKEN_LIST_INITIAL_CAPACITY);
        if (list->tokens == NULL) return false;
        list->capacity = TOKEN_LIST_INITIAL_CAPACITY;
    }
    
    /* Expand if needed */
    if (list->count >= list->capacity) {
        int new_capacity = list->capacity * 2;
        Token* new_tokens = (Token*)realloc(list->tokens, sizeof(Token) * new_capacity);
        if (new_tokens == NULL) return false;
        list->tokens = new_tokens;
        list->capacity = new_capacity;
    }
    
    /* Add token */
    list->tokens[list->count++] = token;
    return true;
}

Token* token_list_get(TokenList* list, int index) {
    if (list == NULL || index < 0 || index >= list->count) {
        return NULL;
    }
    return &list->tokens[index];
}

int token_list_count(const TokenList* list) {
    if (list == NULL) return 0;
    return list->count;
}

void token_list_clear(TokenList* list) {
    if (list == NULL) return;
    list->count = 0;
}

void token_list_print(const TokenList* list, FILE* output) {
    if (list == NULL || output == NULL) return;
    
    fprintf(output, "TokenList (%d tokens):\n", list->count);
    for (int i = 0; i < list->count; i++) {
        fprintf(output, "  [%d] ", i);
        token_print(&list->tokens[i], output);
    }
}
