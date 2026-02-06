#include <stdio.h>
#include "tokens.h"

/*
in this test i'm always calling get_word() to make it easier to test,
but in a real example some tokens would just be compared with 
type == IDENTIFIER, or strncmp(token->word,"define", token->lenght == 0)
or other ways
*/

char *token_type_to_string(Token_type t) {
    switch (t) {
        case IDENTIFIER: return "IDENTIFIER";
        case NUMBER:     return "NUMBER";
        case SYMBOL:     return "SYMBOL";
        case STRING:     return "STRING";
        case END:        return "END";
        default:         return "UNKNOWN";
    }
}

int main(void) {
    char line[] = "#define MAX 10\n";

    Tokenizer tk;
    Token tok;

    tokens_init(&tk, 1, line);
    printf("Input line: %s\n", line);
    printf("Tokens:\n");

    while (tokenize(&tk, &tok)) {
        char *word = get_word(tok);

        printf("  %-10s | '%s' | line %d\n",
               token_type_to_string(tok.type),
               word,
               tok.line_n);
        free(word);
    }

    printf("\nSecond test:\n");

    char line2[] = "printf(\"MAX value is %d\", MAX);\n";
    tokens_init(&tk, 2, line2);
    printf("Input line: %s\n", line2);
    printf("Tokens:\n");

    while (tokenize(&tk, &tok)) {
        char *word = get_word(tok);

        printf("  %-10s | '%s'\n",
               token_type_to_string(tok.type),
               word);
        free(word);
    }
    return 0;
}
