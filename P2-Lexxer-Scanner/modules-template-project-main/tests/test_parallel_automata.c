/*
 * test_parallel_automata.c
 *
 * Unit tests specifically for the parallel automata implementation.
 * Verifies that all automata run simultaneously and longest match wins.
 *
 * Team: Compilers P2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include "../src/char_stream/char_stream.h"
#include "../src/token/token.h"
#include "../src/token_list/token_list.h"
#include "../src/automata/automata.h"
#include "../src/logger/logger.h"
#include "../src/counter/counter.h"
#include "../src/lang_spec/lang_spec.h"

// Helper function to create a test file and scan it
static void test_scan_string(const char *input, token_list_t *tokens, 
                             const char *test_name) {
    char temp_file[] = "/tmp/test_automata_XXXXXX";
    int fd = mkstemp(temp_file);
    assert(fd != -1);
    
    write(fd, input, strlen(input));
    close(fd);
    
    char_stream_t cs;
    logger_t lg;
    counter_t cnt;
    
    counter_init(&cnt);
    tl_init(tokens);
    logger_init(&lg, stdout);
    
    assert(cs_open(&cs, temp_file) == 0);
    
    printf("    Testing: %s\n", test_name);
    int result = automata_scan(&cs, tokens, &lg, &cnt);
    
    cs_close(&cs);
    unlink(temp_file);
}

// Test 1: Numbers should be recognized correctly
static void test_numbers(void) {
    printf("  Test 1: Numbers\n");
    token_list_t tokens;
    
    test_scan_string("123", &tokens, "single number");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_NUMBER);
    assert(strcmp(tl_get(&tokens, 0)->lexeme, "123") == 0);
    tl_free(&tokens);
    
    test_scan_string("1 22 333", &tokens, "multiple numbers");
    assert(tl_count(&tokens) == 3);
    assert(tl_get(&tokens, 0)->category == CAT_NUMBER);
    assert(tl_get(&tokens, 1)->category == CAT_NUMBER);
    assert(tl_get(&tokens, 2)->category == CAT_NUMBER);
    tl_free(&tokens);
    
    printf("  Numbers tests PASSED\n");
}

// Test 2: Identifiers vs Keywords
static void test_identifiers_and_keywords(void) {
    printf("  Test 2: Identifiers and Keywords\n");
    token_list_t tokens;
    
    test_scan_string("abc", &tokens, "identifier");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_IDENTIFIER);
    assert(strcmp(tl_get(&tokens, 0)->lexeme, "abc") == 0);
    tl_free(&tokens);
    
    test_scan_string("if", &tokens, "keyword if");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_KEYWORD);
    assert(strcmp(tl_get(&tokens, 0)->lexeme, "if") == 0);
    tl_free(&tokens);
    
    test_scan_string("while", &tokens, "keyword while");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_KEYWORD);
    tl_free(&tokens);
    
    test_scan_string("var123", &tokens, "identifier with digits");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_IDENTIFIER);
    assert(strcmp(tl_get(&tokens, 0)->lexeme, "var123") == 0);
    tl_free(&tokens);
    
    printf("  Identifiers and Keywords tests PASSED\n");
}

// Test 3: Literals
static void test_literals(void) {
    printf("  Test 3: Literals\n");
    token_list_t tokens;
    
    test_scan_string("\"hello\"", &tokens, "simple literal");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_LITERAL);
    assert(strcmp(tl_get(&tokens, 0)->lexeme, "\"hello\"") == 0);
    tl_free(&tokens);
    
    test_scan_string("\"a b c\"", &tokens, "literal with spaces");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_LITERAL);
    tl_free(&tokens);
    
    test_scan_string("\"\"", &tokens, "empty literal");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_LITERAL);
    tl_free(&tokens);
    
    printf("  Literals tests PASSED\n");
}

// Test 4: Operators
static void test_operators(void) {
    printf("  Test 4: Operators\n");
    token_list_t tokens;
    
    test_scan_string("+", &tokens, "plus operator");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_OPERATOR);
    assert(strcmp(tl_get(&tokens, 0)->lexeme, "+") == 0);
    tl_free(&tokens);
    
    test_scan_string("+ > * =", &tokens, "multiple operators");
    assert(tl_count(&tokens) == 4);
    assert(tl_get(&tokens, 0)->category == CAT_OPERATOR);
    assert(tl_get(&tokens, 1)->category == CAT_OPERATOR);
    assert(tl_get(&tokens, 2)->category == CAT_OPERATOR);
    assert(tl_get(&tokens, 3)->category == CAT_OPERATOR);
    tl_free(&tokens);
    
    printf("  Operators tests PASSED\n");
}

// Test 5: Special characters
static void test_special_chars(void) {
    printf("  Test 5: Special Characters\n");
    token_list_t tokens;
    
    test_scan_string("(", &tokens, "left paren");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_SPECIALCHAR);
    tl_free(&tokens);
    
    test_scan_string("( ) ; { }", &tokens, "multiple special chars");
    assert(tl_count(&tokens) == 5);
    for (int i = 0; i < 5; i++) {
        assert(tl_get(&tokens, i)->category == CAT_SPECIALCHAR);
    }
    tl_free(&tokens);
    
    printf("  Special Characters tests PASSED\n");
}

// Test 6: Mixed tokens (verifies parallel automata work correctly)
static void test_mixed_tokens(void) {
    printf("  Test 6: Mixed Tokens (Parallel Automata Test)\n");
    token_list_t tokens;
    
    test_scan_string("int x = 5;", &tokens, "variable declaration");
    assert(tl_count(&tokens) == 5);
    assert(tl_get(&tokens, 0)->category == CAT_KEYWORD);      // int
    assert(tl_get(&tokens, 1)->category == CAT_IDENTIFIER);   // x
    assert(tl_get(&tokens, 2)->category == CAT_OPERATOR);     // =
    assert(tl_get(&tokens, 3)->category == CAT_NUMBER);       // 5
    assert(tl_get(&tokens, 4)->category == CAT_SPECIALCHAR);  // ;
    tl_free(&tokens);
    
    test_scan_string("if (x > 0)", &tokens, "conditional expression");
    assert(tl_count(&tokens) == 6);
    assert(tl_get(&tokens, 0)->category == CAT_KEYWORD);      // if
    assert(tl_get(&tokens, 1)->category == CAT_SPECIALCHAR);  // (
    assert(tl_get(&tokens, 2)->category == CAT_IDENTIFIER);   // x
    assert(tl_get(&tokens, 3)->category == CAT_OPERATOR);     // >
    assert(tl_get(&tokens, 4)->category == CAT_NUMBER);       // 0
    assert(tl_get(&tokens, 5)->category == CAT_SPECIALCHAR);  // )
    tl_free(&tokens);
    
    test_scan_string("abc123", &tokens, "identifier with numbers (longest match)");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_IDENTIFIER);   // Should be one token, not abc + 123
    assert(strcmp(tl_get(&tokens, 0)->lexeme, "abc123") == 0);
    tl_free(&tokens);
    
    printf("  Mixed Tokens tests PASSED\n");
}

// Test 7: Edge cases
static void test_edge_cases(void) {
    printf("  Test 7: Edge Cases\n");
    token_list_t tokens;
    
    test_scan_string("123abc", &tokens, "number followed by identifier");
    assert(tl_count(&tokens) == 2);
    assert(tl_get(&tokens, 0)->category == CAT_NUMBER);      // 123
    assert(tl_get(&tokens, 1)->category == CAT_IDENTIFIER);  // abc
    tl_free(&tokens);
    
    test_scan_string("a1b2c3", &tokens, "alternating letters and digits");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_IDENTIFIER);  // Should be one identifier
    assert(strcmp(tl_get(&tokens, 0)->lexeme, "a1b2c3") == 0);
    tl_free(&tokens);
    
    test_scan_string("   123   ", &tokens, "number with whitespace");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_NUMBER);
    tl_free(&tokens);
    
    printf("  Edge Cases tests PASSED\n");
}

// Test 8: Longest match verification
static void test_longest_match(void) {
    printf("  Test 8: Longest Match (Maximal Munch)\n");
    token_list_t tokens;
    
    // This tests that identifier automata wins over just accepting first letter
    test_scan_string("hello", &tokens, "identifier longest match");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_IDENTIFIER);
    assert(strcmp(tl_get(&tokens, 0)->lexeme, "hello") == 0);
    assert(strlen(tl_get(&tokens, 0)->lexeme) == 5);  // All 5 chars
    tl_free(&tokens);
    
    // Number should consume all digits
    test_scan_string("9876543210", &tokens, "long number");
    assert(tl_count(&tokens) == 1);
    assert(tl_get(&tokens, 0)->category == CAT_NUMBER);
    assert(strlen(tl_get(&tokens, 0)->lexeme) == 10);
    tl_free(&tokens);
    
    printf("  Longest Match tests PASSED\n");
}

// Test 9: Real code snippet
static void test_real_code(void) {
    printf("  Test 9: Real Code Snippet\n");
    token_list_t tokens;
    
    const char *code = 
        "int main() {\n"
        "    return 0;\n"
        "}\n";
    
    test_scan_string(code, &tokens, "simple main function");
    
    // Should have: int main ( ) { return 0 ; }
    assert(tl_count(&tokens) >= 9);
    
    // Verify some key tokens
    int idx = 0;
    assert(tl_get(&tokens, idx++)->category == CAT_KEYWORD);      // int
    assert(tl_get(&tokens, idx++)->category == CAT_IDENTIFIER);   // main
    assert(tl_get(&tokens, idx++)->category == CAT_SPECIALCHAR);  // (
    assert(tl_get(&tokens, idx++)->category == CAT_SPECIALCHAR);  // )
    assert(tl_get(&tokens, idx++)->category == CAT_SPECIALCHAR);  // {
    assert(tl_get(&tokens, idx++)->category == CAT_KEYWORD);      // return
    assert(tl_get(&tokens, idx++)->category == CAT_NUMBER);       // 0
    assert(tl_get(&tokens, idx++)->category == CAT_SPECIALCHAR);  // ;
    assert(tl_get(&tokens, idx++)->category == CAT_SPECIALCHAR);  // }
    
    tl_free(&tokens);
    printf("  Real Code tests PASSED\n");
}

// Main test runner
int main(void) {
    printf("\n========================================\n");
    printf("  PARALLEL AUTOMATA UNIT TESTS\n");
    printf("========================================\n\n");
    
    test_numbers();
    test_identifiers_and_keywords();
    test_literals();
    test_operators();
    test_special_chars();
    test_mixed_tokens();
    test_edge_cases();
    test_longest_match();
    test_real_code();
    
    printf("\n========================================\n");
    printf("  ALL TESTS PASSED! ✓\n");
    printf("========================================\n\n");
    
    return 0;
}
