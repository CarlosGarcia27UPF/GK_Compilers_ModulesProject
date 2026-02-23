/*
 * =============================================================================
 * utils_files.h
 * Utility constants and prototypes for timestamped log-file generation.
 *
 * Provides path splitting, timestamped filename generation, and an output
 * routing helper used by all test and main modules to write logs.
 *
 * Author: [Team]
 * Date:   2026
 * =============================================================================
 */
#ifndef UTILS_FILES_H
#define UTILS_FILES_H
 
#include <stdio.h>
#include <stdlib.h>     // To set env for time zone
#include <string.h>     // For filename manipulation    
#include <time.h>       // For time-stamp of output logs filenames

// Global defines
#define MAXFILENAME 256          // Maximum length of the filename for output logs
#define MAXFILEEXT 64            // Maximum length of the file extension

// Sentinel name for writing to stdout instead of a log file
#define UTILS_STDOUT_NAME "stdout"          // Pass as filename to get stdout output

// Timezone setting for Europe/Madrid time (GMT+2 in summer) on CI servers
#define UTILS_TZ_SETTING "TZ=GMT-2"        // Passed to putenv to fake Madrid local time

// Base year offset in struct tm: tm_year field stores years since UTILS_TM_YEAR_BASE
#define UTILS_TM_YEAR_BASE 1900             // struct tm.tm_year is years since 1900

// File open modes used by utils_files
#define UTILS_FOPEN_APPEND_MODE "a"         // fopen mode to append to an existing log file
#define UTILS_DEFAULT_LOG_EXT   "log"       // Default file extension when no extension is provided

// Timestamp format: PATHDIRLOGS + YYYY_MM_DD_HH_MM_basename.ext
#define UTILS_TIMESTAMP_FMT "%s%04d_%02d_%02d_%02d_%02d_%s.%s"  // snprintf format for timestamped log paths

// Path to the logs directory: put your full path, the directory has to exist
//#define PATHDIRLOGS "I:/Mi unidad/UPFdrive/docencia/github/compilers/modules_template/logs/" 
#define PATHDIRLOGS "./logs/" // For running yml

// Function prototypes
void  split_path(const char *fullpath, char *path, char *filename, char *extension);
void  generate_timestamped_log_filename(const char *base_name, char *output, size_t maxlen);
FILE* set_output_test_file(const char* filename);

#endif // UTILS_FILES_H