#ifndef CPU_MEMORY_H
#define CPU_MEMORY_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MEM_SIZE 4 * 1024
#define CODE_END_OFFSET 0x07FF
#define MEMORY_BASE_OFFSET 0x0800
#define MEMORY_END_OFFSET 0x0BFF
#define STACK_END_OFFSET 0x0C00
#define STACK_BASE_OFFSET 0x0FFF

#define GET_NNN_BIT(value) ((value) >> 8 & 0x000F)
#define GET_NN_BIT(value) ((value) >> 4 & 0x000F)
#define GET_N_BIT(value) ((value) & 0x000F)

typedef enum {
	PRINT_MEM    = 1 << 0,
	PRINT_CODE   = 1 << 1,
	PRINT_REGS   = 1 << 2,
	PRINT_FLAGS  = 1 << 3,
	PRINT_STACK  = 1 << 4
} debug_info_t;

/*
* R0 - Register for math
* R1 - Register for math
* R2 - Register for math
* R3 - Register for math
* R4 - Counter
* R5 - Scall data address
* R6 - Scall operation
*/

typedef enum {
	R0, R1, R2, 
	R3, R4, R5, 
	R6, RCNT
} regs_t;

typedef struct {
	int e;
	int o;
	int z;
} flags_t;

typedef struct {
	uint8_t opcode;
	uint16_t nnn;
	uint8_t nn;
	uint8_t n;
} instruction_t;

typedef struct {
    uint16_t data[MEM_SIZE];
    uint16_t pc;
	uint16_t sp;
	int32_t jump_address;
    uint8_t regs[RCNT];
} memory_t;

memory_t* init_memory();
instruction_t create_instruction(uint16_t value);
void memory_write(memory_t* memory, int offset, uint16_t value);
uint16_t memory_read(memory_t* memory, int offset);
void reg_write(uint8_t* regs, int reg, uint8_t value);
uint8_t reg_read(uint8_t* regs, int reg);
void push(memory_t* memory, uint16_t value);
uint16_t pop(memory_t* memory);

#endif // !CPU_MEMORY_H