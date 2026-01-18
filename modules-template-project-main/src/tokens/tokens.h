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
    int length;     // necessary to get full token word
    int line_n;       // get this from PP_Core directly | here use it to make error line handling easier
} Token;

// tokenizer 
typedef struct {
    int line_n;       // current line from file (save here and pass it to all tokens)
    int position;   // word position currently being tokenized
    char *full_line;
} Tokenizer;


/*
way to use it:

Tokenizer tkz;
tokens_init(&tkz, line_num, full_line) // both line_num and full_line are gotten from PP_core

Token token_out;
while( tokenize(&tkz, &token_out) ){
    use individual token (depending on module does one thing or another)   
 }

*/

// Functions that other modules should call
void tokens_init(Tokenizer *tk, int line_num, char *full_line);

// 1 if word has been tokenized, 0 if end of line, so it works with while loop
int tokenize(Tokenizer *tkz, Token *token_out); // line is raw line text gotten from PP_core

char *get_word(Token);
#endif // TOKENS_H

