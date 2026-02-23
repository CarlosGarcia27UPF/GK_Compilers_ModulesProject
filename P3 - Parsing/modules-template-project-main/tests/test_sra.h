/*
 * =============================================================================
 * test_sra.h
 * Constants and declarations for SRA engine integration tests.
 *
 * Author: [Team]
 * =============================================================================
 */

#ifndef TEST_SRA_H
#define TEST_SRA_H

#include <string.h>
#include "../tests/test_modules.h"
#include "../src/lang_spec/lang_spec.h"
#include "../src/token_loader/token_loader.h"
#include "../src/parser_stack/parser_stack.h"
#include "../src/sra/sra.h"

/* Output log for this test module */
#define SRA_TEST_LOG      "./test_sra.log"

/* Argument values for the expected-result mode */
#define SRA_EXPECT_ACCEPT "accept"  /* Third argv: expect parse to succeed */
#define SRA_EXPECT_ERROR  "error"   /* Third argv: expect parse to fail    */

/* Function prototypes */
void test_sra_accept(const char *cscn_file, const char *lang_file);
void test_sra_error(const char *cscn_file, const char *lang_file);

#endif /* TEST_SRA_H */
