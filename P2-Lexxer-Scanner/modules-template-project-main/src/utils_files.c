/*
 * -----------------------------------------------------------------------------
 * utils_files.c
 *
 * Utilities for creating timestamped log filenames and routing test output.
 *
 * Team: Compilers P2
 * -----------------------------------------------------------------------------
 */

#include "./main.h"

// Splits full path into path, base filename, and extension (without dot).
static void split_path(const char *fullpath, char *path, char *filename,
                       char *extension) {
    const char *last_slash;
    const char *base_name;
    const char *last_dot;
    size_t path_len;
    size_t name_len;

    if (fullpath == NULL || path == NULL || filename == NULL || extension == NULL) {
        return;
    }

    last_slash = strrchr(fullpath, '/');
    if (last_slash != NULL) {
        path_len = (size_t)(last_slash - fullpath + 1);
        strncpy(path, fullpath, path_len);
        path[path_len] = '\0';
        base_name = last_slash + 1;
    } else {
        path[0] = '\0';
        base_name = fullpath;
    }

    last_dot = strrchr(base_name, '.');
    if (last_dot != NULL) {
        name_len = (size_t)(last_dot - base_name);
        strncpy(filename, base_name, name_len);
        filename[name_len] = '\0';
        strcpy(extension, last_dot + 1);
    } else {
        strcpy(filename, base_name);
        extension[0] = '\0';
    }
}

// Creates: PATHDIRLOGS/yyyy_mm_dd_hh_mm_<base>.<ext>
static void generate_timestamped_log_filename(const char* base_name, char* output,
                                              size_t maxlen) {
    time_t now;
    struct tm *time_info;
    char path[MAXFILENAME];
    char filename[MAXFILENAME];
    char extension[MAXFILEEXT];

    if (base_name == NULL || output == NULL || maxlen == 0) {
        return;
    }

    now = time(NULL);
    time_info = localtime(&now);
    if (time_info == NULL) {
        output[0] = '\0';
        return;
    }

    split_path(base_name, path, filename, extension);
    if (extension[0] == '\0') {
        snprintf(extension, sizeof(extension), "%s", DEFAULT_LOG_EXT);
    }

    snprintf(output, maxlen, "%s%04d_%02d_%02d_%02d_%02d_%s.%s",
             PATHDIRLOGS,
             time_info->tm_year + 1900,
             time_info->tm_mon + 1,
             time_info->tm_mday,
             time_info->tm_hour,
             time_info->tm_min,
             filename,
             extension);
}

// Returns stdout or an opened timestamped file for test output.
FILE* set_output_test_file(const char* filename) {
    FILE *dest = stdout;
    char timestamped_filename[MAXFILENAME];
    const char *display_name = filename;

    if (filename == NULL) {
        return stdout;
    }

    if (strcmp(filename, OUTPUT_STDOUT_NAME) != 0) {
#ifdef _WIN32
        _putenv(FIXED_TZ);
        _tzset();
#else
        putenv(FIXED_TZ);
        tzset();
#endif
        generate_timestamped_log_filename(filename, timestamped_filename,
                                          sizeof(timestamped_filename));
        display_name = timestamped_filename;
        dest = fopen(display_name, LOG_APPEND_MODE);
        if (dest == NULL) {
            fprintf(stderr, "Error opening output file %s\n", display_name);
            dest = stdout;
            display_name = OUTPUT_STDOUT_NAME;
        }
    }

    fprintf(stdout, "See log of execution in %s (filename %s)\n",
            (dest == stdout) ? "stdout" : "file", display_name);
    fprintf(dest, "See log of execution in %s (filename %s)\n",
            (dest == stdout) ? "stdout" : "file", display_name);
    fflush(dest);
    return dest;
}
