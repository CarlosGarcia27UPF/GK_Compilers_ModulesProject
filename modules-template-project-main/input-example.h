/*
 * Compilers
 * Input example code: Define simple .c and .h files to test your processing
 * File name: input-example.h
 * Additional files: input-example.c
 */
#ifndef PP_H // to avoid multiple includes of the same file
#define	PP_H

// Your pre-processor is not required to include c libraries. 
// Just comment them out or leave the include untouched.
//#include <stdio.h>
//#include <stdlib.h>     // To set env for time zone
//#include <string.h>     // For filename manipulation   

#define ON              1
#define OFF             0

#define PRINTDESTFILE       "out.txt"    // file destination of normal program print: stdout/out.txt
#define PRINTERRORFILE      "errmsg.txt"   // file destination of normal program print: errmsg.txt

#define N               2   
#define PRINTWARNING    ON      // ON = Yes OFF / NO
#define PRINTERROR      ON     // ON = Yes OFF / NO

// MACROS used for the printouts instead of using f/printfs so that all prints
// in the program are treated the same way.
// Use ERROR when the print out informs of a ERROR/problem in the program and it must finish the program
// Use WARNING when the print out informs of a ERROR/problem in the program but the preprocessor can continue
// You have to define the format of your errors, this is just an example to test, 
// but your code can do a different version of this to treat errors
/* usage: ERROR(("Warning: Note the two brackets\n")) */
#define ERROR1( num, message)({printf("\n%d ERROR \n", num);printf message;})
#define FERROR1(num, message)({fprintf(errfile,"\n%d ERROOORRRRRRRRRR\n", num);fprintf message;})
#define WARNING(num, message)({fprintf(errfile,"%d ERROR: ", num);fprintf message;\
                        printf("ERROOORRRRRRRRRR see output file\n");})
#define ERROR(num, message)({WARNING( num, message); exit(0);})

FILE    *errfile;        // File where to write error messages
FILE    *ofile;          // File where to write program information

#endif	/* PP_H */