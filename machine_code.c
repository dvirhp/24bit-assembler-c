#include "machine_code.h"

BitFieldsNode* bits_fields_head = NULL;

unsigned int convert_to_bits_fields(BitFields fields) {
    unsigned int result = 0;
    result |= (fields.opcode & 0x3F) << 18;        /* 6 bits for opcode */
    result |= (fields.src_method & 0x3) << 16;     /* 2 bits for source addressing method */
    result |= (fields.src_register & 0x7) << 13;   /* 3 bits for source register */
    result |= (fields.dst_method & 0x3) << 11;     /* 2 bits for destination addressing method */
    result |= (fields.dst_register & 0x7) << 8;    /* 3 bits for destination register */
    result |= (fields.funct & 0x1F) << 3;          /* 5 bits for function */
    result |= (fields.are & 0x7);                  /* 3 bits for are */
    return result;
}

BitFieldsNode* create_bits_fields_node(BitFields data) {
    BitFieldsNode* new_node;
    new_node = (BitFieldsNode*)malloc(sizeof(BitFieldsNode));
    if (!new_node) { /* Allocation failed */
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    new_node->data = data;
    new_node->num_of_line = IC; /* Update the line number */
    new_node->next = NULL;

    return new_node;
}

void append_bits_fields_node(BitFieldsNode** bits_fields_head, BitFields data) {
    BitFieldsNode* new_node;
    BitFieldsNode* temp;
    new_node = create_bits_fields_node(data);
    if (!new_node) {
        return;
    }
    if (*bits_fields_head == NULL) { /* The list is empty */
        *bits_fields_head = new_node;
    } else {
        temp = *bits_fields_head;
        while (temp->next != NULL) { /* Move to the last node */
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

void free_list_bits_fields(BitFieldsNode* bits_fields_head) {
    BitFieldsNode* temp;
    while (bits_fields_head != NULL) { 
        temp = bits_fields_head;
        bits_fields_head = bits_fields_head->next;
        free(temp);
    }
}

int print_bits_fields_to_file(char* filename) {
    FILE* file;
    BitFieldsNode* temp;
    unsigned int address;
    unsigned int result;
    if (IC_second > (1 << 21)) { /* Check if IC_second is greater than 2^21*/
        fprintf(stderr, "Error: Number of words is too large.\n");
        return -1;
    }
    file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s' for writing\n", filename);
        return -1;
    }

    fprintf(file, "     %d %d\n", (IC_second-99)-temp_DC,temp_DC); /* Print the number of instructions and data */

    temp = bits_fields_head;
    address = 100; /* The address of the first line */
    while (temp != NULL) {
        result = convert_to_bits_fields(temp->data);
        fprintf(file, "%07u %06X\n", address, result); /* hexadecimal */
        address++; /* Next address */
        temp = temp->next;
    }

    fclose(file);
    return 0;
}

void replace_nodes_with_data(int line_number, BitFields data1, BitFields data2) {
    BitFieldsNode* temp;
    int found = 0; /* Matching found */
    temp = bits_fields_head;
    while (temp != NULL) {
        /* Check if the current node matches the line number and has no data */
        if (temp->num_of_line == line_number && convert_to_bits_fields(temp->data) == 0) {
            if (found == 0) {
                /* Replace the first matching node with data1 */
                temp->data = data1;
                found++;
            } else if (found == 1) {
                /* Replace the second matching node with data2 */
                temp->data = data2;
                found++;
                break; /* Exit after replacing */
            }
        }
        temp = temp->next; /* Move to the next node */
    }
}