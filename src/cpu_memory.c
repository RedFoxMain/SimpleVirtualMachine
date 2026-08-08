#include "cpu_memory.h"
#include "error.h"

memory_t* init_memory() {
    memory_t* temp = (memory_t*)malloc(sizeof(memory_t));
    if(!temp)
        throw_error("Can't allocate memory for cpu!\n");
    memset(temp->data, 0, MEM_SIZE);
    memset(temp->regs, 0, RCNT);
    temp->jump_address = -1;
    temp->pc = 0;
    temp->sp = STACK_BASE_OFFSET;
    return temp;
}

instruction_t create_instruction(uint16_t value) {
    instruction_t temp = {
        .opcode = value >> 12,
        .nnn = value & 0x0FFF,
        .nn = value & 0x00FF,
        .n = value & 0x000F
    };
    return temp;
}

void memory_write(memory_t* memory, int offset, uint16_t value) {
    int base_offset = offset + MEMORY_BASE_OFFSET;
    if (base_offset > MEMORY_END_OFFSET)
        throw_error("Memory overflow!\n");
    memory->data[base_offset] = value;
}

uint16_t memory_read(memory_t* memory, int offset) {
    return memory->data[offset + MEMORY_BASE_OFFSET];
}

instruction_t memory_read_instruction(memory_t* memory) {
    return create_instruction(memory->data[memory->pc]);
}

void reg_write(uint8_t* regs, int reg, uint8_t value) {
    regs[reg] = value;
}

uint8_t reg_read(uint8_t* regs, int reg) {
    return regs[reg];
}

void push(memory_t* memory, uint16_t value) {
	if(memory->sp < MEMORY_END_OFFSET)
		throw_error("Stack overflow\n");
	memory->data[--memory->sp] = value;
}

uint16_t pop(memory_t* memory) {
	if(memory->sp == STACK_BASE_OFFSET)
		throw_error("Stack is empty\n");
	return memory->data[memory->sp++];
}