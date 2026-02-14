#include <stdio.h>
#include <stdbool.h>
#include "../cpu/cpu.h"

bool compare(uint8_t* arr1, uint8_t* arr2) {
	for (int i = 0; i < REG_COUNT; ++i) {
		if (arr1[i] != arr2[i])
			return false;
	}
	return true;
}

cpu_t* runCpu(const char* file_name) {
	// Init CPU
	cpu_t cpu = { 0 };
	initCPU(&cpu);
	loadProgram(&cpu, readFromFile(file_name));
	while (cpu.is_running) {
		decode(&cpu);
	}
	//printCpuInfo(&cpu, PRINT_REGS);
	return &cpu;
}

/* TEST FUNCTIONS */
void testOp(const char* test_name, cpu_t* cpu, uint8_t* result) {
	if (compare(cpu->regs, result)) { printf("%s op test passed!\n", test_name); return; }
	printf("%s op test not passed!\n", test_name); exit(1);
}

void testFlag(const char* test_name, bool flag, bool result) {
	if (flag == result) { printf("%s op test passed!\n", test_name); return; }
	printf("%s op test not passed!\n", test_name); exit(1);
}

void testValue(const char* test_name, cpu_t* cpu, uint8_t result) {
	if (cpu->regs[R0] == result) { printf("%s op test passed!\n", test_name); return; }
	printf("%s op test not passed!\n", test_name); exit(1);
}

void testStack(const char* test_name, cpu_t* cpu, size_t index, uint8_t result) {
	if (cpu->stack[index] == result) { printf("%s op test passed!\n", test_name); return; }
	printf("%s op test not passed!\n", test_name); exit(1);
}

/* MAIN METHODS */
void testMathCase() {
	printf("START TESTING MATH OPS\n");

	uint8_t add_reg_test[] = { 1, 2, 3, 0, 0, 0 };
	testOp("Add", runCpu("../src/test/programs/add_reg.txt"), add_reg_test);

	uint8_t add_value_test[] = { 0, 10, 25, 0, 0, 35 };
	testOp("Add", runCpu("../src/test/programs/add_value.txt"), add_value_test);

	uint8_t sub_reg_test[] = { 5, 2, 3, 0, 0, 0 };
	testOp("Sub", runCpu("../src/test/programs/sub_reg.txt"), sub_reg_test);

	uint8_t sub_value_test[] = { 0, 10, 25, 0, 0, 15 };
	testOp("Sub", runCpu("../src/test/programs/sub_value.txt"), sub_value_test);

	uint8_t mul_reg_test[] = { 5, 2, 10, 0, 0, 0 };
	testOp("Mul", runCpu("../src/test/programs/mul_reg.txt"), mul_reg_test);

	uint8_t mul_value_test[] = { 0, 10, 25, 0, 0, 250 };
	testOp("Mul", runCpu("../src/test/programs/mul_value.txt"), mul_value_test);

	uint8_t div_reg_test[] = { 10, 2, 5, 0, 0, 0 };
	testOp("Div", runCpu("../src/test/programs/div_reg.txt"), div_reg_test);

	uint8_t div_value_test[] = { 0, 250, 10, 0, 0, 25 };
	testOp("Div", runCpu("../src/test/programs/div_value.txt"), div_value_test);

	uint8_t inc_test[] = { 1, 0, 0, 0, 0, 0 };
	testOp("Inc", runCpu("../src/test/programs/inc.txt"), inc_test);

	uint8_t dec_test[] = { 2, 1, 0, 0, 0, 0 };
	testOp("Dec", runCpu("../src/test/programs/dec.txt"), dec_test);

	printf("END TESTING MATH OPS\n\n");
}

void testLogicCase() {
	printf("START TESTING LOGIC OPS\n");

	testFlag("And", runCpu("../src/test/programs/and.txt")->flag.l, 1);
	testFlag("Or", runCpu("../src/test/programs/or.txt")->flag.l, 1);
	testFlag("Xor", runCpu("../src/test/programs/xor.txt")->flag.l, 0);

	printf("END TESTING LOGIC OPS\n\n");
}

void testOtherCase() {
	printf("START TESTING OTHER OPS\n");

	// Shift test
	testValue("Shift Left", runCpu("../src/test/programs/shift_left.txt"), 10);
	testValue("Shift Right", runCpu("../src/test/programs/shift_right.txt"), 2);

	// Stack op test
	testStack("Push", runCpu("../src/test/programs/push.txt"), 0, 10);
	testStack("Push reg", runCpu("../src/test/programs/push_reg.txt"), 0, 10);
	testValue("Pop", runCpu("../src/test/programs/pop.txt"), 10);

	printf("END TESTING OTHER OPS\n");
}

/* MAIN */
int main(int argc, char* argv[]) {
	testMathCase();
	testLogicCase();
	testOtherCase();
}