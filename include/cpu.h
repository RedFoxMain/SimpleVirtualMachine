#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "cpu_memory.h"
#include "utils.h"

#define IS_ZERO(value) (!(value) ? 1 : 0)

typedef enum {
		OPC_HALT = 0, 
        OPC_MOV = 1, 
        OPC_MOVI = 2,
        OPC_MTH = 3,
        OPC_IDC = 4,
        OPC_LGC = 5,
        OPC_SHIFT = 6,
        OPC_SETJMPA = 7,
		OPC_JMP = 8, 
        OPC_CMP = 9, 
        OPC_LDA = 10,
        OPC_STR = 11,
        OPC_STK = 12,
        OPC_SCALL = 13,
        OPC_RET = 14,
        OPC_CALL = 15
} opcode_t;

typedef enum {
        // OPC_HALT
        OPCM_HALT = 0,
        OPCM_CLF = 1,
        OPCM_CLR = 2,
        // OPC_MTH
        OPCM_ADD = 0,
        OPCM_SUB = 1,
        OPCM_MUL = 2,
        OPCM_DIV = 3,
        // OPC_LGC
        OPCM_AND = 0,
        OPCM_OR = 1,
        OPCM_XOR = 2,
        // OPC_SHIFT
        OPCM_LSHIFT = 0,
        OPCM_RSHIFT = 1,
        // OPC_IDC
        OPCM_INC = 0,
        OPCM_DEC = 1,
        // OPC_JMP
        OPCM_JMP = 0,
        OPCM_JZ = 1,
        OPCM_JNZ = 2,
        OPCM_JE = 3,
        OPCM_JNE = 4,
        OPCM_JO = 5,
        OPCM_JNO = 6,
        OPCM_JLS = 7,
        OPCM_JGT = 8,
        // OPC_STK
        OPCM_PUSH = 0,
        OPCM_POP = 1,
        // OPCM_SCALL
        OPCM_PUTC = 0,
        OPCM_PUTI = 1,
        OPCM_PUTH = 2,
        OPCM_PUTS = 3,
        OPCM_PRINTF = 4,
        OPCM_SCANF = 5,
} opcode_mode_t;

typedef enum {
	OT_ADD, OT_SUB, OT_MUL
} overflow_type_t;

typedef struct {
	bool is_running;
	memory_t* memory;
	flags_t flag;
	size_t program_size;
} cpu_t;

void init_cpu(cpu_t* cpu);
void load_program(cpu_t* cpu, const char* file_name);
void decode(cpu_t* cpu);
void destroy_cpu(cpu_t* cpu);
void print_cpu_info(cpu_t* cpu, debug_info_t mode);

#endif // !CPU_H