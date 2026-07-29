#ifndef CPU_ARGPARSE_H
#define CPU_ARGPARSE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char* name;
    char* arg_value;
    char* info;
    bool has_args;
    bool found;
} args_t;

typedef struct {
    args_t* args;
    size_t args_count;
} argparser_t;

argparser_t* init_argparser(args_t* args, size_t args_count);
void parse_args(argparser_t* argparser, size_t argc, char* argv[]);
void print_args_info(argparser_t* argparser);
args_t create_arg(char* name, bool has_args, char* info);
args_t get_option(argparser_t* argparser, char* name);

#endif // !CPU_ARGPARSE_H