/*
 * -----------------------------------------------------------------------------
 * token_list.h
 *
 * Ordered token list (dynamic array). Stores tokens in the order they
 * appear in the input. No formatting or scanning logic here.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

#include "../token/token.h"

// Initial capacity and growth factor for token storage.
#define TL_INIT_CAPACITY 128
#define TL_GROWTH_FACTOR 2

// Token list storage.
typedef struct {
    token_t *tokens;   // Dynamic token array.
    int count;         // Number of used slots.
    int capacity;      // Allocated token slots.
} token_list_t;

// Initializes an empty token list.
void tl_init(token_list_t *list);

// Adds one token.
void tl_add(token_list_t *list, const token_t *tok);

// Returns token at index or NULL.
const token_t* tl_get(const token_list_t *list, int index);

// Returns token count.
int tl_count(const token_list_t *list);

// Frees token list storage.
void tl_free(token_list_t *list);

#endif /* TOKEN_LIST_H */
