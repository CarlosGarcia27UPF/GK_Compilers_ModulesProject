#ifndef MAIN_H
#define MAIN_H
 
#include <stdio.h>
#include "./utils_files.h"
#include "./module_args/module_args.h"
#include "./module_2/module_2.h"
#include "./cli/cli.h"
#include "./io/io.h"
#include "./comments/comments.h"
#include "./directives/directives.h"
#include "./macros/macros.h"
#include "./errors/errors.h"
#include "./tokens/tokens.h"

// Output file of project run: either a stdout or a filename with log extension (comment one out)
//#define PROJOUTFILENAME "./proj_modules_template.log"
#define PROJOUTFILENAME "stdout"

#endif // MAIN_H
