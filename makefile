# Compiler and flags
CC = gcc
CFLAGS = -Wall -ansi -pedantic

# Executable target
assembler: assembler.o mcro_pass.o opcode_process.o symbol_list.o first_pass.o utils.o machine_code.o second_pass.o
	$(CC) $(CFLAGS) assembler.o mcro_pass.o opcode_process.o symbol_list.o first_pass.o utils.o machine_code.o second_pass.o -o assembler

# Object file rules
## Main file that mange all the process from reading the file to writing the output
assembler.o: assembler.c mcro_pass.h first_pass.h symbol_list.h machine_code.h
	$(CC) $(CFLAGS) -c assembler.c

## Take care of the macro stage , read the file and replace the macro with the content code
mcro_pass.o: mcro_pass.c mcro_pass.h utils.h
	$(CC) $(CFLAGS) -c mcro_pass.c

## Process the opcodes by checking the diffrent cases
opcode_process.o: opcode_process.c opcode_process.h symbol_list.h machine_code.h
	$(CC) $(CFLAGS) -c opcode_process.c

## Constract the symbol table and keep tracks of the values for each symbol
symbol_list.o: symbol_list.c symbol_list.h utils.h
	$(CC) $(CFLAGS) -c symbol_list.c

## Handle each line in the file and process it to get the machine code (in case there isnt need for a second pass)
first_pass.o: first_pass.c first_pass.h utils.h opcode_process.h machine_code.h
	$(CC) $(CFLAGS) -c first_pass.c

## Utility functions ,help minimize the code duplication
utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

## Making the actual transition from code to bits
machine_code.o: machine_code.c machine_code.h utils.h symbol_list.h
	$(CC) $(CFLAGS) -c machine_code.c

## Final pass throw the file to filling the blanks and write the output
second_pass.o: second_pass.c second_pass.h utils.h symbol_list.h first_pass.h
	$(CC) $(CFLAGS) -c second_pass.c

# Clean up object files and the executable
clean:
	rm -f *.o assembler

.PHONY: all clean