# SimpleVirtualMachine
## About SVM
## Memory
### Code
The Vm reads *.bin file and then loads all the code at address 0x0000.
By default, the Vm can store 2048(2kB) different commands. <br>
> [!NOTE]
> I think that in the future, I might allow the user to decide where he may allocate memory. <br>
> But for now, the Vm has 2kB of code, 1 kB of stack and 1 kB of memory.

### Registers
There are 7 registers in the Vm. From R0 to R6. Each register has it's own purpose. <br>
R0-R3 are mathematical registers, <br>
R4 is a counter (for loop), <br>
R5 stores the data address for system operations, <br>
R6 store the system operation code.

### Stack
In a Vm, the stack is a part of memory. The stack is located int the address range 0x0C00 - 0x0FFF.
This means that it can store 1024 values (some of that values may be program return addresses, if you use labels and call) of the uint8_t type.

### Free memory
The free memory is located in the address range 0x0800 - 0x0BFF. <br>
This means that it can store 1024 values of the uint8_t type.

### Flags
By default, the Vm has 4 flags. <br>
| Flag name       | Short name | Range of values |
| --------------- | ---- | --------------------- |
| Program counter | pc   | 0 - program size      |
| Equal           | e    | -1 - 1                |
| Zero            | z    | 0 - 1                 |
| Overflow        | o    | 0 - 1                 |

The pc flag is updated after each instruction, except for hlt, ret, and call. <br>
Before each command, except for scall, ret, and call. setjmpa, jmp(and others), the hlt Vm resets all flags and, after executing the instructions, updates them again. <br>
If the result of the operations is not equal to zero, the z flag is set to 0 or 1.
If the result of the operations is an overflow, the uint8_t o flag is set to 1, otherwise to 0.
You can read about flag e in the note below.
> [!NOTE]
> Why does the e flag have such a range? <br>
> -1 means that the left value is less than the right. <br>
> 0 means that the values are equal. <br>
> 1 means that the left value is greater than the right.

## Opcodes Table
| Assembler mnemonic example | Opcode | Second Byte | Third Byte | Fourth Byte | Description |
| -------------------------:  | ------ | ----------- | ---------- | ----------- | --- |
| hlt                        | 0      | 0           | 0          | 0           | Interrupts the execution of the code |
| clf                        | 0      | 1           | 0          | 0           | Clear all flags |
| clr                        | 0      | 2           | 0          | 0           | Reset all registers |
| mov reg, reg               | 1      | 0-6         | 0-6        | 0           | Move the value of the right register to the left register |
| movi reg, val              | 2      | 0-6         | 0-FF       |             | Move the value of the last to bits to the left register |
| add reg, reg               | 3      | 0           | 0-6        | 0-6         | Adds the value of the right register and the left register and stores the result in the left register |
| sub reg, reg               | 3      | 1           | 0-6        | 0-6         | Subtracts the value of the right register from the left one and stores the result in the left register |
| mul reg, reg               | 3      | 2           | 0-6        | 0-6         | Multiplies the value of the right register by the value of the left register and stores the result in the left register |
| div reg, reg               | 3      | 3           | 0-6        | 0-6         | Divides the value of the right register by the value of the left register and stores the result in the left register |
| inc reg                    | 4      | 0           | 0-6        | 0           | Increases the value of the register |
| dec reg                    | 4      | 1           | 0-6        | 0           | Reduces the value of the register |
| and reg, reg               | 5      | 0           | 0-6        | 0-6         | Performs a bitwise AND operation and stores the result in the left register |
| or reg, reg                | 5      | 1           | 0-6        | 0-6         | Performs a bitwise OR operation and stores the result in the left register |
| xor reg, reg               | 5      | 2           | 0-6        | 0-6         | Performs a bitwise XOR operation and stores the result in the left register |
| shl reg, reg               | 6      | 0           | 0-6        | 0-6         | Performs a left shift of the value in the left register by the value from the right register and stores the result in the left register  |
| shr reg, reg               | 6      | 1           | 0-6        | 0-6         | Performs a right shift of the value in the left register by the value from the right register and stores the result in the left register |
| setjmpa value              | 7      | 0-FFF       |            |             | Sets the jump address |
| jmp 			     | 8      | 0	    | 0		 | 0	       | Unconditional transition |
| jz 			     | 8      | 1	    | 0		 | 0	       | Transition if the the Z flag is 0 |
| jnz 			     | 8      | 2	    | 0 	 | 0	       | Transition if the the Z flag is not 0 |
| je 			     | 8      | 3	    | 0		 | 0	       | Transition if the the E flag is 0 |
| jne 			     | 8      | 4	    | 0		 | 0	       | Transition if the the E flag is not 0 |
| jo 			     | 8      | 5	    | 0		 | 0	       | Transition if the the O flag is 0 |
| jno 			     | 8      | 6	    | 0		 | 0	       | Transition if the the O flag is not 0 |
| jls 			     | 8      | 7	    | 0		 | 0	       | Transition if the the E flag is less than 0 |
| jgt 			     | 8      | 8	    | 0		 | 0	       | Transition if the the E flag is greater than 0 |
| cmp reg, reg               | 9      | 0-6         | 0-6        | 0           | Compares two registers and sets the E flag |
| lda reg, reg               | A      | 0-6         | 0-6        | 0           | Loads the value from memory at the address in the right register into the left register |
| str reg, reg               | B      | 0-6         | 0-6        | 0           | Saves the value of the right register at the address from the left register to memory |
| push reg                   | C      | 0           | 0-6        | 0           | Places the register value at the end of the stack |
| pop reg                    | C      | 1           | 0-6        | 0           | It retrieves the value from the top of the stack and stores it in the register |
| scall                      | D      | 0           | 0          | 0           | System operations |
| ret			     | E      | 0           | 0          | 0           | Returns from the subroutine |
| call                       | F      | 0           | 0          | 0           | Calls the subroutine |

## System call
System call operations depend on register R6. The address offset relative to the data for the operation must be stored in register R5.
| Operation | Code | Description            |
| --------- | ---- | ---------------------- |
| putc      | 0    | Print value as char    |
| puti      | 1    | Print value as integer |
| puth      | 2    | Print value as hex     |
| puts      | 3    | Print string           |
