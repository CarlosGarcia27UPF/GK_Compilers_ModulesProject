#ifndef PP_CORE_H
#define PP_CORE_H

#include "pp_context.h"
#include "buffer/buffer.h"


int pp_run(pp_context_t *ctx, const buffer_t *input, buffer_t *output);

#endif
