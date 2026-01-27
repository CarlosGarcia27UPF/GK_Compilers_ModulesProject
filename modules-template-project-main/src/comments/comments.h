/* -----------------------------------------------------------------------------
 * comments.h
 *
 * Module: comments - Comment removal
 * Responsible for: Removing single-line and multi-line comments from source code.
 *
 * P1 behavior:
 *  - Each comment is replaced by a single space ' '.
 *  - Newlines are preserved (including newlines inside block comments).
 *  - Comment markers inside string/char literals are ignored.
 * 
 * Author: Francesc Baiget Altemir
 * -------------------------------------------------------------------------- */

#ifndef COMMENTS_H
#define COMMENTS_H

#include <stdio.h>
#include "buffer/buffer.h"

/* Comment processing state for multi-line handling */
typedef struct {
    int in_block_comment;
    int prev_char;
} comment_state_t;

/* Initialize comment processing state */
void comments_state_init(comment_state_t *state);

/* Process a single line removing comments.
 * State is preserved across calls for multi-line block comments.
 * Returns 0 on success, non-zero on error.
 */
int comments_process_line(const char *input, long input_len, buffer_t *output, comment_state_t *state);

/* Update comment parsing state without producing output.
 * Use this when comments must be preserved (e.g., -d mode) but directives/macros
 * should still ignore text inside block comments.
 */
void comments_update_state(const char *input, long input_len, comment_state_t *state);

#endif /* COMMENTS_H */
