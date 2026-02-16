/*
 * -----------------------------------------------------------------------------
 * token_list.c
 *
 * Ordered token list implementation using a growable dynamic array.
 *
 * Team: Compilers P2
 * 
 * Author: Francesc Baiget
 * -----------------------------------------------------------------------------
 */

#include "token_list.h"
#include <stdlib.h>  // malloc, realloc, free
#include <stdio.h>   // fprintf, stderr
#include <stddef.h>  // NULL

// Allocates initial storage for the token list.
void tl_init(token_list_t *list) {
    if (list == NULL) {
        return;
    }

    list->tokens = (token_t *)malloc(TL_INIT_CAPACITY * sizeof(token_t));
    if (list->tokens == NULL) {
        fprintf(stderr, "tl_init: memory allocation failed\n");
        list->capacity = 0;
    } else {
        list->capacity = TL_INIT_CAPACITY;
    }
    list->count = 0;
}

// Doubles storage capacity when needed.
static int tl_grow(token_list_t *list) {
    if (list == NULL) {
        return -1;
    }

    if (list->capacity <= 0) {
        list->tokens = (token_t *)malloc(TL_INIT_CAPACITY * sizeof(token_t));
        if (list->tokens == NULL) {
            fprintf(stderr, "tl_grow: memory allocation failed\n");
            return -1;
        }
        list->capacity = TL_INIT_CAPACITY;
        return 0;
    }

    int new_cap = list->capacity * TL_GROWTH_FACTOR;
    token_t *new_buf = (token_t *)realloc(list->tokens, new_cap * sizeof(token_t));
    if (new_buf == NULL) {
        fprintf(stderr, "tl_grow: memory reallocation failed\n");
        return -1;
    }
    list->tokens = new_buf;
    list->capacity = new_cap;
    return 0;
}

// Appends a token copy to the list.
void tl_add(token_list_t *list, const token_t *tok) {
    if (list == NULL || tok == NULL) {
        return;
    }

    if (list->count >= list->capacity) {
        if (tl_grow(list) != 0) {
            return;
        }
    }
    list->tokens[list->count] = *tok;
    list->count++;
}

// Returns token pointer by index.
const token_t* tl_get(const token_list_t *list, int index) {
    if (list == NULL || index < 0 || index >= list->count) {
        return NULL;
    }
    return &list->tokens[index];
}

// Returns number of tokens stored.
int tl_count(const token_list_t *list) {
    if (list == NULL) {
        return 0;
    }
    return list->count;
}

// Releases list memory.
void tl_free(token_list_t *list) {
    if (list == NULL) {
        return;
    }

    if (list->tokens != NULL) {
        free(list->tokens);
        list->tokens = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}
