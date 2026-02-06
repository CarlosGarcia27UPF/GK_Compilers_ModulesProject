#ifndef MAIN_H
#define MAIN_H
 
#include <stdio.h>
#include "./utils_files.h"
#include "./module_args/module_args.h"
#include "./module_2/module_2.h"

// Output file of project run: either a stdout or a filename with log extension (comment one out)
#define PROJOUTFILENAME "./proj_modules_template.log"
//#define PROJOUTFILENAME "stdout"

extern FILE* ofile; // The output handler for the project run (same variable name as in modules)

#endif // MAIN_H
