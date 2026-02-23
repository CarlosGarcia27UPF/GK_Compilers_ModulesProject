/*
 * =============================================================================
 * main.h
 * Global declarations for the p3_parser program.
 *
 * Includes all module headers and declares shared global variables.
 * Author: [Team]
 * =============================================================================
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "./utils_files.h"
#include "./module_args/module_args.h"
#include "./lang_spec/lang_spec.h"
#include "./token_loader/token_loader.h"
#include "./parser_stack/parser_stack.h"
#include "./sra/sra.h"
#include "./out_writer/out_writer.h"

/* Output log filename for the main program run */
#define PROJOUTFILENAME "./proj_p3_parser.log"

extern FILE        *ofile;     /* Shared output log (defined in main.c)  */
extern config_t     g_config;  /* CLI arguments (defined in main.c)      */
extern lang_spec_t  g_lang;    /* Language specification (defined in main.c) */

#endif /* MAIN_H */
