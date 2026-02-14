#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../memory/cpu_memory.h"

typedef enum {
	HALT, MOV, MOVI,
	ADD, SUB, MUL, DIV, IDC,
	AND, OR, SETJMPA, SHIFT,
	JMP, CMP, SCALL
};

// CPU
typedef struct {
	bool is_running;
	bool is_printing;
	memory_t* memory;
	flags_t flag;
	instruction_t instr;
	int program_size;
} cpu_t;

void loadProgram(cpu_t* cpu, const char* file_name);
void initCPU(cpu_t* cpu);
uint16_t fetch(cpu_t* cpu);
void decode(cpu_t* cpu);

// Debug method
void printCpuInfo(cpu_t* cpu, uint8_t mode);
#endif // !CPU_H