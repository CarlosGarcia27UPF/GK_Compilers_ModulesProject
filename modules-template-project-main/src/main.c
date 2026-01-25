/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module provides the program entry point and orchestrates execution.
 *
 * - `get_input_path`: Extracts the input filename from argv.
 * - `run_preprocessor`: Coordinates CLI parsing, IO, and preprocessing.
 * - `main`: Thin wrapper that calls `run_preprocessor`.
 *
 * Usage:
 *     Invoked by the OS to run the preprocessor on a given input file.
 *
 * Status:
 *     Active - core application entry point.
 * -------------------------------------------------------------------------- */

#include "main.h"
#include "cli/cli.h"
#include "pp_core/pp_core.h"
#include "pp_core/pp_context.h"
#include "buffer/buffer.h"
#include "io/io.h"
#include "errors/errors.h"
#include "spec/pp_spec.h"

#include <string.h>

FILE* ofile = NULL;

/* Return the last non-flag argument, assumed to be the input path. */
static const char *get_input_path(int argc, char **argv)
{
    const char *path = 0;
    for (int i = 1; i < argc; i++) {
        // Last non-flag argument is treated as the input file
        if (argv[i][0] != PP_CHAR_DASH) path = argv[i];
    }
    return path;
}

/* Orchestrate CLI parsing, file IO, and preprocessing. */
static int run_preprocessor(int argc, char **argv)
{
    // Initialize logging using configuration from main.h
    ofile = stdout;
    ofile = set_output_test_file(PROJOUTFILENAME);
    errors_init();

    // Parse command-line options (help, comments-only, directives/macros)
    cli_options_t opt = cli_parse(argc, argv);

    if (opt.do_help) {
        // Show help and exit if -help was requested
        cli_print_help(argv[0]);
        return 0;
    }

    // Determine which file to preprocess
    const char *in_path = get_input_path(argc, argv);
    if (!in_path) {
        cli_print_help(argv[0]);
        return 1;
    }

    // Buffers for input file, processed output, and output filename
    buffer_t in, out, out_name;
    buffer_init(&in);
    buffer_init(&out);
    buffer_init(&out_name);

    // Read input file and compute output filename
    if (io_read_file(in_path, &in) != 0) return 1;
    if (io_make_output_name(in_path, &out_name) != 0) return 1;

    // Set up preprocessing context
    pp_context_t ctx;
    ctx.opt = opt;
    ctx.current_file = in_path;
    ctx.current_line = 0;

    // Compute base directory for resolving relative includes
    char base_dir[PP_MAX_PATH_LEN];
    io_compute_base_dir(in_path, base_dir, sizeof(base_dir));

    // Run the preprocessor (comments, directives, macros)
    pp_run(&ctx, &in, &out, base_dir);

    // Write processed output to the generated filename
    io_write_file(out_name.data, &out);

    buffer_free(&in);
    buffer_free(&out);
    buffer_free(&out_name);

    // Close output file if it was redirected
    if (ofile && ofile != stdout) {
        fclose(ofile);
    }

    return get_error_count() > 0 ? 1 : 0;
    
}

//Minimal wrapper for program entry point.
int main(int argc, char **argv)
{
    return run_preprocessor(argc, argv);
}
