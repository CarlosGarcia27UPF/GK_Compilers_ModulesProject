#ifndef CLI_H
#define CLI_H

typedef struct {
    int do_comments;    // -c
    int do_directives;  // -d
    int do_help;        // -help
} cli_options_t;

cli_options_t cli_parse(int argc, char **argv);
void cli_print_help(const char *progname);

#endif
