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
    char *line = tkz->full_line;
    int i = tkz->position;

    // skip whitespaces
    while(line[i] == ' ' || line[i] == '\t'){
        i++;
    }

    // return 0 when end of line
    if(line[i] == '\n' || line[i] == '\0'){
        return 0;
    }

    token_out->word = &line[i];
    token_out->line_n = tkz->line_n;

    if (isalpha((unsigned char)line[i]) || line[i] == '_'){
        int start = i;
        i++;
        while (isalnum(line[i]) || line[i] == '_') {
            i++;
        }
        token_out->type = IDENTIFIER;
        token_out->length = i - start;
    }

    else if (isdigit((unsigned char)line[i])) {
        int start = i;
        i++;
        while (isdigit((unsigned char)line[i])) {
            i++;
        }
        token_out->type = NUMBER;
        token_out->length = i - start;
    }

    else if (line[i] == '"') {
        int start = i;
        // skip opening quote to avoid immediate return
        i++;
        while (line[i] != '"' && line[i] != '\0') {
            i++;
        }
        if (line[i] == '"') {
            // include close quote
            i++;
        }
        token_out->type = STRING;
        token_out->length = i - start;
    }

     else {
        token_out->type = SYMBOL;
        token_out->length = 1;
        i++;
    }

    tkz->position = i;
    return 1;

}

// only used to get the wanted tokens
char* get_word(Token tok) {
    char *word = malloc(tok.length + 1);
    memcpy(word, tok.word, tok.length);
    word[tok.length] = '\0'; // string terminator
    return word;
}

