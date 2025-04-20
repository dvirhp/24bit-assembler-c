#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "symbol_list.h"
#include "machine_code.h"

/** 
 * Removes the opcode suffix from a string
 * @param str The string to remove the suffix from
 */
void remove_opcode_suffix(char* str);

/** 
 * Gets the index of a command by the opcode 
 * @param op The opcode to get the index of
 * @return The index of the command in the commands array
 */
int get_command_index(Opcode op);

/** 
 * Checks if a word is a register
 * @param str The word to check
 * @return 1 if the word is a register, 0 otherwise
 */
int is_register(char* str);

/** 
 * Checks if a word is a reserved word
 * @param str The word to check
 * @return The reserved word if the word is a reserved word, INVALID otherwise
 */
ReservedWord check_reserved_word(char* str);

/** 
 * Checks if a reserved word is a reserved word in the opcode
 * @param OpcodeOptions The opcode options
 * @param ReservedWord The reserved word to check
 * @return 1 if the word is a reserved word, 0 otherwise
 */
int is_reserved_word_in_opcode_option(OpcodeOptions* OpcodeOptions, ReservedWord word);

/** 
 * Checks if an opcode is a valid opcode
 * @param op The opcode to check
 * @param opcode_s The source reserved word
 * @param opcode_f The destination reserved word
 * @return 1 if the opcode is valid, 0 otherwise
 */
int is_valid_opcode_reserved_words(Opcode op, ReservedWord opcode_source, ReservedWord opcode_destination);

/** 
 * Processes the opcode struct bit wise
 * @param opStruct The opcode struct to process
 * @return 1 if the opcode was processed successfully, 0 otherwise
 */
int process_opcode_struct(OpcodeStruct opStruct);

/** 
 * Fills the bitfields struct with the given data
 * @param data The bitfields struct to fill
 * @param opcode The opcode number
 * @param src_method The source method
 * @param src_register The source register
 * @param dst_method The destination method
 * @param dst_register The destination register
 * @param funct The funct number
 * @param are The 'ARE' number
 */
void fill_bitfields(BitFields* data, int opcode, int src_method, int src_register, int dst_method, int dst_register, int funct, int are);

/** 
 * Finds the opcode number and funct number of an opcode
 * @param opcode The opcode to find
 * @param opcode_num The pointer to the opcode number
 * @param funct The pointer to the funct number
 * @return 1 if the opcode was found, 0 otherwise
 */
int find_opcode_num_and_funct(Opcode opcode, int* opcode_num, int* funct);

/** 
 * Appends a new bitfields node to the list
 * @param word The type that reqiured to append
 * @param str The contetnt of the word
 */
void append_additional_nodes(ReservedWord word, char* str);

/** 
 * Handles an opcode line to a level of identifing the opcode and src/dst methods
 * @param line The line to handle
 * @return 1 if the line was handled successfully, 0 otherwise
 */
int handle_opcode_line(char* line);