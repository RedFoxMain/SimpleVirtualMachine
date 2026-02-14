#include "cpu.h"

void clearFlags(cpu_t* cpu);

void loadProgram(cpu_t* cpu, char* file_name) {
	FILE* fp = fopen(file_name, "rb");
	if (fp == NULL) {
		printf("Cannot find file (%s)\n", (file_name == NULL || strlen(file_name) == 0) ? "null" : file_name);
		return;
	}
	fseek(fp, 0, SEEK_END);
	int file_size = ftell(fp);
	rewind(fp);
	cpu->program_size = (int)fread(cpu->memory->data + PROGRAM_ADDRESS, sizeof(uint16_t), file_size / 4, fp);
	fclose(fp);
}

// Init CPU
void initCPU(cpu_t* cpu) {
	cpu->is_running = true;
	cpu->is_printing = false;
	cpu->program_size = 0;

	// Init memory
	cpu->memory = (memory_t*)malloc(sizeof(memory_t));
	initMemory(cpu->memory);
	
	// Init instruction bits
	cpu->instr.nnn = 0;
	cpu->instr.nn = 0;
	cpu->instr.n = 0;

	// Set flags
	clearFlags(cpu);
}

// Clear CPU flags
void clearFlags(cpu_t* cpu) {
	cpu->flag.e = -2;
	cpu->flag.o = 0;
	cpu->flag.z = 1;
}

// Update z and o flags
void updateZOFlags(cpu_t* cpu, uint8_t reg) {
	cpu->flag.z = (!reg) ? 1 : 0;
	cpu->flag.o = ((UINT8_MAX - reg) == 255);
	cpu->flag.e = -2;
}

// Update e flags
void updateEFlag(cpu_t* cpu, uint8_t val1, uint8_t val2) {
	if (val1 == val2)
		cpu->flag.e = 0;
	else if(val1 > val2)
		cpu->flag.e = 1;
	else if (val1 < val2)
		cpu->flag.e = -1;
}

// Parse opcode
uint16_t fetch(cpu_t* cpu) {
	uint16_t op_code = memoryRead(cpu->memory, cpu->memory->pc++, 0);
	cpu->instr.nnn = op_code & 0x0FFF;
	cpu->instr.nn = op_code & 0x00FF;
	cpu->instr.n = op_code & 0x000F;
	return op_code;
}

// Decode command
void decode(cpu_t* cpu) {
	uint8_t reg1;
	uint8_t reg2;

	switch (fetch(cpu) >> 12) {
		case HALT:
			cpu->is_running = false;
			break;

		case MOV:
			reg1 = GET_NNN_BIT(cpu->instr.nnn);
			reg2 = GET_NN_BIT(cpu->instr.nn);
			regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg2));
			updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
			break;

		case MOVI:
			regWrite(cpu->memory->regs, GET_NNN_BIT(cpu->instr.nnn), cpu->instr.nn);
			updateZOFlags(cpu, regRead(cpu->memory->regs, GET_NNN_BIT(cpu->instr.nnn)));
			break;

		case ADD:
			reg1 = GET_NNN_BIT(cpu->instr.nnn);
			reg2 = GET_NN_BIT(cpu->instr.nn);
			regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) + regRead(cpu->memory->regs, reg2));
			updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
			break;

		case SUB:
			reg1 = GET_NNN_BIT(cpu->instr.nnn);
			reg2 = GET_NN_BIT(cpu->instr.nn);
			regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) - regRead(cpu->memory->regs, reg2));
			updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
			break;

		case MUL:
			reg1 = GET_NNN_BIT(cpu->instr.nnn);
			reg2 = GET_NN_BIT(cpu->instr.nn);
			regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) * regRead(cpu->memory->regs, reg2));
			updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
			break;

		case DIV:
			reg1 = GET_NNN_BIT(cpu->instr.nnn);
			reg2 = GET_NN_BIT(cpu->instr.nn);
			regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) / regRead(cpu->memory->regs, reg2));
			updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
			break;

		case IDC: // inc | dec commands
			reg1 = GET_NN_BIT(cpu->instr.nn);
			switch (GET_NNN_BIT(cpu->instr.nnn)) {
				case 0: // inc
					regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) + 1);
					break;
				case 1: // dec
					regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) - 1);
					break;
			}
			updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
			break;

		case AND:
			reg1 = GET_NNN_BIT(cpu->instr.nnn);
			reg2 = GET_NN_BIT(cpu->instr.nn);
			regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) & regRead(cpu->memory->regs, reg2));
			updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
			break;

		case OR:
			reg1 = GET_NNN_BIT(cpu->instr.nnn);
			reg2 = GET_NN_BIT(cpu->instr.nn);
			regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) | regRead(cpu->memory->regs, reg2));
			updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
			break;

		case SETJMPA:
			cpu->memory->jump_address = cpu->instr.nnn;
			break;

		case SHIFT:
			reg1 = GET_NN_BIT(cpu->instr.nn);
			reg2 = GET_N_BIT(cpu->instr.n);
			switch (GET_NNN_BIT(cpu->instr.nnn)) {
				case 0: // Left shift
					regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) << regRead(cpu->memory->regs, reg2));
					break;
				case 1: // Right shift
					regWrite(cpu->memory->regs, reg1, regRead(cpu->memory->regs, reg1) >> regRead(cpu->memory->regs, reg2));
					break;
			}
			updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
			break;
		
		case JMP:
			switch (GET_NNN_BIT(cpu->instr.nnn)) {
				case 0: // jmp
					cpu->memory->pc = cpu->memory->jump_address;
					break;
				case 1: // jz
					if (cpu->flag.z == 0)
						cpu->memory->pc = cpu->memory->jump_address;
					break;
				case 2: // jnz
					if (cpu->flag.z != 0)
						cpu->memory->pc = cpu->memory->jump_address;
					break;
				case 3: // je
					if (cpu->flag.e == 0)
						cpu->memory->pc = cpu->memory->jump_address;
					break;
				case 4: // jne
					if (cpu->flag.e != 0)
						cpu->memory->pc = cpu->memory->jump_address;
					break;
				case 5: // jgt
					if (cpu->flag.e == 1)
						cpu->memory->pc = cpu->memory->jump_address;
					break;
				case 6: // jls
					if (cpu->flag.e == -1)
						cpu->memory->pc = cpu->memory->jump_address;
					break;
					break;
				case 7: // jo
					if (cpu->flag.o == 0)
						cpu->memory->pc = cpu->memory->jump_address;
					break;
				case 8: //jno
					if (cpu->flag.o != 0)
						cpu->memory->pc = cpu->memory->jump_address;
					break;
			}
			clearFlags(cpu);
			break;

		case CMP:
			reg1 = GET_NNN_BIT(cpu->instr.nnn);
			reg2 = GET_NN_BIT(cpu->instr.nn);
			updateEFlag(cpu, regRead(cpu->memory->regs, reg1), regRead(cpu->memory->regs, reg2));
			break;

		case SCALL:
			switch (GET_NNN_BIT(cpu->instr.nnn)) {
				case 0: // printf
					cpu->is_printing = true;
					switch (GET_NN_BIT(cpu->instr.nn)) {
						case 0: // output as char
							printf("%ñ", regRead(cpu->memory->regs, GET_N_BIT(cpu->instr.n)));
							break;
						case 1: // output as int
							printf("%i", regRead(cpu->memory->regs, GET_N_BIT(cpu->instr.n)));
							break;
					}
					break;
				case 1: // scanf
					break;
				case 2: // push
					reg1 = GET_NN_BIT(cpu->instr.nn);
					push(cpu->memory->stack, regRead(cpu->memory->regs, reg1));
					break;
				case 3: // pop
					reg1 = GET_NN_BIT(cpu->instr.nn);
					regWrite(cpu->memory->regs, reg1, pop(cpu->memory->stack));
					updateZOFlags(cpu, regRead(cpu->memory->regs, reg1));
					break;
			}
			break;
	}
}

// Print all registers
void printRegs(cpu_t* cpu) {
	printf("===REGS===\n");
	for (int i = 0; i < RCNT; i++)
		printf("R[%i] = %03i\n", i, cpu->memory->regs[i]);
	printf("==========\n");
}

// Print CPU flags
void printFlags(cpu_t* cpu) {
	printf("FLAGS: Equal(e)=%i Zero(z)=%i Overflow(o)=%i\n", cpu->flag.e, cpu->flag.z, cpu->flag.o);
}

// Print stack
void printStack(cpu_t* cpu) {
	printf("STACK: ");
	for (size_t i = 0; i < cpu->memory->stack->p_st; ++i)
		printf("%04i ", cpu->memory->stack->data[i]);
	printf("\n");
}

// Print memory
void printMemory(cpu_t* cpu) {
	printf("=======MEMORY=======\n");
	for (uint16_t i = 0; i < cpu->program_size; ++i)
		printf("mem[0x%02X] = 0x%04X\n", PROGRAM_ADDRESS + i, memoryRead(cpu->memory, i, PROGRAM_ADDRESS));
	printf("====================\n");
}

void printCpuInfo(cpu_t* cpu, uint8_t mode) {
	if (cpu->is_printing) {
		printf("\n");
		cpu->is_printing = false;
	}
	if (mode & PRINT_MEM) printMemory(cpu);
	if (mode & PRINT_REGS) printRegs(cpu);
	if (mode & PRINT_STACK) printStack(cpu);
	if (mode & PRINT_FLAGS) printFlags(cpu);
}