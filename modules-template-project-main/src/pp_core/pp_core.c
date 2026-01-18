#include "pp_core.h"

/* Returns 0 on success, non-zero on error */
int pp_run(pp_context_t *ctx, const buffer_t *input, buffer_t *output, const char *base_dir)
{
    (void) base_dir;  /* unused for now */
    long i = 0;
    long line_start = 0;

    if (!ctx || !input || !output || !input->data) {
        return 1;
    }

    /* We count lines the same way humans do:
       - each '\n' ends a line
       - last line may or may not have '\n'
    */
    ctx->current_line = 0;

    while (i < input->len) {
        if (input->data[i] == '\n') {
            long line_len = (i - line_start) + 1;  /* include '\n' */
            ctx->current_line++;

            /* For now: copy line unchanged.
               Later: pass this line to comments/directives/macros. */
            if (buffer_append_n(output, input->data + line_start, line_len) != 0) {
                ctx->error_count++;
                return 2;
            }

            i++;
            line_start = i;
        } else {
            i++;
        }
    }

    /* Handle last line if file doesn't end with '\n' */
    if (line_start < input->len) {
        long line_len = input->len - line_start;
        ctx->current_line++;

        if (buffer_append_n(output, input->data + line_start, line_len) != 0) {
            ctx->error_count++;
            return 3;
        }
    }

    return 0;
}
