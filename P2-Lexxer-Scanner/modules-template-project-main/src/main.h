#ifndef MAIN_H
#define MAIN_H
 
#include <stdio.h>
#include "./utils_files.h"
#include "./module_args/module_args.h"
#include "./module_2/module_2.h"

// Scanner module includes.
#include "./lang_spec/lang_spec.h"
#include "./char_stream/char_stream.h"
#include "./token/token.h"
#include "./token_list/token_list.h"
#include "./automata/automata.h"
#include "./out_writer/out_writer.h"
#include "./error/error.h"
#include "./logger/logger.h"
#include "./counter/counter.h"

// Output file path used by template utilities.
#define PROJOUTFILENAME "./proj_modules_template.log"

// Minimum number of CLI args (program + input file).
#define MIN_ARGS 2

// argv index of input file.
#define ARG_INPUT_FILE 1

// Max filename buffer size used by driver.
#define MAX_FILENAME_BUF 512

extern FILE* ofile; // Output handler used by template modules.

#endif /* MAIN_H */
