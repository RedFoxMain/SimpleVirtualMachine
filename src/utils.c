#include "../include/utils.h"

void print_string(memory_t* memory, uint16_t address) {
    uint8_t chr;
    while ((chr = memory_read(memory, address++)) != 0)
        printf("%c", chr);
}