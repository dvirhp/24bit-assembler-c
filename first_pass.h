#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "opcode_process.h"
#include "machine_code.h"

/**
 * Processes a word that represents a point directive (".data", ".string" , ".entry", ".extern")
 * It determines the type of the directive and returns the corresponding TypeWord 
 * @param word The word to handle
 * @return TypeWord The type of the word
 */
TypeWord handle_word_point(char* word);

/**
 * Processes a word that represents a label ("X:")
 * It determines the type of the label and returns the corresponding TypeWord
 * @param word The word to handle.
 * @return TypeWord The type of the word
 */
TypeWord handle_word_label(char* word);

/**
 * Processes a word using the handle_word_point function
 * It determines the type of the directive and returns the corresponding TypeWord
 * Includes the opcode/label check
 * @param word The word to handle.
 * @return TypeWord The type of the word
 */
TypeWord handle_directive(char* word);

/**
 * Processes the next words in a data directive (".data X, Y")
 * @param str The string containing the data directive
 * @return int 1 if successful, 0 otherwise.
 */
int handle_next_words_data(char* str);

/**
 * Processes the next words in a string directive (".string X")
 * @param token Containing the string directive
 * @return int 1 if successful, 0 otherwise
 */
int handle_next_words_string(char* token);

/**
 * Processes the next words in an entry directive (".entry X")
 * @param token The token containing the entry directive
 * @return int 1 if successful, 0 otherwise
 */
int handle_next_words_entry(char* token);

/**
 * Processes the next words in an extern directive (".extern X")
 * @param token The token containing the extern directive
 * @return int 1 if successful, 0 otherwise
 */
int handle_next_words_extern(char* token);

/**
 * This function determines the type of the string (label, opcode ....)
 * and handles it accordingly
 * @param str The string to process.
 * @return int 1 if successful, 0 otherwise.
 */
int process_string(char* str);

#endif