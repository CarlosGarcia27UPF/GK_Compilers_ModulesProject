/*
 * =============================================================================
 * test_dfa.h
 * Constants and declarations for DFA unit tests.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef TEST_DFA_H
#define TEST_DFA_H

#include "../tests/test_modules.h"
#include "../src/dfa/dfa.h"

/* Output log for this test module */
#define DFA_TEST_LOG  "./test_dfa.log"

/* Function prototypes */
void test_dfa_init(void);
void test_dfa_set_get_action(void);
void test_dfa_set_get_goto(void);
void test_dfa_bounds(void);

#endif /* TEST_DFA_H */
