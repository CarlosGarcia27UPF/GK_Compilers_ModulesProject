/**
 * @file test_scanner_new.c
 * @brief Unit tests for P2 Scanner/Lexer using new module structure
 *
 * Tests the integrated scanner functionality through the driver,
 * automata, and supporting modules.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/lang_spec/lang_spec.h"
#include "../src/char_stream/char_stream.h"
#include "../src/token/token.h"
#include "../src/token_list/token_list.h"
#include "../src/automata/automata.h"
#include "../src/driver/driver.h"

/* ========================================================================== */
/*                              TEST UTILITIES                                 */
/* ========================================================================== */

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, msg) do { \
    if (!(condition)) { \
        printf("  FAILED: %s\n", msg); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define TEST_PASS(msg) do { \
    printf("  PASSED: %s\n", msg); \
    tests_passed++; \
} while(0)

/* ========================================================================== */
/*                           LANG_SPEC TESTS                                   */
/* ========================================================================== */

void test_lang_is_digit(void) {
    printf("\n--- test_lang_is_digit ---\n");
    
    TEST_ASSERT(lang_is_digit('0'), "0 should be digit");
    TEST_ASSERT(lang_is_digit('5'), "5 should be digit");
    TEST_ASSERT(lang_is_digit('9'), "9 should be digit");
    TEST_ASSERT(!lang_is_digit('a'), "a should not be digit");
    TEST_ASSERT(!lang_is_digit(' '), "space should not be digit");
    
    TEST_PASS("digit classification");
}

void test_lang_is_letter(void) {
    printf("\n--- test_lang_is_letter ---\n");
    
    TEST_ASSERT(lang_is_letter('a'), "a should be letter");
    TEST_ASSERT(lang_is_letter('Z'), "Z should be letter");
    TEST_ASSERT(lang_is_letter('_'), "_ should be letter");
    TEST_ASSERT(!lang_is_letter('0'), "0 should not be letter");
    TEST_ASSERT(!lang_is_letter(' '), "space should not be letter");
    
    TEST_PASS("letter classification");
}

void test_lang_is_keyword(void) {
    printf("\n--- test_lang_is_keyword ---\n");
    
    TEST_ASSERT(lang_is_keyword("if"), "'if' should be keyword");
    TEST_ASSERT(lang_is_keyword("while"), "'while' should be keyword");
    TEST_ASSERT(lang_is_keyword("return"), "'return' should be keyword");
    TEST_ASSERT(!lang_is_keyword("foo"), "'foo' should not be keyword");
    TEST_ASSERT(!lang_is_keyword("IF"), "'IF' should not be keyword (case sensitive)");
    
    TEST_PASS("keyword detection");
}

/* ========================================================================== */
/*                          CHAR_STREAM TESTS                                  */
/* ========================================================================== */

void test_char_stream_basic(void) {
    printf("\n--- test_char_stream_basic ---\n");
    
    CharStream cs;
    const char* input = "abc";
    cs_init_string(&cs, input, (int)strlen(input));
    
    TEST_ASSERT(cs_peek(&cs) == 'a', "peek should return 'a'");
    TEST_ASSERT(cs_get(&cs) == 'a', "get should return 'a'");
    TEST_ASSERT(cs_peek(&cs) == 'b', "peek should return 'b'");
    TEST_ASSERT(cs_get(&cs) == 'b', "get should return 'b'");
    TEST_ASSERT(cs_get(&cs) == 'c', "get should return 'c'");
    TEST_ASSERT(cs_is_eof(&cs), "should be at EOF");
    TEST_ASSERT(cs_peek(&cs) == '\0', "peek at EOF should return '\\0'");
    
    TEST_PASS("char stream navigation");
}

void test_char_stream_position(void) {
    printf("\n--- test_char_stream_position ---\n");
    
    CharStream cs;
    const char* input = "ab\ncd";
    cs_init_string(&cs, input, (int)strlen(input));
    
    TEST_ASSERT(cs_get_line(&cs) == 1, "initial line should be 1");
    TEST_ASSERT(cs_get_column(&cs) == 1, "initial column should be 1");
    
    cs_get(&cs); // 'a'
    TEST_ASSERT(cs_get_column(&cs) == 2, "column should be 2 after 'a'");
    
    cs_get(&cs); // 'b'
    cs_get(&cs); // '\n'
    TEST_ASSERT(cs_get_line(&cs) == 2, "line should be 2 after newline");
    TEST_ASSERT(cs_get_column(&cs) == 1, "column should be 1 on new line");
    
    TEST_PASS("position tracking");
}

/* ========================================================================== */
/*                             TOKEN TESTS                                     */
/* ========================================================================== */

void test_token_create(void) {
    printf("\n--- test_token_create ---\n");
    
    Token t = token_create("hello", CAT_IDENTIFIER, 1);
    
    TEST_ASSERT(strcmp(t.lexeme, "hello") == 0, "lexeme should be 'hello'");
    TEST_ASSERT(t.category == CAT_IDENTIFIER, "category should be CAT_IDENTIFIER");
    TEST_ASSERT(t.line == 1, "line should be 1");
    TEST_ASSERT(t.keyword == KW_NONE, "should not be keyword");
    
    TEST_PASS("token creation");
}

void test_token_keyword(void) {
    printf("\n--- test_token_keyword ---\n");
    
    Token t = token_create("if", CAT_KEYWORD, 5);
    
    TEST_ASSERT(t.category == CAT_KEYWORD, "category should be CAT_KEYWORD");
    // keyword field is set by automata, token_create just copies lexeme
    
    TEST_PASS("keyword token");
}

/* ========================================================================== */
/*                          TOKEN_LIST TESTS                                   */
/* ========================================================================== */

void test_token_list_basic(void) {
    printf("\n--- test_token_list_basic ---\n");
    
    TokenList list;
    token_list_init(&list);
    
    TEST_ASSERT(token_list_count(&list) == 0, "initial count should be 0");
    
    Token t1 = token_create("x", CAT_IDENTIFIER, 1);
    Token t2 = token_create("42", CAT_NUMBER, 1);
    
    token_list_add(&list, t1);
    token_list_add(&list, t2);
    
    TEST_ASSERT(token_list_count(&list) == 2, "count should be 2");
    
    Token* got = token_list_get(&list, 0);
    TEST_ASSERT(got != NULL, "get(0) should not be NULL");
    TEST_ASSERT(strcmp(got->lexeme, "x") == 0, "first token should be 'x'");
    
    got = token_list_get(&list, 1);
    TEST_ASSERT(got != NULL, "get(1) should not be NULL");
    TEST_ASSERT(strcmp(got->lexeme, "42") == 0, "second token should be '42'");
    
    token_list_free(&list);
    
    TEST_PASS("token list operations");
}

/* ========================================================================== */
/*                           AUTOMATA TESTS                                    */
/* ========================================================================== */

void test_automata_number(void) {
    printf("\n--- test_automata_number ---\n");
    
    CharStream cs;
    Token t;
    const char* input = "12345";
    cs_init_string(&cs, input, (int)strlen(input));
    
    ScanResult result = automata_scan_next(&cs, &t);
    
    TEST_ASSERT(result == SCAN_OK, "scan should succeed");
    TEST_ASSERT(t.category == CAT_NUMBER, "should recognize number");
    TEST_ASSERT(strcmp(t.lexeme, "12345") == 0, "lexeme should be '12345'");
    
    TEST_PASS("number recognition");
}

void test_automata_identifier(void) {
    printf("\n--- test_automata_identifier ---\n");
    
    CharStream cs;
    Token t;
    const char* input = "myVar123";
    cs_init_string(&cs, input, (int)strlen(input));
    
    ScanResult result = automata_scan_next(&cs, &t);
    
    TEST_ASSERT(result == SCAN_OK, "scan should succeed");
    TEST_ASSERT(t.category == CAT_IDENTIFIER, "should recognize identifier");
    TEST_ASSERT(strcmp(t.lexeme, "myVar123") == 0, "lexeme should be 'myVar123'");
    
    TEST_PASS("identifier recognition");
}

void test_automata_keyword(void) {
    printf("\n--- test_automata_keyword ---\n");
    
    CharStream cs;
    Token t;
    const char* input = "while";
    cs_init_string(&cs, input, (int)strlen(input));
    
    ScanResult result = automata_scan_next(&cs, &t);
    
    TEST_ASSERT(result == SCAN_OK, "scan should succeed");
    TEST_ASSERT(t.category == CAT_KEYWORD, "should recognize keyword");
    TEST_ASSERT(strcmp(t.lexeme, "while") == 0, "lexeme should be 'while'");
    TEST_ASSERT(t.keyword == KW_WHILE, "keyword type should be KW_WHILE");
    
    TEST_PASS("keyword recognition");
}

void test_automata_literal(void) {
    printf("\n--- test_automata_literal ---\n");
    
    CharStream cs;
    Token t;
    const char* input = "\"hello world\"";
    cs_init_string(&cs, input, (int)strlen(input));
    
    ScanResult result = automata_scan_next(&cs, &t);
    
    TEST_ASSERT(result == SCAN_OK, "scan should succeed");
    TEST_ASSERT(t.category == CAT_LITERAL, "should recognize literal");
    // Note: lexeme includes quotes in our implementation
    TEST_ASSERT(strstr(t.lexeme, "hello world") != NULL, "lexeme should contain 'hello world'");
    
    TEST_PASS("literal recognition");
}

void test_automata_operator(void) {
    printf("\n--- test_automata_operator ---\n");
    
    CharStream cs;
    Token t;
    const char* input = "==";
    cs_init_string(&cs, input, (int)strlen(input));
    
    ScanResult result = automata_scan_next(&cs, &t);
    
    TEST_ASSERT(result == SCAN_OK, "scan should succeed");
    TEST_ASSERT(t.category == CAT_OPERATOR, "should recognize operator");
    TEST_ASSERT(strcmp(t.lexeme, "==") == 0, "lexeme should be '=='");
    
    TEST_PASS("operator recognition");
}

void test_automata_special(void) {
    printf("\n--- test_automata_special ---\n");
    
    CharStream cs;
    Token t;
    const char* input = "(";
    cs_init_string(&cs, input, (int)strlen(input));
    
    ScanResult result = automata_scan_next(&cs, &t);
    
    TEST_ASSERT(result == SCAN_OK, "scan should succeed");
    TEST_ASSERT(t.category == CAT_SPECIALCHAR, "should recognize special char");
    TEST_ASSERT(strcmp(t.lexeme, "(") == 0, "lexeme should be '('");
    
    TEST_PASS("special char recognition");
}

void test_automata_non_recognized(void) {
    printf("\n--- test_automata_non_recognized ---\n");
    
    CharStream cs;
    Token t;
    const char* input = "@";
    cs_init_string(&cs, input, (int)strlen(input));
    
    ScanResult result = automata_scan_next(&cs, &t);
    
    TEST_ASSERT(result == SCAN_OK, "scan should succeed");
    TEST_ASSERT(t.category == CAT_NONRECOGNIZED, "should mark as non-recognized");
    TEST_ASSERT(strcmp(t.lexeme, "@") == 0, "lexeme should be '@'");
    
    TEST_PASS("non-recognized char handling");
}

/* ========================================================================== */
/*                          INTEGRATION TESTS                                  */
/* ========================================================================== */

void test_full_scan_simple(void) {
    printf("\n--- test_full_scan_simple ---\n");
    
    CharStream cs;
    const char* input = "x = 42;";
    cs_init_string(&cs, input, (int)strlen(input));
    
    TokenList list;
    token_list_init(&list);
    
    Token t;
    ScanResult result;
    while (1) {
        result = automata_scan_next(&cs, &t);
        token_list_add(&list, t);
        if (t.category == CAT_EOF) break;
    }
    
    // Expected: x(IDENT), =(OP), 42(NUM), ;(SPECIAL), EOF
    TEST_ASSERT(token_list_count(&list) >= 4, "should have at least 4 tokens");
    
    Token* t0 = token_list_get(&list, 0);
    TEST_ASSERT(t0->category == CAT_IDENTIFIER, "first token should be identifier");
    
    token_list_free(&list);
    
    TEST_PASS("full scan simple expression");
}

void test_full_scan_with_keywords(void) {
    printf("\n--- test_full_scan_with_keywords ---\n");
    
    CharStream cs;
    const char* input = "if (x > 0) return 1;";
    cs_init_string(&cs, input, (int)strlen(input));
    
    TokenList list;
    token_list_init(&list);
    
    Token t;
    ScanResult result;
    int keyword_count = 0;
    while (1) {
        result = automata_scan_next(&cs, &t);
        if (t.category == CAT_KEYWORD) keyword_count++;
        token_list_add(&list, t);
        if (t.category == CAT_EOF) break;
    }
    
    TEST_ASSERT(keyword_count == 2, "should have 2 keywords (if, return)");
    
    token_list_free(&list);
    
    TEST_PASS("full scan with keywords");
}

/* ========================================================================== */
/*                              MAIN                                           */
/* ========================================================================== */

int main(void) {
    printf("=== P2 Scanner/Lexer Unit Tests ===\n");
    
    /* Lang spec tests */
    test_lang_is_digit();
    test_lang_is_letter();
    test_lang_is_keyword();
    
    /* Char stream tests */
    test_char_stream_basic();
    test_char_stream_position();
    
    /* Token tests */
    test_token_create();
    test_token_keyword();
    
    /* Token list tests */
    test_token_list_basic();
    
    /* Automata tests */
    test_automata_number();
    test_automata_identifier();
    test_automata_keyword();
    test_automata_literal();
    test_automata_operator();
    test_automata_special();
    test_automata_non_recognized();
    
    /* Integration tests */
    test_full_scan_simple();
    test_full_scan_with_keywords();
    
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    
    return (tests_failed > 0) ? 1 : 0;
}
