#include <stdio.h>
#include "cpu.h"
#include "error.h"
#include "argparse.h"

int main(int argc, char* argv[]) {
	args_t args[] = {
		create_arg("h", false, "Display help message"),
		create_arg("f", true, "Specife the input file"),
		create_arg("i", false, "Print all cpu information after running"),
		create_arg("m", false, "Print cpu memory information after running"),
		create_arg("c", false, "Print loaded code"),
		create_arg("r", false, "Print cpu registers information after running"),
		create_arg("l", false, "Print cpu flags information after running"),
		create_arg("s", false, "Print cpu stack information after running")
	};

	argparser_t* argparser = init_argparser(args, sizeof(args) / sizeof(args[0]));
	parse_args(argparser, argc, argv);
	if (get_option(argparser, "h").found) {
	 	printf("%s -ffile [-i | -m | -r | -l | -s]\n", argv[0]);
		print_args_info(argparser);
	 	return 0;
	}

	// Init CPU
	cpu_t cpu = {0};
	init_cpu(&cpu);
	if(get_option(argparser, "f").found) load_program(&cpu, get_option(argparser, "f").arg_value);
	else throw_error("Input file was not specified. Use -f flag\n");
	if (get_option(argparser, "c").found)
		print_cpu_info(&cpu, PRINT_CODE);

	while (cpu.is_running)  {
		decode(&cpu);
	}

	if (get_option(argparser, "i").found)
		print_cpu_info(&cpu, PRINT_CODE | PRINT_MEM | PRINT_REGS | PRINT_FLAGS | PRINT_STACK);
	if (get_option(argparser, "m").found)
		print_cpu_info(&cpu, PRINT_MEM);
	if (get_option(argparser, "r").found)
		print_cpu_info(&cpu, PRINT_REGS);
	if (get_option(argparser, "l").found)
		print_cpu_info(&cpu, PRINT_FLAGS);
	if (get_option(argparser, "s").found)
		print_cpu_info(&cpu, PRINT_STACK);
	destroy_cpu(&cpu);
	return 0;
}