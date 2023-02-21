#include "args.h"

#include <stdlib.h>
#include <string.h>

static bool is_positional(const char* arg)
{
    if(strlen(arg) < 1) return false;

    return arg[0] != '-';
}

static bool is_flag_or_variable(const char* arg, const char* name)
{
    if(strlen(arg) < 1) return false;

    return arg[0] == '-' && strcmp(arg + 1, name) == 0;
}

void arg_parser_parse(const ArgParser* parser)
{
    size_t pos = 0;

    // initialise data with known values
    for(size_t j = 0; j < parser->arg_info_count; j++)
    {
        if(parser->arg_info[j].type == ARG_FLAG)
        {
            bool* data_ptr = (bool*)parser->arg_info[j].data;

            if(data_ptr)
            {
                *data_ptr = false;
            }
        }
        else
        {
            const char** data_ptr = (const char**)parser->arg_info[j].data;

            if(data_ptr)
            {
                *data_ptr = NULL;
            }
        }
    }
    
    for(size_t i = 1; i < parser->argc; i++)
    {
        size_t arg_info_pos = 0;
        bool found = false;

        for(size_t j = 0; j < parser->arg_info_count; j++)
        {
            switch(parser->arg_info[j].type)
            {
                case ARG_POSITIONAL:
                {
                    if(!is_positional(parser->argv[i]) || pos != arg_info_pos)
                    {
                        arg_info_pos++;

                        continue;
                    }

                    const char** data_ptr = (const char**)parser->arg_info[j].data;

                    if(data_ptr)
                    {
                        *data_ptr = parser->argv[i];
                    }

                    pos++;

                    break;
                }
                case ARG_VARIABLE:
                {
                    if(!is_flag_or_variable(parser->argv[i], parser->arg_info[j].name))
                    {
                        continue;
                    }

                    if((i + 1) >= parser->argc)
                    {
                        fprintf(stderr, "'%s' needs one parameter\n", parser->argv[i]);

                        arg_parser_print_help(parser, stderr);

                        return;
                    }

                    const char** data_ptr = (const char**)parser->arg_info[j].data;
            
                    if(data_ptr)
                    {
                        *data_ptr = parser->argv[i + 1];
                    }

                    ++i;

                    break;
                }
                case ARG_FLAG:
                {
                    if(!is_flag_or_variable(parser->argv[i], parser->arg_info[j].name))
                    {
                        continue;
                    }

                    bool* data_ptr = (bool*)parser->arg_info[j].data;

                    if(data_ptr)
                    {
                        *data_ptr = true;
                    }

                    break;
                }
                default:
                {
                    found = false;

                    continue;
                }
            }

            found = true;

            break;
        }

        if(!found)
        {
            fprintf(stderr, "'%s' is not a valid argument\n", parser->argv[i]);

            arg_parser_print_help(parser, stderr);

            return;
        }
    }
}

void arg_parser_print_help(const ArgParser* parser, FILE* file)
{
    fputs("argument help: \n", file);

    for(size_t i = 0; i < parser->arg_info_count; i++)
    {
        switch(parser->arg_info[i].type)
        {
            case ARG_POSITIONAL:
            {
                fprintf(file, "    [%s] - %s\n", parser->arg_info[i].name, parser->arg_info[i].info);

                break;
            }
            case ARG_VARIABLE:
            {
                fprintf(file, "    -%s <INPUT> - %s\n", parser->arg_info[i].name, parser->arg_info[i].info);

                break;
            }
            case ARG_FLAG:
            {
                fprintf(file, "    -%s - %s\n", parser->arg_info[i].name, parser->arg_info[i].info);

                break;
            }
        }
    }

    if(file == stderr)
    {
        exit(EXIT_FAILURE);
    }
}

