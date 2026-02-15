/*
 * SIMPLE EXAMPLE: How to use the logger with counter macros
 * 
 * This shows practical usage patterns in your scanner code
 */

#include "logger.h"
#include <stdio.h>
#include <ctype.h>

/* example counter structure */
typedef struct {
    long comp;
    long io;
    long gen;
} simple_counter_t;

/* ===== simple scanner example with counters ===== */

/*
 * simple token scanner example
 * shows how to use COUNT_* macros naturally
 */
void simple_scan_example(FILE *input, logger_t *lg, simple_counter_t *cnt) {
    int ch;
    int line_num = 1;
    
    printf("scanning tokens...\n");
    
    while ((ch = fgetc(input)) != EOF) {
        /* count i/o: one character read */
        COUNT_IO(cnt, 1);
        
        /* count comparison: is it newline? */
        COUNT_COMP(cnt, 1);
        if (ch == '\n') {
            line_num++;
            COUNT_GEN(cnt, 1);  /* line counter increment */
            continue;
        }
        
        /* skip whitespace */
        COUNT_COMP(cnt, 1);
        if (isspace(ch)) {
            COUNT_GEN(cnt, 1);  /* skip operation */
            continue;
        }
        
        /* check for operators */
        COUNT_COMP(cnt, 1);
        if (ch == '+' || ch == '-') {
            COUNT_GEN(cnt, 1);  /* operator found */
            printf("found operator: %c\n", ch);
        }
        /* check for digits */
        else if (isdigit(ch)) {
            COUNT_GEN(cnt, 1);  /* digit found */
            printf("found digit: %c\n", ch);
        }
        /* check for letters */
        else if (isalpha(ch)) {
            COUNT_GEN(cnt, 1);  /* letter found */
            printf("found letter: %c\n", ch);
        }
    }
    
    /* log final counts for this function */
    logger_log_counts(lg, line_num, "simple_scan_example",
                      cnt->comp, cnt->io, cnt->gen);
}

/* ===== complete main example ===== */

/*
 * compile with different flags to see different behavior:
 * 
 * gcc -o example example.c logger.c
 *   (release mode, no counters)
 * 
 * gcc -DCOUNTCONFIG=1 -o example_count example.c logger.c
 *   (release mode with counters to stdout)
 * 
 * gcc -DDEBUG_FLAG=1 -DOUTFORMAT=1 -DCOUNTCONFIG=1 -o example_debug example.c logger.c
 *   (debug with line numbers and counters to .scn file)
 */

int main() {
    logger_t lg;
    simple_counter_t cnt = {0};
    FILE *test_file;
    FILE *outfile;
    
    /* create test input file */
    test_file = fopen("test_input.txt", "w");
    if (test_file == NULL) {
        perror("fopen test_input.txt");
        return 1;
    }
    
    fprintf(test_file, "x = 5 + 3;\n");
    fprintf(test_file, "y = x - 2;\n");
    fclose(test_file);
    
    /* create output file for debug mode */
    outfile = fopen("test_input.txt.scn", "w");
    if (outfile == NULL) {
        perror("fopen output");
        return 1;
    }
    
    /* initialize logger */
    logger_init(&lg, outfile, "test_input.txt");
    
    printf("=== logger example ===\n");
    printf("input file: test_input.txt\n");
    printf("output file: test_input.txt.scn\n");
#ifdef COUNTCONFIG
    printf("counter file: test_input.txt.dbgcnt (if separate)\n");
#endif
    printf("\n");
    
    /* open input and scan */
    test_file = fopen("test_input.txt", "r");
    if (test_file) {
        simple_scan_example(test_file, &lg, &cnt);
        fclose(test_file);
    }
    
    /* write sample tokens (showing logger_write_tokens) */
    printf("\nwriting sample tokens...\n");
    logger_write_tokens(&lg, 1, "x");
    logger_write_tokens(&lg, 1, "=");
    logger_write_tokens(&lg, 1, "5");
    logger_write_tokens(&lg, 1, "+");
    logger_write_tokens(&lg, 1, "3");
    logger_write_tokens(&lg, 1, ";");
    
    logger_write_tokens(&lg, 2, "y");
    logger_write_tokens(&lg, 2, "=");
    logger_write_tokens(&lg, 2, "x");
    logger_write_tokens(&lg, 2, "-");
    logger_write_tokens(&lg, 2, "2");
    logger_write_tokens(&lg, 2, ";");
    
    /* cleanup */
    logger_close(&lg);
    fclose(outfile);
    
    printf("\n=== done ===\n");
    printf("check test_input.txt.scn for token output\n");
#ifdef COUNTCONFIG
    printf("check test_input.txt.dbgcnt for counter info\n");
#endif
    
    return 0;
}

/*
 * ===== QUICK TEST: What to type in terminal =====
 * 
 * TEST 1: Release mode (minimal output)
 * $ gcc -o example example.c logger.c
 * $ ./example
 * 
 * Expected stdout:
 *   === logger example ===
 *   input file: test_input.txt
 *   output file: test_input.txt.scn
 *   
 *   scanning tokens...
 *   found operator: +
 *   found digit: 5
 *   found digit: 3
 *   found operator: -
 *   found digit: 2
 *   
 *   writing sample tokens...
 *   
 *   === done ===
 *   check test_input.txt.scn for token output
 * 
 * 
 * TEST 2: With counters to separate file
 * $ gcc -DCOUNTCONFIG=1 -DCOUNTOUT=0 -o example_count example.c logger.c
 * $ ./example_count
 * 
 * Expected: test_input.txt.dbgcnt will have counter info
 * 
 * 
 * TEST 3: Debug mode with line numbers
 * $ gcc -DOUTFORMAT=1 -DCOUNTCONFIG=1 -o example_debug example.c logger.c
 * $ ./example_debug
 * 
 * Expected: test_input.txt.scn will have:
 *   1 x
 *   
 *   1 =
 *   
 *   1 5
 *   ...
 * 
 */