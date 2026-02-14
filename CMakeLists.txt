cmake_minimum_required (VERSION 3.8)
project("SimpleVirtualMachine" C)
set(CMAKE_C_STANDARD_REQUIRED ON)

set( CPU
	"${CMAKE_SOURCE_DIR}/src/cpu/cpu.c"
	"${CMAKE_SOURCE_DIR}/src/cpu/cpu.h"
)

set( MEMORY
	"${CMAKE_SOURCE_DIR}/src/memory/cpu_memory.c"
	"${CMAKE_SOURCE_DIR}/src/memory/cpu_memory.h"
)

set( STACK
	"${CMAKE_SOURCE_DIR}/src/memory/stack/stack.c"
	"${CMAKE_SOURCE_DIR}/src/memory/stack/stack.h"
)

add_executable(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/main.c" ${CPU} ${MEMORY} ${STACK} ${VECTOR})
target_compile_features(${PROJECT_NAME} PRIVATE c_std_99)