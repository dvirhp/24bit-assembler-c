#ifndef MACHINE_CODE_H
#define MACHINE_CODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "symbol_list.h"

/* Structure representing the bit fields of an instruction */
typedef struct {
    int opcode;
    int src_method;
    int src_register;
    int dst_method;
    int dst_register;
    int funct;
    int are;
} BitFields;

/* Structure representing a node in the bit fields list */
typedef struct BitFieldsNode {
    BitFields data; /* The data of the node */
    int num_of_line; /* The num of the line */
    struct BitFieldsNode* next; /* Pointer to the next node */  
} BitFieldsNode;

/**
 * Converts the bit fields to a single unsigned int
 * @param fields The bit fields to convert
 * @return The unsigned int representing the bit fields
 */
unsigned int convert_to_bits_fields(BitFields fields);

/**
 * Creates a new bit fields node
 * @param data The data of the node
 * @return The new bit fields node
 */
BitFieldsNode* create_bits_fields_node(BitFields data);

/**
 * Add a new bit fields node to the list
 * @param head The head of the list
 * @param data The new node to add
 */
void append_bits_fields_node(BitFieldsNode** bits_fields_head, BitFields data);

/**
 * Frees the bit fields list
 * @param head The head of the list
 */
void free_list_bits_fields(BitFieldsNode* bits_fields_head);

/**
 * Prints the bit fields to a file
 * @param filename The name of the file to print to
 * @return 0 if the file was printed successfully
 * , -1 otherwise (Check if IC_second is greater than 2^21 which is the limit for the assignment)
 */
 int print_bits_fields_to_file(char* filename) ;
 
/**
 * Replaces the nodes with the given data
 * @param line_number The line number to start replace from
 * @param data1 The first data to replace with the line info 
 * @param data2 The second data to replace with the line info 
 */
void replace_nodes_with_data(int line_number, BitFields data1, BitFields data2);

extern BitFieldsNode* bits_fields_head; /* The head of the bit fields list */
extern int IC_second;
extern int temp_DC;
#endif