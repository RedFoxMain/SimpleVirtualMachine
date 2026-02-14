#include "stack.h"

void initStack(stack_t* stack) {
    memset(stack->data, 0, STACK_DEPTH);
}

void push(stack_t* stack, uint8_t value) {
	stack->data[stack->p_st++] = value;
}

uint8_t pop(stack_t* stack) {
	uint8_t value = stack->data[stack->p_st - 1];
	stack->data[stack->p_st--] = 0;
	return value;
}