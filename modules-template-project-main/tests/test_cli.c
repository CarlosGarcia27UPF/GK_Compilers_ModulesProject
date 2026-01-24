/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description: Unit tests for CLI argument parsing.
 * -------------------------------------------------------------------------- */

#include "cli/cli.h"
#include "spec/pp_spec.h"

#include <assert.h>
#include <stdio.h>

/* Test program name used in argv vectors. */
#define TEST_PROGNAME "pp"
/* Test input file used in argv vectors. */
#define TEST_INPUT_FILE "input.c"

/* Verify default behavior: no flags => comments enabled. */
static void test_cli_default_no_flags(void)
{
    char *argv[] = {TEST_PROGNAME, TEST_INPUT_FILE, 0};
    int argc = 2;

    cli_options_t opt = cli_parse(argc, argv);
    assert(opt.do_comments == 1);
    assert(opt.do_directives == 0);
    assert(opt.do_help == 0);
}

/* Verify -c enables comment removal only. */
static void test_cli_flag_c_only(void)
{
    char *argv[] = {TEST_PROGNAME, PP_FLAG_C, TEST_INPUT_FILE, 0};
    int argc = 3;

    cli_options_t opt = cli_parse(argc, argv);
    assert(opt.do_comments == 1);
    assert(opt.do_directives == 0);
    assert(opt.do_help == 0);
}

/* Verify -d enables directive processing only. */
static void test_cli_flag_d_only(void)
{
    char *argv[] = {TEST_PROGNAME, PP_FLAG_D, TEST_INPUT_FILE, 0};
    int argc = 3;

    cli_options_t opt = cli_parse(argc, argv);
    assert(opt.do_comments == 0);
    assert(opt.do_directives == 1);
    assert(opt.do_help == 0);
}

/* Verify -all enables both comment and directive processing. */
static void test_cli_flag_all(void)
{
    char *argv[] = {TEST_PROGNAME, PP_FLAG_ALL, TEST_INPUT_FILE, 0};
    int argc = 3;

    cli_options_t opt = cli_parse(argc, argv);
    assert(opt.do_comments == 1);
    assert(opt.do_directives == 1);
    assert(opt.do_help == 0);
}

/* Verify multiple flags enable their combined behavior. */
static void test_cli_flag_combo(void)
{
    char *argv[] = {TEST_PROGNAME, PP_FLAG_C, PP_FLAG_D, TEST_INPUT_FILE, 0};
    int argc = 4;

    cli_options_t opt = cli_parse(argc, argv);
    assert(opt.do_comments == 1);
    assert(opt.do_directives == 1);
    assert(opt.do_help == 0);
}

/* Verify -help is detected regardless of other flags. */
static void test_cli_flag_help(void)
{
    char *argv[] = {TEST_PROGNAME, PP_FLAG_HELP, PP_FLAG_D, TEST_INPUT_FILE, 0};
    int argc = 4;

    cli_options_t opt = cli_parse(argc, argv);
    assert(opt.do_help == 1);
}

int main(void)
{
    printf("=== CLI Module Test Suite ===\n\n");

    test_cli_default_no_flags();
    test_cli_flag_c_only();
    test_cli_flag_d_only();
    test_cli_flag_all();
    test_cli_flag_combo();
    test_cli_flag_help();

    printf("=== All CLI tests passed! ===\n\n");
    return 0;
}
