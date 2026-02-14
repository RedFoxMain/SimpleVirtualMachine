#include <stdio.h>
#include "cpu/cpu.h"

int main(int argc, char* argv[]) {
	 if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
	 	printf("%s [file] [-i]", argv[0]);
	 	return 0;
	 }

	// Init CPU
	cpu_t cpu = {0};
	initCPU(&cpu);
	loadProgram(&cpu, argv[1]);
	while (cpu.is_running) {
	    decode(&cpu);
	}
	if (!strcmp(argv[2], "-i"))
		printCpuInfo(&cpu, PRINT_MEM | PRINT_REGS | PRINT_FLAGS | PRINT_STACK);
	return 0;
}