/*
 * =============================================================================
 * test_parser_stack.h
 * Constants and declarations for parser_stack unit tests.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef TEST_PARSER_STACK_H
#define TEST_PARSER_STACK_H

#include "../tests/test_modules.h"
#include "../src/parser_stack/parser_stack.h"

/* Output log for this test module */
#define PARSER_STACK_TEST_LOG  "./test_parser_stack.log"

/* Test values */
#define PS_TEST_STATE1  5
#define PS_TEST_STATE2  9
#define PS_TEST_SYM1    3

/* Function prototypes */
void test_ps_init(void);
void test_ps_push_pop(void);
void test_ps_top_state(void);

#endif /* TEST_PARSER_STACK_H */
