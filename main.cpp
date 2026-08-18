#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yyparse(void);
int yylex(void);
extern FILE *yyin;


/* ================= FLAGS ================= */

int show_tokens = 0;
int show_ast    = 0;
int sa_scan     = 0;
int sa_parse    = 0;

/* Output files */
FILE *tokfile = nullptr;
FILE *astfile = nullptr;

/* ================= USAGE ================= */

static void usage()
{
    fprintf(stderr,
        "Usage: sclp [--show-tokens] [--show-ast] "
        "[--sa-scan] [--sa-parse] file\n");
    exit(1);
}

/* ================= OPTION PROCESSING ================= */

static const char *process_command_options(int argc, char *argv[])
{
    const char *input_file = nullptr;

    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "--show-tokens") == 0)
            show_tokens = 1;

        else if (strcmp(argv[i], "--show-ast") == 0)
            show_ast = 1;

        else if (strcmp(argv[i], "--sa-scan") == 0)
            sa_scan = 1;

        else if (strcmp(argv[i], "--sa-parse") == 0)
            sa_parse = 1;

        else if (argv[i][0] == '-')
            usage();

        else
            input_file = argv[i];
    }

    if (!input_file)
        usage();

    return input_file;
}

/* ================= MAIN ================= */

int main(int argc, char *argv[])
{
    int status = 0;

    const char *input_file = process_command_options(argc, argv);

    yyin = fopen(input_file, "r");
    if (!yyin) {
        perror(input_file);
        return 1;
    }

    /* ---------- Open .toks file if needed ---------- */
    char tokname[1024];
    if (show_tokens) {
        snprintf(tokname, sizeof(tokname), "%s.toks", input_file);
        tokfile = fopen(tokname, "w");
        if (!tokfile) {
            perror(tokname);
            return 1;
        }
    }

    /* ---------- Open .ast file if needed ---------- */
    char astname[1024];
    if (show_ast && !sa_scan && !sa_parse) {
        snprintf(astname, sizeof(astname), "%s.ast", input_file);
        astfile = fopen(astname, "w");
        if (!astfile) {
            perror(astname);
            return 1;
        }
    }

    /* ---------- Mode Priority ---------- */

    if (sa_scan) {
        /* Scanner only */
        int tok;
        while ((tok = yylex()) != 0) {
            if (tok < 0) {
                status = 1;
                break;
            }
        }
    }
    else {
        /* Parser (with or without semantics) */
        status = yyparse();
    }

    /* ---------- If error, truncate output files ---------- */

    if (status != 0) {

        if (tokfile) {
            fclose(tokfile);
            tokfile = fopen(tokname, "w");
            if (tokfile) fclose(tokfile);
            tokfile = nullptr;
        }

        if (astfile) {
            fclose(astfile);
            astfile = fopen(astname, "w");
            if (astfile) fclose(astfile);
            astfile = nullptr;
        }
    }

    /* ---------- Close files ---------- */

    if (tokfile)
        fclose(tokfile);

    if (astfile)
        fclose(astfile);

    if (yyin)
        fclose(yyin);

    return status;
}
