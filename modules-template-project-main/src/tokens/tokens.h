/*
 * -----------------------------------------------------------------------------
 * tokens.h
 *
 * Module: tokens - Tokenization/Lexer
 * Responsible for: Breaking source code into tokens for processing
 *
 * Author: [Team Member 7]
 * -----------------------------------------------------------------------------
 */

#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>

extern FILE* ofile;

typedef enum {
    IDENTIFIER,     // define, MAX, ifdef, x...
    NUMBER,         // numbers 
    SYMBOL,         // #, (), =, < ...
    STRING,         // literals: "file.txt", "int", "for"....
    END             // end of line "\n"
} Token_type;

// individual token structure
typedef struct {
    Token_type type;
    char *word;     // pointer to start of token (no malloc for all & faster)
    int lenght;     // necessary to get full token word
    int line;       // get this from PP_Core directly | here use it to make error line handling easier
} Token;

// tokenizer 
typedef struct {
    int line;       // current line from file (save here and pass it to all tokens)
    int position;   // word position currently being tokenized
} Tokenizer;



void tokens_init(void);

void tokenize_next();

char get_word(Token);
#endif // TOKENS_H
