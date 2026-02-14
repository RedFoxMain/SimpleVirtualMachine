#include "cpu_memory.h"

void initMemory(memory_t* memory) {
    if (memory != NULL) {
        memset(&memory->data, 0, MEM_SIZE);
        memset(memory->regs, 0, RCNT);
        memory->pc = PROGRAM_ADDRESS;
        memory->jump_address = PROGRAM_ADDRESS;

        memory->stack = (stack_t*)malloc(sizeof(stack_t));
        if (memory->stack != NULL) {
            initStack(memory->stack);
            memory->stack->p_st = 0;
        } else {
            printf("Cannot allocate memory for CPU stack!\n");
            exit(1);
        }
    } else {
        printf("Cannot allocate memory for CPU!\n");
        exit(1);
    }
}

void memoryWrite(memory_t* memory, int index, uint16_t value) {
    memory->data[PROGRAM_ADDRESS + index] = value;
}

uint16_t memoryRead(memory_t* memory, int index, int offset) {
    return memory->data[index + offset];
}

void regWrite(uint8_t* regs, int reg, uint8_t value) {
    regs[reg] = value;
}

uint8_t regRead(uint8_t* regs, int reg) {
    return regs[reg];
}