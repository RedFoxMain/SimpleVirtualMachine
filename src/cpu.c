#include "cpu.h"
#include "error.h"

void load_program(cpu_t* cpu, const char* file_name) {
	FILE* file;
	if(fopen_s(&file, file_name, "rb") != 0)
		throw_error("Can't find file: %s\n", file_name);
	fseek(file, 0, SEEK_END);
	size_t file_size = (size_t)ftell(file);
	rewind(file);
	uint16_t* buffer = (uint16_t*)malloc(file_size);
	if(!buffer) {
		fclose(file);
		throw_error("Can't allocate memory for cpu buffer!\n");
	}
	size_t readed_data = fread(buffer, 1, file_size, file);
	if (readed_data != file_size) {
		fclose(file);
		throw_error("%s: Read(%zu) != Write(%zu)\n", file_name, readed_data, file_size);
	}
	fclose(file);
	cpu->program_size = readed_data / sizeof(uint16_t);
	if(cpu->program_size > CODE_END_OFFSET)
		throw_error("Not enough memory for code! Max memory size: %zu\n", CODE_END_OFFSET - 1);
	memcpy(cpu->memory->data, buffer, file_size);
}

void clear_flags(cpu_t* cpu) {
	cpu->flag.z = 1;
	cpu->flag.o = 0;
	cpu->flag.e = -2;
}

void init_cpu(cpu_t* cpu) {
	cpu->program_size = 0;
	cpu->memory = init_memory();
	cpu->is_running = true;
	clear_flags(cpu);
}

void destroy_cpu(cpu_t* cpu) {
	free(cpu->memory);
}

int update_eflag(uint8_t val1, uint8_t val2) {
	if (val1 == val2) return 0;
	return (val1 > val2) ? 1 : -1;
}

bool is_overflow(uint8_t value1, uint8_t value2, overflow_type_t type) {
	if(type == OT_ADD) return value1 > 0xFF - value2;
	if(type == OT_SUB) return value1 < value2;
	return ((value1 == 0 || value2 == 0) ? 0 : value1 > 0xFF / value2);
}

void update_zflag(cpu_t* cpu, uint8_t reg) {
	cpu->flag.z = IS_ZERO(reg_read(cpu->memory->regs, reg));
}

void decode(cpu_t* cpu) {
	uint8_t src_reg_index = 0;
	uint8_t reg1_value = 0, reg2_value = 0;
	instruction_t instr = create_instruction(memory_read_instruction(cpu->memory, cpu->memory->pc));
	switch (instr.opcode) {
		case OPC_HALT:
			switch (GET_NNN_BIT(instr.nnn)) {
				case OPCM_HALT:
					cpu->is_running = false;
					cpu->memory->jump_address = -1;
					break;
				case OPCM_CLF:
					clear_flags(cpu);
					cpu->memory->pc++;
					break;
				case OPCM_CLR:
					memset(cpu->memory->regs, 0, RCNT);
					cpu->memory->pc++;
					break;
			}
			break;

		case OPC_MOV:
			clear_flags(cpu);
			src_reg_index = GET_NNN_BIT(instr.nnn);
			reg_write(cpu->memory->regs, src_reg_index, reg_read(cpu->memory->regs, GET_NN_BIT(instr.nn)));
			update_zflag(cpu, src_reg_index);
			cpu->memory->pc++;
			break;

		case OPC_MOVI:
			clear_flags(cpu);
			src_reg_index = GET_NNN_BIT(instr.nnn);
			reg_write(cpu->memory->regs, src_reg_index, instr.nn);
			update_zflag(cpu, src_reg_index);
			cpu->memory->pc++;
			break;
		
		case OPC_MTH:
			clear_flags(cpu);
			src_reg_index = GET_NN_BIT(instr.nn);
			reg1_value = reg_read(cpu->memory->regs, src_reg_index);
			reg2_value = reg_read(cpu->memory->regs, GET_N_BIT(instr.n));
			switch (GET_NNN_BIT(instr.nnn)) {
				case OPCM_ADD:
					reg_write(cpu->memory->regs, src_reg_index, reg1_value + reg2_value);
					cpu->flag.o = is_overflow(reg1_value, reg2_value, OT_ADD);
					break;
				case OPCM_SUB:
					reg_write(cpu->memory->regs, src_reg_index, reg1_value - reg2_value);
					cpu->flag.o = is_overflow(reg1_value, reg2_value, OT_SUB);
					break;
				case OPCM_MUL:
					reg_write(cpu->memory->regs, src_reg_index, reg1_value * reg2_value);
					cpu->flag.o = is_overflow(reg1_value, reg2_value, OT_MUL);
					break;
				case OPCM_DIV:
					if (!reg2_value) {
						printf("DivisionByZero");
						cpu->is_running = false;
					}
					reg_write(cpu->memory->regs, src_reg_index, reg1_value / reg2_value);
					break;
			}
			update_zflag(cpu, src_reg_index);
			cpu->memory->pc++;
			break;

		case OPC_IDC: // inc | dec commands
			clear_flags(cpu);
			src_reg_index = GET_NN_BIT(instr.nn);
			reg1_value = reg_read(cpu->memory->regs, src_reg_index);
			switch (GET_NNN_BIT(instr.nnn)) {
				case OPCM_INC: // inc
					reg_write(cpu->memory->regs, src_reg_index, reg1_value + 1);
					cpu->flag.o = is_overflow(reg1_value, reg1_value - 1, OT_ADD);
					break;
				case OPCM_DEC: // dec
					reg_write(cpu->memory->regs, src_reg_index, reg1_value - 1);
					cpu->flag.o = is_overflow(reg1_value + 1, reg1_value, OT_SUB);
					break;
			}
			update_zflag(cpu, src_reg_index);
			cpu->memory->pc++;
			break;
		
		case OPC_LGC:
			clear_flags(cpu);
			src_reg_index = GET_NN_BIT(instr.nn);
			reg1_value = reg_read(cpu->memory->regs, src_reg_index);
			reg2_value = reg_read(cpu->memory->regs, GET_N_BIT(instr.n));
			switch (GET_NNN_BIT(instr.nnn)) {
				case OPCM_AND:
					reg_write(cpu->memory->regs, src_reg_index, reg1_value & reg2_value);
					break;
				case OPCM_OR:
					reg_write(cpu->memory->regs, src_reg_index, reg1_value | reg2_value);
					break;
				case OPCM_XOR:
					reg_write(cpu->memory->regs, src_reg_index, reg1_value ^ reg2_value);
					break;
			}
			update_zflag(cpu, src_reg_index);
			cpu->memory->pc++;
			break;

		case OPC_SHIFT:
			clear_flags(cpu);
			src_reg_index = GET_NN_BIT(instr.nn);
			reg1_value = reg_read(cpu->memory->regs, src_reg_index);
			reg2_value = reg_read(cpu->memory->regs, GET_N_BIT(instr.n));
			switch (GET_NNN_BIT(instr.nnn)) {
				case OPCM_LSHIFT: // Left shift
					reg_write(cpu->memory->regs, src_reg_index, reg1_value << reg2_value);
					break;
				case OPCM_RSHIFT: // Right shift
					reg_write(cpu->memory->regs, src_reg_index, reg1_value >> reg2_value);
					break;
			}
			update_zflag(cpu, src_reg_index);
			cpu->memory->pc++;
			break;
		
		case OPC_SETJMPA:
			cpu->memory->jump_address = instr.nnn;
			cpu->memory->pc++;
			break;

		case OPC_JMP:
			if (cpu->memory->jump_address == UINT16_MAX)
				throw_error("Jump address wasn't specified!\n");
			switch (GET_NNN_BIT(instr.nnn)) {
				case OPCM_JMP: // jmp
					cpu->memory->pc = cpu->memory->jump_address;
					break;
				case OPCM_JZ: // jz
					if (cpu->flag.z == 0)
						cpu->memory->pc = cpu->memory->jump_address;
					else cpu->memory->pc++;
					break;
				case OPCM_JNZ: // jnz
					if (cpu->flag.z != 0)
						cpu->memory->pc = cpu->memory->jump_address;
					else cpu->memory->pc++;
					break;
				case OPCM_JE: // je
					if (cpu->flag.e == 0)
						cpu->memory->pc = cpu->memory->jump_address;
					else cpu->memory->pc++;
					break;
				case OPCM_JNE: // jne
					if (cpu->flag.e != 0)
						cpu->memory->pc = cpu->memory->jump_address;
					else cpu->memory->pc++;
					break;
				case OPCM_JO: // jo
					if (cpu->flag.o == 0)
						cpu->memory->pc = cpu->memory->jump_address;
					else cpu->memory->pc++;
					break;
				case OPCM_JNO: //jno
					if (cpu->flag.o != 0)
						cpu->memory->pc = cpu->memory->jump_address;
					else cpu->memory->pc++;
					break;
				case OPCM_JLS: // jls
					if (cpu->flag.e == -1)
						cpu->memory->pc = cpu->memory->jump_address;
					else cpu->memory->pc++;
					break;
				case OPCM_JGT: // jgt
					if (cpu->flag.e == 1)
						cpu->memory->pc = cpu->memory->jump_address;
					else cpu->memory->pc++;
					break;
			}
			cpu->memory->jump_address = UINT16_MAX;
			break;

		case OPC_CMP:
			clear_flags(cpu);
			reg1_value = reg_read(cpu->memory->regs, GET_NNN_BIT(instr.nnn));
			reg2_value = reg_read(cpu->memory->regs, GET_NN_BIT(instr.nn));
			cpu->flag.e = update_eflag(reg1_value, reg2_value);
			cpu->memory->pc++;
			break;
		
		case OPC_LDA:
			clear_flags(cpu);
			reg_write(cpu->memory->regs, R5, instr.nnn);
			cpu->memory->pc++;
			break;

		case OPC_STR:
			clear_flags(cpu);
			reg1_value = reg_read(cpu->memory->regs, GET_NNN_BIT(instr.nnn));
			reg2_value = reg_read(cpu->memory->regs, GET_NN_BIT(instr.nn));
			memory_write(cpu->memory,  reg1_value, reg2_value);
			cpu->memory->pc++;
			break;

		case OPC_STK:
			clear_flags(cpu);
			src_reg_index = GET_NN_BIT(instr.nn);
			switch (GET_NNN_BIT(instr.nnn)) {
				case OPCM_PUSH:
					push(cpu->memory, reg_read(cpu->memory->regs, src_reg_index));
					break;
				case OPCM_POP:
					reg_write(cpu->memory->regs, src_reg_index, pop(cpu->memory));
					break;
			}
			update_zflag(cpu, src_reg_index);
			cpu->memory->pc++;
			break;

		case OPC_SCALL:
			uint16_t data_address = reg_read(cpu->memory->regs, R5);
			switch (reg_read(cpu->memory->regs, R6)) {
				case OPCM_PUTC:
					uint8_t chr;
					if (isprint(chr = memory_read(cpu->memory, data_address)))
						printf("%c", chr);
					break;
				case OPCM_PUTI:
					printf("%i", memory_read(cpu->memory, data_address));
					break;
				case OPCM_PUTH:
					printf("%x", memory_read(cpu->memory, data_address));
					break;
				case OPCM_PUTS:
					print_string(cpu->memory, data_address);
					break;
				case OPCM_PRINTF:
					break;
				case OPCM_SCANF:
					break;
			}
			cpu->memory->pc++;
			break;
		
		case OPC_RET:
			cpu->memory->pc = pop(cpu->memory);
			break;

		case OPC_CALL:
			push(cpu->memory, cpu->memory->pc + 1);
			cpu->memory->pc = cpu->memory->jump_address;
			cpu->memory->jump_address = -1;
			break;
	}
}

void print_regs(cpu_t* cpu) {
	printf("===REGS===\n");
	for (int i = 0; i < RCNT; i++)
		printf("R[%i] = %03i\n", i, cpu->memory->regs[i]);
	printf("==========\n");
}

void print_flags(cpu_t* cpu) {
	printf(
		"FLAGS: Equal(e)=%i Zero(z)=%i Overflow(o)=%i PC=%04X SP=%04X\n", 
		cpu->flag.e, cpu->flag.z, cpu->flag.o, cpu->memory->pc, cpu->memory->sp
	);
}

void print_stack(cpu_t* cpu) {
	printf("STACK: ");
	if(cpu->memory->sp == STACK_BASE_OFFSET) {
		printf("None\n");
		return;
	}
	for (size_t i = cpu->memory->sp; i < STACK_BASE_OFFSET; ++i)
		printf("%04i ", cpu->memory->data[i]);
	printf("\n");
}

void print_code(cpu_t* cpu) {
	printf("=====CODE=====\n");
	for (uint16_t i = 0; i < cpu->program_size; ++i)
		printf("%04i: 0x%04X\n", i, memory_read_instruction(cpu->memory, i));
	printf("==============\n");
}

void print_memory(cpu_t* cpu) {
	printf("====MEMORY====\n");
	for (uint16_t i = MEMORY_BASE_OFFSET; i < MEMORY_END_OFFSET; ++i) {
		uint8_t value = memory_read(cpu->memory, i);
		if (!value) continue;
		printf("0x%04X: 0x%04X\n", i, value);
	}
	printf("==============\n");
}

void print_cpu_info(cpu_t* cpu, debug_info_t mode) {
	printf("\n");
	if (mode & PRINT_CODE) print_code(cpu);
	if (mode & PRINT_MEM) print_memory(cpu);
	if (mode & PRINT_REGS) print_regs(cpu);
	if (mode & PRINT_STACK) print_stack(cpu);
	if (mode & PRINT_FLAGS) print_flags(cpu);
}