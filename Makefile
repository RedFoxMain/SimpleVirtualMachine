CC := clang
CCFLAGS := -std=c99 -Iinclude -Wall -Wextra
BUILD := build
TARGET := SimpleVirtualMachine.exe

SRC := $(wildcard src/*.c)

all: $(SRC)
	@if not exist $(BUILD) mkdir $(BUILD)
	@$(CC) $(CCFLAGS) $^ -o $(BUILD)/$(TARGET)