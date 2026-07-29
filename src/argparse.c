#include "argparse.h"
#include "error.h"

argparser_t* init_argparser(args_t* args, size_t args_count) {
    argparser_t* temp = (argparser_t*)malloc(sizeof(argparser_t));
    if(!temp) 
        throw_error("Can't allocate memory for argparser!\n");
    temp->args = args;
    temp->args_count = args_count;
    return temp;
}

args_t create_arg(char* name, bool has_args, char* info) {
    args_t arg = {
        .name = name,
        .arg_value = NULL,
        .has_args = has_args,
        .info = info,
        .found = false
    };
    return arg;
}

args_t get_option(argparser_t* argparser, char* name) {
    if(strlen(name) == 0) 
        throw_error("Options %s not found!\n", name);
    for (size_t i = 0; i < argparser->args_count; i++)
        if(!strcmp(argparser->args[i].name, name))
            return argparser->args[i];
    throw_error("Options %s not found!\n", name);
}

void parse_args(argparser_t* argparser, size_t argc, char* argv[]) {
    if(argc < 2)
        throw_error("program.exe -f input_file -o output_file\n");
    for (size_t i = 1; i < argc; i++) {
        char* arg = argv[i];
        if(arg[0] == '-') {
            char* option_name = arg + 1;
            size_t option_name_len = strlen(option_name);
            for (size_t j = 0; j < argparser->args_count; j++) {
                size_t arg_len = strlen(argparser->args[j].name);
                if(!strncmp(option_name, argparser->args[j].name, arg_len)) {
                    argparser->args[j].found = true;
                    if(argparser->args[j].has_args) {
                        if(option_name_len > arg_len)
                            argparser->args[j].arg_value = option_name + arg_len;
                        else {
                            if(i + 1 < argc)
                                argparser->args[j].arg_value = argv[++i];
                            else 
                                throw_error("The flag \"%s\" requires an argument\n", arg);
                        }
                    }
                    break;
                }
            }
        } else throw_error("Invalid option \"%s\"\n", argv[i]);
    }
}

void print_args_info(argparser_t* argparser) {
    for (size_t i = 0; i < argparser->args_count; i++)
        printf("-%s: %s\n", argparser->args[i].name, argparser->args[i].info);
}