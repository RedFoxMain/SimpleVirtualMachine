#ifndef CPU_STACK_H
#define CPU_STACK_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STACK_DEPTH 30 * sizeof(uint8_t)

typedef struct {
    uint8_t data[STACK_DEPTH];
	int p_st;
} stack_t;

// Init stack
void initStack(stack_t* stack);

// Stack push
void push(stack_t* cpu, uint8_t value);

// Stack pop
uint8_t pop(stack_t* cpu);

#endif // !#ifndef CPU_STACK_H