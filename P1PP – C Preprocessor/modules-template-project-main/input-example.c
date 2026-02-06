/*
 * Compilers
 * Input example code: Define simple .c and .h files to test your processing
 * File name: input-example.c
 * Additional files: input-example.h
 */
#include "./input-example.h"

int main(int argc, char**argv) {
    
    int i;
 
    //printf("I am in main function \n");
    ofile = stdout;             // default is stdout
    ofile = fopen( PRINTDESTFILE, "w");
    if(ofile == NULL)                 
        WARNING(0, (ofile, "Problem creating %s\n", PRINTDESTFILE));

    errfile = stdout;           // default is stdout
    errfile = fopen( PRINTERRORFILE, "w");
    if(errfile != NULL)                
        WARNING(0, (ofile, "Problem creating %s\n", PRINTERRORFILE));
    

    // Prints arguments
    fprintf(ofile, "Arguments:\n");
    for (i = 0; i < argc; i++) {
        fprintf(ofile, "%i: %s\n", i, argv[i]);
#ifdef (PRINTERROR == ON)
        WARNING(i, (errfile, "warning print example %s \n", argv[i]));
#endif
    }
    
#if (PRINTERROR == ON)
    ERROR(i, (errfile, "This is an error, and it stops"));
#endif
    
    fclose(ofile);
    fclose(errfile);
    
    return 0;
}
