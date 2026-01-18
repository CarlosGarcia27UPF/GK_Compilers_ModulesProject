#include "cli/cli.h"
#include "pp_core/pp_core.h"
#include "pp_core/pp_context.h"
#include "buffer/buffer.h"
#include "io/io.h"

static const char *get_input_path(int argc, char **argv)
{
    const char *path = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') path = argv[i];
    }
    return path;
}

int main(int argc, char **argv)
{
    cli_options_t opt = cli_parse(argc, argv);

    if (opt.do_help) {
        cli_print_help(argv[0]);
        return 0;
    }

    const char *in_path = get_input_path(argc, argv);
    if (!in_path) {
        cli_print_help(argv[0]);
        return 1;
    }

    buffer_t in, out, out_name;
    buffer_init(&in);
    buffer_init(&out);
    buffer_init(&out_name);

    if (io_read_file(in_path, &in) != 0) return 1;
    if (io_make_output_name(in_path, &out_name) != 0) return 1;

    pp_context_t ctx;
    ctx.opt = opt;
    ctx.current_file = in_path;
    ctx.current_line = 0;
    ctx.error_count = 0;

    pp_run(&ctx, &in, &out, ".");

    io_write_file(out_name.data, &out);

    buffer_free(&in);
    buffer_free(&out);
    buffer_free(&out_name);

    return (ctx.error_count > 0) ? 1 : 0;
}
