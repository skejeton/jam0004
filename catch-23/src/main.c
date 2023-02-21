#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

typedef struct MainArguments
{
    const char* input_file;
    bool is_verbose;
} MainArguments;

static inline MainArguments parse_main_args(int argc, const char** argv)
{
    MainArguments args;

    const ArgInfo args_info[] =
    {
        { ARG_POSITIONAL, "INPUT_FILE", "input file to process", &args.input_file },
        { ARG_FLAG, "v", "show verbose output", &args.is_verbose }
    };

    ArgParser arg_parser = arg_parser_create(argc, argv, args_info, sizeof(args_info) / sizeof(args_info[0]));
    
    arg_parser_parse(&arg_parser);

    if(!args.input_file)
    {
        fprintf(stderr, "error: an input file is needed\n");
        arg_parser_print_help(&arg_parser, stderr);
    }

    return args;
}

int main(int argc, const char* argv[])
{
    MainArguments args = parse_main_args(argc, argv);
    DestroyList dl = destroy_list_create();
    
    Unit unit;
    if (!unit_load_from_file(args.input_file, &unit, dl)) {
        fprintf(stderr, "error loading file '%s'\n", args.input_file);
        exit(EXIT_FAILURE);
    }

    if(args.is_verbose)
    {
        printf("Input string: %s\n", unit.scan.text);
    }

    lexer_lex(&unit);

    if(args.is_verbose)
    {
        puts("Tokens:");

        for(TokenList* tok = unit.tokens; tok != NULL; tok = tok->next)
        {
            struct scanner_location loc = token_get_location(tok->token);
            struct token_info info = token_type_get_info(tok->token.type);

            printf("%s:%zu:%zu %s %s\n", unit.filename, loc.line_num, loc.col_num, info.mnemonic, tok->token.data.str ? tok->token.data.str : "NULL");
        }
    }

    Parser p = parser_create(unit.tokens);
    Ast dest;
    if (!parser_parse(&p, dl, &dest)) {
        fprintf(stderr, "well, you have an error somewhere. *CAREFULLY* look at the spec and find it! go go go!\n");
        exit(EXIT_FAILURE);
    }

    ast_print(&dest, 0);

    unit_destroy(&unit);
    destroy_list_destroy(dl);

    return 0;
}

