# SimpleVirtualMachine
## Table Of Content
- [About SVM](#about-svm)
- [Build & Run](#build--run)
  - [Dependencies](#dependencies)
  - [Build](#build)
  - [Run](#run)
- [Architecture](#architecture)
  - [Code](#code)
  - [Memory](#memory)
  - [Registers](#registers)
  - [Flags](#flags)
- [Opcodes table](#opcodes-table)
- [System call](#system-call)

## About SVM
SVM is a simple educational project created in C.<br>
Features:
- The virtual machine has its own [assembler language](https://github.com/RedFoxMain/smasm.git)
- 16 basic commands
- 7 registers of type uint8_t
- 4 flags
- A stack of 1 kB in size of type uint16_t
- Free memory 1 kB in size of type uint16_t

## Build & Run
### Dependencies
- Make
- Clang (or GCC)

### Build
```bash
git clone https://github.com/RedFoxMain/SimpleVirtualMachine.git
cd SimpleVirtualMachine
make all
```

### Run

Before running the executable file, you need to specify the file (using the f flag) that will be executed.
Example:
```bash
SimpleVirtualMachine -fprograms/array_test.bin
```
If you want to view all the debugging information, you can check the -i option. Example:
```bash
SimpleVirtualMachine -fprograms/array_test.bin -i
```
For more information about flags, you can read by typing:
```bash
SimpleVirtualMachine -h
```

## Architecture
### Code
The Vm reads *.bin file and then loads all the code at address 0x0000.
By default, the Vm can store 2048(2 kB) different commands. <br>
I think that in the future, I might allow the user to decide where he may allocate memory, but for now, the Vm has only 4 kB of memory.

### Memory
Memory is linear and divided into three segments.
| Segment |  Address range  | Permission   |
| ------- | --------------- | ------------ |
| Code    | 0x0000 - 0x07FF | Only read    |
| Memory  | 0x0800 - 0x0BFF | Read & Write |
| Stack   | 0x0C00 - 0x0FFF | Read & Write |

### Registers
There are 7 registers in the Vm. From R0 to R6. Each register has it's own purpose. <br>
| Register | Size     | Purpose |
| -------- | -------- | ------- |
| R0-R3    | uint8_t  | Mathematical registers |
| R4       | uint8_t  | Counter  |
| R5       | uint8_t  | Stores the data address for system operations  |
| R6       | uint8_t  | Stores the system operation code |

### Flags
By default, the Vm has 4 flags. <br>
The flags are updated after executing most instructions (except for hlt, ret, call, and jumps).
| Flag name       | Short name | Range of values | Description |
| --------------- | ---- | --------------------- | ----------- |
| Program counter | pc   | 0 - program size      | The pc flag is updated after each instruction, except for hlt, ret, and call |
| Equal           | e    | -1 - 1                | If left value is less than the right e=-1, else e=1, if they are equal e=0 |
| Zero            | z    | 0 - 1                 | If the result of the operations is not equal to zero z=0 else z=1 |
| Overflow        | o    | 0 - 1                 | If the result of the operations is an overflow, o=1, else o=0 |

## Opcodes Table
| Assembler mnemonic example | Operands | Opcode (15 - 12) | 11 - 8      | 7 - 4      | 3 - 0       | Description |
| -------------------------: | :------: | ---------------- | ----------- | ---------- | ----------- | --- |
| hlt                        | —        | 0                | 0           | 0          | 0           | Interrupts the execution of the code |
| clf                        | —        | 0                | 1           | 0          | 0           | Clear all flags |
| clr                        | —        | 0                | 2           | 0          | 0           | Reset all registers |
| mov                        | reg, reg | 1                | 0-6         | 0-6        | 0           | Move the value of the right register to the left register |
| movi                       | reg, val | 2                | 0-6         | 0-FF       |             | Move the value of 7-0 to the left register |
| add                        | reg, reg | 3                | 0           | 0-6        | 0-6         | Addition of two registers. The result is in the left one |
| sub                        | reg, reg | 3                | 1           | 0-6        | 0-6         | Subtraction of two registers. The result is in the left one |
| mul                        | reg, reg | 3                | 2           | 0-6        | 0-6         | Multiplication of two registers. The result is in the left one |
| div                        | reg, reg | 3                | 3           | 0-6        | 0-6         | Division of two registers. Result in the left one |
| inc                        | reg      | 4                | 0           | 0-6        | 0           | Increases the value of the register |
| dec                        | reg      | 4                | 1           | 0-6        | 0           | Reduces the value of the register |
| and                        | reg, reg | 5                | 0           | 0-6        | 0-6         | Bitwise AND, result in the left register | 
| or                         | reg, reg | 5                | 1           | 0-6        | 0-6         | Bitwise OR, result in the left register |
| xor                        | reg, reg | 5                | 2           | 0-6        | 0-6         | Bitwise XOR, result in the left register |
| shl                        | reg, reg | 6                | 0           | 0-6        | 0-6         | Left shift, result in the left register  |
| shr                        | reg, reg | 6                | 1           | 0-6        | 0-6         | Right shift, result in the left register |
| setjmpa                    | val      | 7                | 0-FFF       |            |             | Sets the value of 11-0 to the jump address |
| jmp 			                 | —        | 8                | 0	         | 0		      | 0	          | Unconditional transition |
| jz 			                   | —        | 8                | 1	         | 0		      | 0	          | Transition if z = 0 |
| jnz 			                 | —        | 8                | 2	         | 0 	        | 0	          | Transition if z != 0 |
| je 			                   | —        | 8                | 3	         | 0		      | 0	          | Transition if e = 0 |
| jne 			                 | —        | 8                | 4	         | 0		      | 0	          | Transition if e != 0 |
| jo 			                   | —        | 8                | 5	         | 0		      | 0	          | Transition if o = 0 |
| jno 			                 | —        | 8                | 6	         | 0		      | 0	          | Transition if o != 0 |
| jls 			                 | —        | 8                | 7	         | 0		      | 0	          | Transition if e < 0 |
| jgt 			                 | —        | 8                | 8	         | 0		      | 0	          | Transition if e > 0 |
| cmp                        | reg, reg | 9                | 0-6         | 0-6        | 0           | Compares two registers and sets the E flag |
| lda                        | reg, reg | A                | 0-6         | 0-6        | 0           | Loads the value from memory at the address in the right register into the left register |
| str                        | reg, reg | B                | 0-6         | 0-6        | 0           | Saves the value of the right register at the address from the left register to memory |
| push                       | reg      | C                | 0           | 0-6        | 0           | Places the register value at the end of the stack |
| pop                        | reg      | C                | 1           | 0-6        | 0           | Gets value from top of the stack in the register |
| scall                      | —        | D                | 0           | 0          | 0           | System operations |
| ret			                   | —        | E                | 0           | 0          | 0           | Returns from the subroutine |
| call                       | —        | F                | 0           | 0          | 0           | Calls the subroutine |

> [!NOTE]
> The call operation will save the current pc to the stack. <br>
> The ret operation retrieves the last value from the stack and assigns this value to the pc.

## System call
System call operations depend on register R6. The address offset relative to the data for the operation must be stored in register R5.
| Operation | Code | Description            |
| --------- | ---- | ---------------------- |
| putc      | 0    | Print value as char    |
| puti      | 1    | Print value as integer |
| puth      | 2    | Print value as hex     |
| puts      | 3    | Print string           |
