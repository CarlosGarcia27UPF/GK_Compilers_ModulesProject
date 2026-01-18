/*
 * -----------------------------------------------------------------------------
 * tokens.c
 *
 * Module: tokens - Tokenization/Lexer
 * Responsible for: Breaking source code into tokens for processing
 *
 * Author: [Team Member 7]
 * -----------------------------------------------------------------------------
 */

#include "tokens.h"


void tokens_init(void) {
    fprintf(ofile, "[tokens] OK\n");
}

// only used to get the wanted tokens
char* get_word(Token tok) {
    char *word = malloc(tok.lenght + 1);
    memcpy(word, tok.word, tok.lenght);
    word[tok.lenght] = '\0'; // string terminator
    return word;
}

