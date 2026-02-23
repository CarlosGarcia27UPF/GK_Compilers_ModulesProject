/*
 * =============================================================================
 * test_parser_stack.c
 * Unit tests for the parser_stack module.
 *
 * Author: [Team]
 * =============================================================================
 */

#include "../tests/test_parser_stack.h"

FILE *ofile = NULL; /* Required by utils_files.c */

/* Test that ps_init creates a stack with the bottom-of-stack sentinel. */
void test_ps_init(void)
{
    parser_stack_t stack;
    ps_init(&stack);
    assert(!ps_is_empty(&stack));
    assert(ps_top_state(&stack) == 0);
    fprintf(ofile, "test_ps_init: PASSED\n");
}

/* Test push and pop operations. */
void test_ps_push_pop(void)
{
    parser_stack_t stack;
    ps_init(&stack);

    ps_push(&stack, PS_TEST_STATE1, PS_TEST_SYM1, "t", "val");
    assert(ps_top_state(&stack) == PS_TEST_STATE1);

    ps_push(&stack, PS_TEST_STATE2, 1, "e", "");
    assert(ps_top_state(&stack) == PS_TEST_STATE2);

    ps_pop(&stack);
    assert(ps_top_state(&stack) == PS_TEST_STATE1);

    ps_pop(&stack);
    assert(ps_top_state(&stack) == 0); /* back to initial sentinel */

    fprintf(ofile, "test_ps_push_pop: PASSED\n");
}

/* Test ps_top_state returns the current top state correctly. */
void test_ps_top_state(void)
{
    parser_stack_t stack;
    ps_init(&stack);
    assert(ps_top_state(&stack) == 0);

    ps_push(&stack, 7, 2, "f", "num");
    assert(ps_top_state(&stack) == 7);
    fprintf(ofile, "test_ps_top_state: PASSED\n");
}

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    ofile = stdout;
    ofile = set_output_test_file(PARSER_STACK_TEST_LOG);

    fprintf(ofile, "=== test_parser_stack starting ===\n");

    test_ps_init();
    test_ps_push_pop();
    test_ps_top_state();

    fprintf(ofile, "=== test_parser_stack: ALL PASSED ===\n");
    printf("test_parser_stack: ALL PASSED\n");

    fclose(ofile);
    return 0;
}
