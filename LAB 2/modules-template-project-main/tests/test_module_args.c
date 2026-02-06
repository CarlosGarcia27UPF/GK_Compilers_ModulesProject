/* 
 * tests/test_module_args.c
 * 
 * This module is responsible to process the program arguments. 
 * It processes the command-line arguments of the entire application and sets the 
 * information in a global config variable.
 * 
 * This module has to manipulate launch.json to set the program arguments for running/debugging. 
 * So the contributors of this module have to have "ownership" of launch.json file too.
 * 
 * It includes the test cases of process_arguments to set the framework on how to set tests,
 * and do an exhaustive testing of a function.
 * The output is directed to a specified file or stdout.
 *
 * The constants and configurations of the tests are defined in the header file test_module1.h.
 * 
 * Team: [Your Team Name]
 * 
 * Update 1: [Functionality added or modified]
 * Author: [The member/s responsible of the update]
 * Date: [Date of the update completed]
*/
#include "../tests/test_module_args.h"

FILE *ofile = NULL; // file handler to send the module's output (to a file or stdout)

void test_process_arguments(int argc, char *argv[]) {
    print_arguments(argc, argv);
    assert(argc >= 2); // At least the program name should be present
}

int main(int argc, char *argv[]) {

    ofile = stdout; // Default output to stdout
    ofile = set_output_test_file(MODARGSTESTLOGFILENAME);

    printf("Starting tests for Module args...\n");
    fprintf(ofile, "Starting tests for Module args...\n");
    fprintf(ofile, "Testing of module arguments not implemented. Not done. Here put all tests of process_arguments\n");
    test_process_arguments(argc, argv);      
    printf("Finished tests for Module args...\n\n");
    fprintf(ofile, "Finished tests for Module args...\n\n");

    fclose(ofile); 
    return 0;
}
