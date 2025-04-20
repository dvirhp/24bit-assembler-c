#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "symbol_list.h"
#include "first_pass.h"
#include "utils.h"

extern int IC_second;
extern int DC_second;
extern int temp_DC;

/** 
 * Handles the data directive including additional rows (if needed) in the second pass
 * @param str The string to process
 * @return 1 if the data directive was processed successfully, 0 otherwise
 */
int handle_next_words_data_second_pass(char* str);

/** 
 * Handles the string directive including additional rows (if needed) in the second pass
 * @param token The string to process
 * @return 1 if the string directive was processed successfully, 0 otherwise
 */
int handle_next_words_string_second_pass(char* token);

/** 
 * Identifies the directive type of a given string and processes it accordingly
 * string , data, opcode, label reqired to be handled separately
 * @param str The string to process
 * @return 1 if the line was processed successfully, 0 otherwise
 */
int process_line_second_pass(char* str);

/** 
 * Handles the opcode line in the second pass
 * processes a line containing an opcode and its operands during the second pass
 * it extracts the opcode and operands,and constructs an OpcodeStruct
 * @param line The line to process, which contains an opcode and its operands
 * @return 1 if the opcode line was processed successfully, 0 otherwise
 */
int handle_opcode_line_second_pass(char* line);

/**
 * Processes the bitfields for a given symbol during the second pass
 * This function process the bitfields for a given instruction counter offset
 * @param data Pointer to the BitFields structure to be updated
 * @param symbol Pointer to the SymbolNode
 * @param are The ARE value to be set in the bitfields
 * @param ic_offset The instruction counter offset to be added to the symbols address
 */
void process_symbol_bitfields(BitFields* data, SymbolNode* symbol, int are, int ic_offset);

/**
 * Processes the word operands, checks for match , and updates the IC in each case
 * @param opStruct The OpcodeStruct to process
 * @return 1 if the opcode was processed successfully, 0 otherwise
 */ 
int process_opcode_struct_second_pass(OpcodeStruct opStruct);

/** 
 * Checks if a word is a reserved word
 * @param str The word to check
 * @return The reserved word if the word is a reserved word, INVALID otherwise
 */
ReservedWord check_reserved_word_second(char* str);
#endif