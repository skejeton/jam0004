#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

enum arg_info_type
{
    ARG_POSITIONAL, /* anywhere in arguments, denoted by its index */
    ARG_VARIABLE, /* -var_name var_data */
    ARG_FLAG /* -flag_name */
};

typedef struct ArgInfo
{
    enum arg_info_type type;
    const char* name;
    const char* info;
    void* data;
} ArgInfo;

typedef struct ArgParser
{
    size_t argc;
    const char** argv;
    const ArgInfo* arg_info;
    size_t arg_info_count;
} ArgParser;

static inline ArgParser arg_parser_create(size_t argc, const char** argv, const ArgInfo* arg_info, size_t arg_info_count)
{
    return (ArgParser) {
        argc,
        argv,
        arg_info,
        arg_info_count
    };
}

void arg_parser_parse(const ArgParser* parser);
// Prints all options to `file`. If `file` is stderr, then exit.
void arg_parser_print_help(const ArgParser* parser, FILE* file);
