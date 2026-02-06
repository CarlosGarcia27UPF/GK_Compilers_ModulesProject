#include "./main.h"

FILE* ofile = NULL; // The output handler for the project run (same variable name as in modules)

int main(int argc, char *argv[]) {
    int n = 0;

    ofile = stdout; // Default output to stdout
    ofile = set_output_test_file(PROJOUTFILENAME);

    fprintf(ofile, "Starting module args ...\n");
    process_arguments(argc, argv);
    fprintf(ofile, "Finished module args!!\n");

    fprintf(ofile, "Starting module 2 ...\n");
    n = fib(FIBNUM);
    fprintf(ofile, "Fibonacci of %d is: %d\n", FIBNUM, n);
    fprintf(ofile, "Finished module 2!!\n");
    
    printf("All modules executed successfully!\n\n");
    fprintf(ofile, "All modules executed successfully!\n\n");

    fclose(ofile); 
    return 0;
}