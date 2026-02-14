#ifndef CPU_MEMORY_H
#define CPU_MEMORY_H

#pragma clang diagnostic ignored "-Wmissing-declarations"

#include <stdint.h>
#include <string.h>
#include "stack/stack.h"

#define PRINT_MEM   0b0001
#define PRINT_REGS  0b0010
#define PRINT_FLAGS 0b0100
#define PRINT_STACK 0b1000

#define PROGRAM_ADDRESS 0x200
#define MEM_SIZE 4 * 1024

#define GET_NNN_BIT(value) ((value >> 8) & 0x000F)
#define GET_NN_BIT(value) ((value >> 4) & 0x000F)
#define GET_N_BIT(value) (value & 0x000F)

/*
* R0 - Register for math
* R1 - Register for math
* R2 - Register for math
* R3 - Register for math
* R4 - Counter
* R5 - SYSCALL operation
*/
typedef enum {
	R0, R1, R2, 
	R3, R4, R5, 
	RCNT
};

// VM flags
typedef struct {
	int e;
	int o;
	int z;
} flags_t;

// Parse instruction
typedef struct {
	uint16_t nnn;
	uint8_t nn;
	uint8_t n;
} instruction_t;

// Memory
typedef struct {
    uint16_t data[MEM_SIZE];
    uint16_t pc;
	uint32_t jump_address;
    uint8_t regs[RCNT];
    stack_t* stack;
} memory_t;

// Initialize cpu memory and stack
void initMemory(memory_t* memory);

// Work with memory
void memoryWrite(memory_t* memory, int index, uint16_t value);
uint16_t memoryRead(memory_t* memory, int index, int offset);

// Work with registers
void regWrite(uint8_t* regs, int reg, uint8_t value);
uint8_t regRead(uint8_t* regs, int reg);
#endif // !CPU_MEMORY_H