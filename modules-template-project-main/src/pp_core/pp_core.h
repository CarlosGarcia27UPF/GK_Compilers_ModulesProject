/* -----------------------------------------------------------------------------
 * Program: C Preprocessor (Practice 1)
 * Author: Emmanuel Kwabena Cooper Acheampong
 * Creation date: 2026-01-24
 * Description:
 *     This module declares the core preprocessing engine entry point.
 *
 * - `pp_run`: Executes preprocessing over a buffer and writes output.
 *
 * Usage:
 *     Include this header in main or integration modules to run preprocessing.
 *
 * Status:
 *     Active - API for the core engine.
 * -------------------------------------------------------------------------- */

#ifndef PP_CORE_H
#define PP_CORE_H

#include "pp_context.h"
#include "buffer/buffer.h"

/* Run the preprocessor on input, writing results to output. */
int pp_run(pp_context_t *ctx, const buffer_t *input, buffer_t *output, const char *base_dir);

#endif
