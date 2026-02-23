/*
 * =============================================================================
 * utils_files.c
 * Utility functions for timestamped log-file generation and output routing.
 *
 * Provides:
 *   split_path                   : Break a full path into directory, basename, extension
 *   generate_timestamped_log_filename : Build a dated filename in PATHDIRLOGS
 *   set_output_test_file         : Open the appropriate output stream (file or stdout)
 *
 * Author: [Team]
 * Date:   2026
 * =============================================================================
 */
#include "./utils_files.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

extern FILE *ofile; /* Defined in main.c (or in each test's .c file) */
 
void split_path(const char *fullpath, char *path, char *filename, char *extension) {
    const char *last_slash = strrchr(fullpath, '/');
    const char *last_dot = strrchr(fullpath, '.');

    if (last_slash) {
        size_t path_len = last_slash - fullpath + 1;
        strncpy(path, fullpath, path_len);
        path[path_len] = '\0';
        strcpy(filename, last_slash + 1);
    } else {
        path[0] = '\0';
        strcpy(filename, fullpath);
    }

    if (last_dot && last_dot > last_slash) {
        strcpy(extension, last_dot + 1);
        filename[last_dot - last_slash - 1] = '\0';  // remove extension from filename
    } else {
        extension[0] = '\0';
    }
}

void generate_timestamped_log_filename(const char* base_name, char* output, size_t maxlen) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char path[MAXFILENAME], filename[MAXFILENAME], extension[MAXFILEEXT];

    split_path(base_name, path, filename, extension);

    if (strlen(extension) == 0) {
        snprintf(extension, sizeof(extension), UTILS_DEFAULT_LOG_EXT); // Default extension if none provided
    }

    // Format: yyyy_mm_dd_hh_mm_base
    snprintf(output, maxlen, UTILS_TIMESTAMP_FMT,
             PATHDIRLOGS, // path
             t->tm_year + UTILS_TM_YEAR_BASE,
             t->tm_mon + 1,
             t->tm_mday,
             t->tm_hour,
             t->tm_min, 
             filename,
             extension);

    fprintf(ofile, "Generated log filename (with time stamp): %s\n", output);

}

// Function to set the output file for test results
// If the filename is "stdout", it will use stdout, otherwise it will open the specified filename
// It adds the timestamp to the filename if it is not "stdout"
FILE* set_output_test_file(const char* filename) {
    FILE *log_file = stdout;  /* Output file handle; starts as stdout */
    char timestamped_filename[MAXFILENAME];

    if (strcmp(filename, UTILS_STDOUT_NAME) != 0) {
        fprintf(log_file, "Machine remote time ");
        generate_timestamped_log_filename(filename, timestamped_filename, sizeof(timestamped_filename));

        // Set the time zone to Europe/Madrid: 
        // (i.e. fake it as GMT-3 if Madrid is in GMT+2 summer time)
        // When run in github actions the server is in another time zone
        // We want timestamp related to our time
        _putenv(UTILS_TZ_SETTING);
        //_putenv("TZ=Europe/Madrid");
        _tzset();
        generate_timestamped_log_filename(filename, timestamped_filename, sizeof(timestamped_filename));
        filename = timestamped_filename;

        log_file = fopen(filename, UTILS_FOPEN_APPEND_MODE); // Tasks can be fast, so they are appended to the same file if it is the same minute
        if (log_file == NULL) {
            fprintf(stderr, "Error opening output file %s. Check if subdirectory exists, otherwise create it and run again\n", filename);
            log_file = stdout;
        }
    }
    if(log_file == stdout){
        printf("See log of execution in stdout (filename %s)\n", filename);
        fprintf(log_file, "See log of execution in stdout (filename %s)\n", filename);
    }
    else{
        printf("See log of execution in file %s\n", filename);
        fprintf(log_file, "See log of execution in file %s\n", filename);
    }
    fflush(log_file);
    return log_file;
}