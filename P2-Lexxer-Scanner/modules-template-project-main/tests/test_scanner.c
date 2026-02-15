/*
 * -----------------------------------------------------------------------------
 * test_scanner.c
 *
 * Autonomous test program for the scanner (lexical analysis) modules.
 * Tests the lang_spec, char_stream, token, token_list, automata, and
 * out_writer modules.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "test_scanner.h"

/* ---- Test: Language Specification ---- */

/*
 * test_lang_spec - verifies char classification helpers used by automata.
 */
static void test_lang_spec(void) {
    printf("  Testing lang_spec...\n");

    /* Character classification */
    assert(ls_is_letter('a') == 1);
    assert(ls_is_letter('Z') == 1);
    assert(ls_is_letter('5') == 0);
    assert(ls_is_digit('0') == 1);
    assert(ls_is_digit('9') == 1);
    assert(ls_is_digit('a') == 0);
    assert(ls_is_operator(OP_ASSIGN) == 1);
    assert(ls_is_operator(OP_GT) == 1);
    assert(ls_is_operator(OP_PLUS) == 1);
    assert(ls_is_operator(OP_STAR) == 1);
    assert(ls_is_operator('x') == 0);
    assert(ls_is_special_char(SC_LPAREN) == 1);
    assert(ls_is_special_char(SC_SEMICOLON) == 1);
    assert(ls_is_special_char('x') == 0);
    assert(ls_is_quote(LIT_QUOTE) == 1);
    assert(ls_is_quote('a') == 0);

    printf("  lang_spec tests PASSED\n");
}

/* ---- Test: Token and Token List ---- */

/*
 * test_token_list - verifies token creation and list operations.
 */
static void test_token_list(void) {
    token_list_t list;
    token_t tok;
    const token_t *retrieved;

    printf("  Testing token + token_list...\n");

    tl_init(&list);
    assert(tl_count(&list) == 0);

    token_init(&tok, "hello", CAT_IDENTIFIER, 1, 1);
    tl_add(&list, &tok);
    assert(tl_count(&list) == 1);

    retrieved = tl_get(&list, 0);
    assert(retrieved != NULL);
    assert(retrieved->category == CAT_IDENTIFIER);
    assert(retrieved->line == 1);

    /* Out of bounds returns NULL */
    assert(tl_get(&list, 5) == NULL);

    tl_free(&list);
    assert(tl_count(&list) == 0);

    printf("  token + token_list tests PASSED\n");
}

/* ---- Test: Scanner with test input file ---- */

/*
 * write_test_file - creates a temporary test input file.
 */
static void write_test_file(void) {
    FILE *fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "if(x > 3)\n");
    fprintf(fp, " printf(\"true\");\n");
    fprintf(fp, "else\n");
    fprintf(fp, " printf(\"false\");\n");
    fclose(fp);
}

/*
 * test_scanner_scan - runs the full scanner on the test input and
 * verifies the token count and categories.
 */
static void test_scanner_scan(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    int result;

    printf("  Testing scanner (automata)...\n");

    write_test_file();

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);

    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);

    cs_close(&cs);

    /* Verify token count */
    assert(tl_count(&tokens) == TEST_BASIC_EXPECTED_TOKENS);

    /* Verify first token: "if" should be CAT_KEYWORD */
    tok = tl_get(&tokens, 0);
    assert(tok != NULL);
    assert(tok->category == CAT_KEYWORD);

    /* Verify "(" token: should be CAT_SPECIALCHAR */
    tok = tl_get(&tokens, 1);
    assert(tok != NULL);
    assert(tok->category == CAT_SPECIALCHAR);

    /* Verify "x" token: should be CAT_IDENTIFIER */
    tok = tl_get(&tokens, 2);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);

    /* Verify ">" token: should be CAT_OPERATOR */
    tok = tl_get(&tokens, 3);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);

    /* Verify "3" token: should be CAT_NUMBER */
    tok = tl_get(&tokens, 4);
    assert(tok != NULL);
    assert(tok->category == CAT_NUMBER);

    /* Verify literal "true" token */
    tok = tl_get(&tokens, 8);
    assert(tok != NULL);
    assert(tok->category == CAT_LITERAL);

    /* Verify "else" keyword */
    tok = tl_get(&tokens, 11);
    assert(tok != NULL);
    assert(tok->category == CAT_KEYWORD);

    tl_free(&tokens);

    printf("  scanner tests PASSED\n");
}

/*
 * test_output_filename - verifies the output filename construction.
 */
static void test_output_filename(void) {
    char buf[256];

    printf("  Testing output filename construction...\n");

    ow_build_output_filename("example.c", buf, 256);
    assert(strcmp(buf, "example.cscn") == 0);

    ow_build_output_filename("test/path/file.c", buf, 256);
    assert(strcmp(buf, "test/path/file.cscn") == 0);

    printf("  output filename tests PASSED\n");
}

/*
 * test_output_writer - verifies the .cscn file is written correctly.
 */
static void test_output_writer(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    char output_filename[256];
    FILE *fp;
    int result;

    printf("  Testing output writer...\n");

    write_test_file();
    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    automata_scan(&cs, &tokens, &lg, &cnt);
    cs_close(&cs);

    ow_build_output_filename(TEST_INPUT_FILE, output_filename, 256);
    result = ow_write_token_file(&tokens, output_filename);
    assert(result == 0);

    /* Verify the file exists and is non-empty */
    fp = fopen(output_filename, "r");
    assert(fp != NULL);
    fseek(fp, 0, SEEK_END);
    assert(ftell(fp) > 0);
    fclose(fp);

    tl_free(&tokens);

    printf("  output writer tests PASSED\n");
}

/* ---- Test: Blank lines in input ---- */

/*
 * test_blank_lines - verifies that blank lines in the input do not
 * produce tokens, and tokens on surrounding lines are correct.
 */
static void test_blank_lines(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    FILE *fp;
    int result;

    printf("  Testing blank lines in input...\n");

    /* Write input with blank lines */
    fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "int x;\n");
    fprintf(fp, "\n");
    fprintf(fp, "\n");
    fprintf(fp, "return 0;\n");
    fclose(fp);

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);
    cs_close(&cs);

    /* Expected tokens: int x ; return 0 ; = 6 */
    assert(tl_count(&tokens) == 6);

    /* "int" is keyword on line 1 */
    tok = tl_get(&tokens, 0);
    assert(tok != NULL);
    assert(tok->category == CAT_KEYWORD);
    assert(tok->line == 1);

    /* "return" is keyword on line 4 (after 2 blank lines) */
    tok = tl_get(&tokens, 3);
    assert(tok != NULL);
    assert(tok->category == CAT_KEYWORD);
    assert(tok->line == 4);

    /* "0" is number on line 4 */
    tok = tl_get(&tokens, 4);
    assert(tok != NULL);
    assert(tok->category == CAT_NUMBER);

    tl_free(&tokens);

    printf("  blank lines tests PASSED\n");
}

/* ---- Test: Unterminated literal ---- */

/*
 * test_unterminated_literal - verifies that an unterminated literal
 * (missing closing quote) produces a NONRECOGNIZED token.
 */
static void test_unterminated_literal(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    FILE *fp;
    int result;

    printf("  Testing unterminated literal...\n");

    /* Write input with unterminated literal */
    fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "x = \"hello\n");
    fprintf(fp, "y = 5;\n");
    fclose(fp);

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);
    cs_close(&cs);

    /* Verify we got tokens and the unterminated literal is NONRECOGNIZED */
    assert(tl_count(&tokens) >= 3);

    /* Token 0: "x" IDENTIFIER */
    tok = tl_get(&tokens, 0);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);

    /* Token 1: "=" OPERATOR */
    tok = tl_get(&tokens, 1);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);

    /* Token 2: unterminated literal → NONRECOGNIZED */
    tok = tl_get(&tokens, 2);
    assert(tok != NULL);
    assert(tok->category == CAT_NONRECOGNIZED);

    tl_free(&tokens);

    printf("  unterminated literal tests PASSED\n");
}

/* ---- Test: Grouped nonrecognized characters ---- */

/*
 * test_grouped_nonrecognized - verifies that consecutive non-recognized
 * characters are grouped into a single NONRECOGNIZED token.
 */
static void test_grouped_nonrecognized(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    FILE *fp;
    int result;

    printf("  Testing grouped nonrecognized...\n");

    /* Write input with consecutive non-recognized chars */
    fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "x = @#$ + y;\n");
    fclose(fp);

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);
    cs_close(&cs);

    /* Expected: x = @#$ + y ; = 6 tokens */
    assert(tl_count(&tokens) == 6);

    /* Token 0: "x" IDENTIFIER */
    tok = tl_get(&tokens, 0);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);

    /* Token 1: "=" OPERATOR */
    tok = tl_get(&tokens, 1);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);

    /* Token 2: "@#$" grouped NONRECOGNIZED */
    tok = tl_get(&tokens, 2);
    assert(tok != NULL);
    assert(tok->category == CAT_NONRECOGNIZED);

    /* Token 3: "+" OPERATOR */
    tok = tl_get(&tokens, 3);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);

    /* Token 4: "y" IDENTIFIER */
    tok = tl_get(&tokens, 4);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);

    /* Token 5: ";" SPECIALCHAR */
    tok = tl_get(&tokens, 5);
    assert(tok != NULL);
    assert(tok->category == CAT_SPECIALCHAR);

    tl_free(&tokens);

    printf("  grouped nonrecognized tests PASSED\n");
}

/* ---- Test: Correct output filenames ---- */

/*
 * test_output_filenames_extended - verifies output filename construction
 * for various input patterns including the dbgcnt suffix.
 */
static void test_output_filenames_extended(void) {
    char buf[256];

    printf("  Testing output filenames (extended)...\n");

    /* example.c -> example.cscn */
    ow_build_output_filename("example.c", buf, 256);
    assert(strcmp(buf, "example.cscn") == 0);

    /* path/to/file.c -> path/to/file.cscn */
    ow_build_output_filename("path/to/file.c", buf, 256);
    assert(strcmp(buf, "path/to/file.cscn") == 0);

    /* simple.c -> simple.cscn */
    ow_build_output_filename("simple.c", buf, 256);
    assert(strcmp(buf, "simple.cscn") == 0);

    /* example.c -> example.cdbgcnt */
    ow_build_count_filename("example.c", buf, 256);
    assert(strcmp(buf, "example.cdbgcnt") == 0);

    printf("  output filenames (extended) tests PASSED\n");
}

/* ---- Test: COUNTIO counts characters ---- */

/*
 * test_countio - verifies that the counter system counts I/O operations
 * (characters read) during scanning.
 */
static void test_countio(void) {
    counter_t cnt;

    printf("  Testing COUNTIO counts characters...\n");

    counter_init(&cnt);
    assert(cnt.io == 0);
    assert(cnt.comp == 0);
    assert(cnt.gen == 0);

    /* Simulate counting */
    counter_add_io(&cnt, 5);
    assert(cnt.io == 5);

    counter_add_comp(&cnt, 3);
    assert(cnt.comp == 3);

    counter_add_gen(&cnt, 2);
    assert(cnt.gen == 2);

    /* Accumulation */
    counter_add_io(&cnt, 10);
    assert(cnt.io == 15);

    printf("  COUNTIO tests PASSED\n");
}

/* ---- Test: NULL counter pointer safety ---- */

/*
 * test_null_counter_pointer - verifies scanning works when counter pointer
 * is NULL (COUNT macros must be NULL-safe).
 */
static void test_null_counter_pointer(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    int result;

    printf("  Testing NULL counter pointer safety...\n");

    write_test_file();
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, NULL);
    assert(result == 0);
    cs_close(&cs);

    assert(tl_count(&tokens) == TEST_BASIC_EXPECTED_TOKENS);
    tl_free(&tokens);

    printf("  NULL counter pointer tests PASSED\n");
}

/* ---- Test: All Keywords ---- */

/*
 * test_all_keywords - verifies that all 5 keywords are recognized
 * when used together in realistic C-like code.
 */
static void test_all_keywords(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    FILE *fp;
    int result, i;
    int keyword_count = 0;

    printf("  Testing all keywords...\n");

    fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "int x;\n");
    fprintf(fp, "if (1) return; else while (0);\n");
    fclose(fp);

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);
    cs_close(&cs);

    /* Count how many keyword tokens we got */
    for (i = 0; i < tl_count(&tokens); i++) {
        tok = tl_get(&tokens, i);
        if (tok->category == CAT_KEYWORD) {
            keyword_count++;
        }
    }

    /* Must find all 5 keywords: int, if, return, else, while */
    assert(keyword_count == 5);

    tl_free(&tokens);

    printf("  all keywords tests PASSED\n");
}

/* ---- Test: Keyword Lookalikes (typos) ---- */

/*
 * test_keyword_typos - verifies that misspelled keywords like
 * "wile", "iff", "intt" are classified as IDENTIFIER, not KEYWORD.
 */
static void test_keyword_typos(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    FILE *fp;
    int result, i;

    printf("  Testing keyword typos/lookalikes...\n");

    fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "wile notKeyword;\n");
    fprintf(fp, "iff alsoNot;\n");
    fprintf(fp, "intt stillNot;\n");
    fprintf(fp, "returns bad;\n");
    fclose(fp);

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);
    cs_close(&cs);

    /* Every identifier-like token must be CAT_IDENTIFIER, never CAT_KEYWORD */
    for (i = 0; i < tl_count(&tokens); i++) {
        tok = tl_get(&tokens, i);
        assert(tok->category != CAT_KEYWORD);
    }

    /* Specifically verify the typo tokens are identifiers */
    /* Line 1: wile notKeyword ; */
    tok = tl_get(&tokens, 0);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);
    assert(strcmp(tok->lexeme, "wile") == 0);

    tok = tl_get(&tokens, 1);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);
    assert(strcmp(tok->lexeme, "notKeyword") == 0);

    /* Line 2: iff alsoNot ; */
    tok = tl_get(&tokens, 3);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);
    assert(strcmp(tok->lexeme, "iff") == 0);

    /* Line 3: intt stillNot ; */
    tok = tl_get(&tokens, 6);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);
    assert(strcmp(tok->lexeme, "intt") == 0);

    tl_free(&tokens);

    printf("  keyword typos tests PASSED\n");
}

/* ---- Test: Numbers with Leading Zeros ---- */

/*
 * test_numbers_leading_zeros - verifies that numbers with leading zeros
 * (007, 000) and large numbers are all recognized as CAT_NUMBER.
 */
static void test_numbers_leading_zeros(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    FILE *fp;
    int result;

    printf("  Testing numbers with leading zeros...\n");

    fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "int x = 007;\n");
    fprintf(fp, "int y = 000;\n");
    fprintf(fp, "int z = 123456789;\n");
    fprintf(fp, "int w = 0;\n");
    fclose(fp);

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);
    cs_close(&cs);

    /* Line 1: int x = 007 ; -> tokens 0..4 */
    tok = tl_get(&tokens, 3);
    assert(tok != NULL);
    assert(tok->category == CAT_NUMBER);
    assert(strcmp(tok->lexeme, "007") == 0);

    /* Line 2: int y = 000 ; -> tokens 5..9 */
    tok = tl_get(&tokens, 8);
    assert(tok != NULL);
    assert(tok->category == CAT_NUMBER);
    assert(strcmp(tok->lexeme, "000") == 0);

    /* Line 3: int z = 123456789 ; -> tokens 10..14 */
    tok = tl_get(&tokens, 13);
    assert(tok != NULL);
    assert(tok->category == CAT_NUMBER);
    assert(strcmp(tok->lexeme, "123456789") == 0);

    /* Line 4: int w = 0 ; -> tokens 15..19 */
    tok = tl_get(&tokens, 18);
    assert(tok != NULL);
    assert(tok->category == CAT_NUMBER);
    assert(strcmp(tok->lexeme, "0") == 0);

    tl_free(&tokens);

    printf("  numbers with leading zeros tests PASSED\n");
}

/* ---- Test: All Operators Together ---- */

/*
 * test_all_operators - verifies that all four operators (=, >, +, *)
 * are correctly recognized when used in a single expression.
 */
static void test_all_operators(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    FILE *fp;
    int result;

    printf("  Testing all operators together...\n");

    fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "x = y + z * a > b;\n");
    fclose(fp);

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);
    cs_close(&cs);

    /* Expected: x = y + z * a > b ; = 10 tokens */
    assert(tl_count(&tokens) == 10);

    /* "=" operator */
    tok = tl_get(&tokens, 1);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);
    assert(tok->lexeme[0] == OP_ASSIGN);

    /* "+" operator */
    tok = tl_get(&tokens, 3);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);
    assert(tok->lexeme[0] == OP_PLUS);

    /* "*" operator */
    tok = tl_get(&tokens, 5);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);
    assert(tok->lexeme[0] == OP_STAR);

    /* ">" operator */
    tok = tl_get(&tokens, 7);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);
    assert(tok->lexeme[0] == OP_GT);

    tl_free(&tokens);

    printf("  all operators tests PASSED\n");
}

/* ---- Test: Arrays and Pointers ---- */

/*
 * test_arrays_pointers - verifies that array bracket syntax and
 * pointer star syntax are correctly tokenized.
 */
static void test_arrays_pointers(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    FILE *fp;
    int result, i;
    int found_lbracket = 0, found_rbracket = 0, found_star = 0;

    printf("  Testing arrays and pointers...\n");

    fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "int arr[10];\n");
    fprintf(fp, "char* ptr;\n");
    fprintf(fp, "void** pptr;\n");
    fclose(fp);

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);
    cs_close(&cs);

    /* Verify brackets and stars are found */
    for (i = 0; i < tl_count(&tokens); i++) {
        tok = tl_get(&tokens, i);
        if (tok->category == CAT_SPECIALCHAR && tok->lexeme[0] == SC_LBRACKET)
            found_lbracket = 1;
        if (tok->category == CAT_SPECIALCHAR && tok->lexeme[0] == SC_RBRACKET)
            found_rbracket = 1;
        if (tok->category == CAT_OPERATOR && tok->lexeme[0] == OP_STAR)
            found_star = 1;
    }

    assert(found_lbracket == 1);
    assert(found_rbracket == 1);
    assert(found_star == 1);

    /* Line 1: int arr [ 10 ] ; = 6 tokens */
    tok = tl_get(&tokens, 0);
    assert(tok != NULL);
    assert(tok->category == CAT_KEYWORD);  /* int */

    tok = tl_get(&tokens, 1);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);  /* arr */

    tok = tl_get(&tokens, 2);
    assert(tok != NULL);
    assert(tok->category == CAT_SPECIALCHAR);  /* [ */

    tok = tl_get(&tokens, 3);
    assert(tok != NULL);
    assert(tok->category == CAT_NUMBER);  /* 10 */

    tok = tl_get(&tokens, 4);
    assert(tok != NULL);
    assert(tok->category == CAT_SPECIALCHAR);  /* ] */

    tok = tl_get(&tokens, 5);
    assert(tok != NULL);
    assert(tok->category == CAT_SPECIALCHAR);  /* ; */

    /* Line 2: char * ptr ; (char is a supported type keyword) */
    tok = tl_get(&tokens, 6);
    assert(tok != NULL);
    assert(tok->category == CAT_KEYWORD);  /* char */

    tok = tl_get(&tokens, 7);
    assert(tok != NULL);
    assert(tok->category == CAT_OPERATOR);  /* * */

    tok = tl_get(&tokens, 8);
    assert(tok != NULL);
    assert(tok->category == CAT_IDENTIFIER);  /* ptr */

    tl_free(&tokens);

    printf("  arrays and pointers tests PASSED\n");
}

/* ---- Test: Complex Literals ---- */

/*
 * test_complex_literals - verifies that various literal forms are
 * correctly recognized: empty literals, literals with spaces,
 * and literals with special characters.
 */
static void test_complex_literals(void) {
    char_stream_t cs;
    token_list_t tokens;
    logger_t lg;
    counter_t cnt;
    const token_t *tok;
    FILE *fp;
    int result, i;
    int literal_count = 0;

    printf("  Testing complex literals...\n");

    fp = fopen(TEST_INPUT_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "char* s1 = \"\";\n");                        /* empty literal */
    fprintf(fp, "char* s2 = \"hello world\";\n");              /* spaces */
    fprintf(fp, "char* s3 = \"symbols!@#$%%^&*()\";\n");      /* special chars */
    fprintf(fp, "char* s4 = \"12345\";\n");                    /* digits inside */
    fclose(fp);

    counter_init(&cnt);
    tl_init(&tokens);
    logger_init(&lg, stdout, NULL);

    result = cs_open(&cs, TEST_INPUT_FILE);
    assert(result == 0);
    result = automata_scan(&cs, &tokens, &lg, &cnt);
    assert(result == 0);
    cs_close(&cs);

    /* Count all literal tokens */
    for (i = 0; i < tl_count(&tokens); i++) {
        tok = tl_get(&tokens, i);
        if (tok->category == CAT_LITERAL) {
            literal_count++;
        }
    }

    /* Should have exactly 4 literals */
    assert(literal_count == 4);

    tl_free(&tokens);

    printf("  complex literals tests PASSED\n");
}

/* ---- Main ---- */

int main(void) {
    printf("Running scanner tests...\n");

    test_lang_spec();
    test_token_list();
    test_scanner_scan();
    test_output_filename();
    test_output_writer();
    test_blank_lines();
    test_unterminated_literal();
    test_grouped_nonrecognized();
    test_output_filenames_extended();
    test_countio();
    test_null_counter_pointer();
    test_all_keywords();
    test_keyword_typos();
    test_numbers_leading_zeros();
    test_all_operators();
    test_arrays_pointers();
    test_complex_literals();

    printf("All scanner tests PASSED!\n");
    return 0;
}
