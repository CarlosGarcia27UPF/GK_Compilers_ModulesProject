/*
 * -----------------------------------------------------------------------------
 * utils_files.h
 *
 * Shared helpers for timestamped log filenames used by template tests/modules.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#ifndef UTILS_FILES_H
#define UTILS_FILES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Max output filename length for generated logs.
#define MAXFILENAME 256
// Max extension length extracted from a filename.
#define MAXFILEEXT 64
// Directory used by CI/local runs for generated logs.
#define PATHDIRLOGS "./logs/"
// Default log extension when none is provided.
#define DEFAULT_LOG_EXT "log"
// Special stdout selector used by set_output_test_file.
#define OUTPUT_STDOUT_NAME "stdout"
// Timezone used to make CI timestamps predictable.
#define FIXED_TZ "TZ=GMT-2"
// File open mode for appending logs.
#define LOG_APPEND_MODE "a"

FILE* set_output_test_file(const char* filename);

#endif /* UTILS_FILES_H */
