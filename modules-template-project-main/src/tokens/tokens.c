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


void tokens_init(Tokenizer *tk, int line_num, char *full_line) {
    tk->line_n = line_num;
    tk->position = 0;
    tk->full_line = full_line;
}

int tokenize(Tokenizer *tkz, Token *token_out){
    // TODO   
}

// only used to get the wanted tokens
<<<<<<< HEAD
char *get_word(Token tok) {
    char *word = malloc(tok.length + 1);
    memcpy(word, tok.word, tok.length);
    word[tok.length] = '\0'; // string terminator
=======
char* get_word(Token tok) {
    char *word = malloc(tok.lenght + 1);
    memcpy(word, tok.word, tok.lenght);
    word[tok.lenght] = '\0'; // string terminator
>>>>>>> ab574046407cc7f316cea967113e34168882e6ca
    return word;
}

