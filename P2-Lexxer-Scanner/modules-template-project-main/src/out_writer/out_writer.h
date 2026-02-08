/**
 * @file out_writer.h
 * @brief Output Writer Module (.cscn formatting) - STUB
 * 
 * ===============================================
 * OUT_WRITER - .CSCN FORMATTING
 * ===============================================
 * 
 * RELEASE/DEBUG formatting only. No scanning.
 * 
 * Called by: driver
 * Calls into: token, lang_spec, logger
 * 
 * TODO: This is a STUB - implement full functionality
 */

#ifndef OUT_WRITER_H
#define OUT_WRITER_H

#include <stdio.h>
#include <stdbool.h>
#include "../token_list/token_list.h"

/* Output modes */
typedef enum {
    OUT_MODE_RELEASE,   /* Release format */
    OUT_MODE_DEBUG      /* Debug format with extra info */
} OutputMode;

/**
 * @brief Initialize output writer
 * @param output Output file
 * @param mode Output mode (RELEASE/DEBUG)
 */
void out_writer_init(FILE* output, OutputMode mode);

/**
 * @brief Write tokens to output file
 * @param tokens Token list to write
 * @return true if successful
 */
bool out_writer_write(const TokenList* tokens);

/**
 * @brief Write header
 */
void out_writer_header(void);

/**
 * @brief Write footer
 */
void out_writer_footer(void);

/**
 * @brief Close output writer
 */
void out_writer_close(void);

#endif /* OUT_WRITER_H */
