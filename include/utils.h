#ifndef CPU_UTILS_H
#define CPU_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "cpu_memory.h"

void print_string(memory_t* memory, uint16_t address);

#endif // !CPU_UTILS_H