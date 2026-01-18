#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "../spec/pp_spec.h"

static int is_flag(const char *arg, const char *flag)
{
    return (arg != NULL) && (strcmp(arg, flag) == 0);
}

cli_options_t cli_parse(int argc, char **argv)
{
    cli_options_t opt;
    opt.do_comments = 0;
    opt.do_directives = 0;
    opt.do_help = 0;

    /* First pass: detect if user provided any flags at all */
    int has_any_flag = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i] != NULL && argv[i][0] == '-') {
            has_any_flag = 1;
            break;
        }
    }

    /* Default behavior: if no flags => -c */
    if (!has_any_flag) {
        opt.do_comments = 1;
    }

    /* Parse flags */
    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];

        if (is_flag(a, PP_FLAG_HELP)) {
            opt.do_help = 1;
        } else if (is_flag(a, PP_FLAG_ALL)) {
            opt.do_comments = 1;
            opt.do_directives = 1;
        } else if (is_flag(a, PP_FLAG_C)) {
            opt.do_comments = 1;
        } else if (is_flag(a, PP_FLAG_D)) {
            opt.do_directives = 1;
        } else {
            /* Not a recognized flag:
               likely the input filename, ignore here. */
        }
    }

    return opt;
}

void cli_print_help(const char *progname)
{
    if (!progname) progname = "pp";

    printf("Usage:\n");
    printf("  %s [options] <file.c|file.h>\n", progname);
    printf("\nOptions:\n");
    printf("  %s     Remove comments (default if no flags)\n", PP_FLAG_C);
    printf("  %s     Process directives (#include, #define, #ifdef/#endif) + macro expansion\n", PP_FLAG_D);
    printf("  %s   Equivalent to %s %s\n", PP_FLAG_ALL, PP_FLAG_C, PP_FLAG_D);
    printf("  %s  Show this help\n", PP_FLAG_HELP);
}
