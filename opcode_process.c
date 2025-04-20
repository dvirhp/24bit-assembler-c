#include "opcode_process.h"

void remove_opcode_suffix(char* str) {
    char* suffix = strstr(str, "_m"); /* Find the suffix */
    if (suffix != NULL) {
        *suffix = '\0'; /* Null terminate the string */
    }
}

void fill_bitfields(BitFields* data, int opcode, int src_method, int src_register, int dst_method, int dst_register, int funct, int are) {
    /* Fill the bitfields struct with the given data */
    data->opcode = opcode;
    data->src_method = src_method;
    data->src_register = src_register;
    data->dst_method = dst_method;
    data->dst_register = dst_register;
    data->funct = funct;
    data->are = are;
}

int get_command_index(Opcode op) {
    int i;
    char temp_opcode[MAX_COMMAND_NAME_LENGTH];
    /* Copy the opcode string to a temp*/
    strncpy(temp_opcode, opcodeStrings[op], sizeof(temp_opcode) - 1);
    temp_opcode[sizeof(temp_opcode) - 1] = '\0'; /* Null terminate the string */

    for (i = 0; i < NUM_COMMANDS; i++) {
        char command_name[10];
        /* Copy the command name to a temp */
        strncpy(command_name, commands[i].name, sizeof(command_name) - 1);
        command_name[sizeof(command_name) - 1] = '\0';
        
        remove_opcode_suffix(command_name);
        if (strcasecmp(temp_opcode, command_name) == 0) { /* Compare */
            return i; /* Return the index*/
        }
    }

    return -1; 
}

int is_register(char* str) {
    int i = 0;
    while (register_lookup_table[i].name != NULL)  {
        if (strcmp(str, register_lookup_table[i].name) == 0) { /* Compare */
            return 1;
        }
        i++;
    }
    return 0;
}

ReservedWord check_reserved_word(char* str) {
    /* Cataloging by addressing methods */
    if(str[0] == '\0') {
        return INVALID;
    }
    if (str[0] == '#' && is_valid_number(str + 1)) { /* Check the sign and the number */
        IC++;
        return ZERO;
    }
    if (is_valid_identifier(str)) { /* Check if the string is a valid */
        IC++;
        return ONE;
    }
    if (str[0] == '&' && is_valid_identifier(str + 1)) { 
        IC++;
        return TWO;
    }
    if (is_register(str)) {
        return THREE;
    }
    printf("Invalid operand: %s at line %d\n", str, line_IC -100);
    return INVALID;
}

int is_reserved_word_in_opcode_option(OpcodeOptions* OpcodeOptions, ReservedWord word) {
    int i;
    for (i = 0; i < OpcodeOptions->count; i++) { /* Iterate through the opcode options */
        if (OpcodeOptions->words[i] == word) { /* Check if the word is in the opcode options */
            return 1;
        }
    }
    return 0;
}

int is_valid_opcode_reserved_words(Opcode op, ReservedWord opcode_source, ReservedWord opcode_destination) {
    Command* cmd;
    /* Check if the opcode is within the valid range */
    if (op < 0 || op >= NUM_COMMANDS) {
        return 0;
    }
    cmd = &commands[op];
    /* Check if the source reserved word is valid for the given opcode */
    if (cmd->OpcodeOptionsSource.count > 0 && !is_reserved_word_in_opcode_option(&cmd->OpcodeOptionsSource, opcode_source)) {
        return 0;
    }
    /* Check if the destination reserved word is valid for the given opcode */
    if (cmd->OpcodeOptionsDestination.count > 0 && !is_reserved_word_in_opcode_option(&cmd->OpcodeOptionsDestination, opcode_destination)) {
        return 0;
    }
    return 1;
}

void append_additional_nodes(ReservedWord word, char* str) {
    BitFields data;
    unsigned int value;
    if (word == ZERO) {
        str++; /*Skip the '#' character */ 
        value = (unsigned int)atoi(str); /* Convert string to unsigned int */ 
        value = (value & 0x1FFFFF) << 3; /* Mask and shift the value */
        fill_bitfields(&data, (value & 0xFC0000) >> 18, (value & 0x030000) >> 16, (value & 0x00E000) >> 13, (value & 0x001800) >> 11, (value & 0x000700) >> 8, (value & 0x0000F8) >> 3, 4);
        append_bits_fields_node(&bits_fields_head, data); 
    } else if (word == ONE) {
        fill_bitfields(&data, 0, 0, 0, 0, 0, 0, 0); /* Fill bitfields with zeros (before second pass) */
        append_bits_fields_node(&bits_fields_head, data); 
    } 
}

int find_opcode_num_and_funct(Opcode opcode, int* opcode_num, int* funct) {
    int i;
    /* Try to find the matching opcode */
    for (i = 0; cmd_lookup_table[i].opcode_name != OP_NONE; i++) {
        if (cmd_lookup_table[i].opcode_name == opcode) {
            /* Set the opcode number */
            *opcode_num = cmd_lookup_table[i].opcode_num;
            *funct = (cmd_lookup_table[i].opcode_funct == -1) ? 0 : cmd_lookup_table[i].opcode_funct;
            return 1; /* Return 1 if found */
        }
    }
    return 0; 
}

int process_opcode_struct(OpcodeStruct opStruct) {
    Opcode opcode = opStruct.opcode;
    ReservedWord word1 = check_reserved_word(opStruct.source); 
    ReservedWord word2 = check_reserved_word(opStruct.destination); 
    int count_index = get_command_index(opcode);
    int count_op_makor = commands[count_index].OpcodeOptionsSource.count; 
    int count_op_dest = commands[count_index].OpcodeOptionsDestination.count;
    int opcode_num, funct;
    int i,j;
    
    /* Check if the opcode number and function can be found */
    if (!find_opcode_num_and_funct(opcode, &opcode_num, &funct)) {
        printf("Error: Opcode not found at line %d.\n", line_IC -100);
        return 0;
    }
    /* Iterate through the source options for the command */
    for (i = 0; i < count_op_makor; i++) {
        if (strcmp(reservedWordStrings[commands[count_index].OpcodeOptionsSource.words[i]], reservedWordStrings[word1]) == 0) {
            /* Iterate through the destination options for the command */
            for (j = 0; j < count_op_dest; j++) {
                if (strcmp(reservedWordStrings[commands[count_index].OpcodeOptionsDestination.words[j]], reservedWordStrings[word2]) == 0) {
                    BitFields new_data;
                    /* Fill the bitfields with the values */
                    fill_bitfields(&new_data, opcode_num, word1, (word1 == THREE) ? atoi(opStruct.source + 1) : 0, word2, (word2 == THREE) ? atoi(opStruct.destination + 1) : 0, funct, 4);

                    /* Handle the case where both words are invalid */
                    if (word1 == INVALID && word2 == INVALID) {
                        fill_bitfields(&new_data, opcode_num, 0, 0, 0, 0, funct, 4);
                    }

                    /* Append the new bitfields to the list */
                    append_bits_fields_node(&bits_fields_head, new_data);
                    /* Append additional nodes for the source and destination */
                    append_additional_nodes(word1, opStruct.source);
                    append_additional_nodes(word2, opStruct.destination);

                    /* Handle the case where the destination is TWO and the source is INVALID */
                    if (word2 == TWO && word1 == INVALID) {
                        BitFields two_data;
                        memset(&two_data, 0, sizeof(BitFields));

                        append_bits_fields_node(&bits_fields_head, two_data);
                    }
                    return 1;
                }
            }
        }
    }
    printf("Error: Invalid source or destination for opcode at line %d.\n", line_IC -100);
    return 0;
}

int handle_opcode_line(char* line) {
    char* token;
    Opcode opcode;
    OpcodeStruct opStruct;
    char* rest_of_line;
    char* source;
    char* destination;
    char temp_str[MAX_LINE_LENGTH + 1];
    strcpy(temp_str, line);
    token = strtok(temp_str, " \t\n"); /* Split the line by whitechars */

    if (token == NULL) {
        printf("Error: No opcode found in line at line %d.\n", line_IC -100);
        return 0;
    }

    opcode = handle_word_opcode(token);

    if (opcode == OP_NONE) {
        printf("Error: Invalid opcode found in line at line %d.\n", line_IC -100);
        return 0;
    }

    rest_of_line = strtok(NULL, "\n"); /* Get the rest of the line */

    if (rest_of_line != NULL) {
        rest_of_line = trim_whitespace(rest_of_line);
        source = strtok(rest_of_line, ","); /* Split the line by comma */
        destination = strtok(NULL, ",");

        if (source != NULL) { /* 1 operand at least */
            source = trim_whitespace(source);
            if (destination != NULL) {
                destination = trim_whitespace(destination);
            }

            opStruct.opcode = opcode;
            strncpy(opStruct.source, "", sizeof(opStruct.source) - 1);
            opStruct.source[sizeof(opStruct.source) - 1] = '\0';
            strncpy(opStruct.destination, "", sizeof(opStruct.destination) - 1);
            opStruct.destination[sizeof(opStruct.destination) - 1] = '\0';

            strncpy(opStruct.source, source, sizeof(opStruct.source) - 1);
            opStruct.source[sizeof(opStruct.source) - 1] = '\0'; /* Null terminate */

            if (destination != NULL) { /* 2 operands */
                strncpy(opStruct.destination, destination, sizeof(opStruct.destination) - 1); /* Copy the destination */
                opStruct.destination[sizeof(opStruct.destination) - 1] = '\0'; /* Null terminate */
            } else {
                strncpy(opStruct.destination, opStruct.source, sizeof(opStruct.destination) - 1); /* Copy the source to the destination */
                opStruct.destination[sizeof(opStruct.destination) - 1] = '\0'; /* Null terminate */
                strncpy(opStruct.source, "", sizeof(opStruct.source) - 1);
                opStruct.source[sizeof(opStruct.source) - 1] = '\0'; /* Null terminate */
            }

            if (process_opcode_struct(opStruct)) { 
                return 1;
            } else {
                return 0;
            }
        }
    }

    opStruct.opcode = opcode;
    strncpy(opStruct.source, "", sizeof(opStruct.source) - 1);
    opStruct.source[sizeof(opStruct.source) - 1] = '\0';
    strncpy(opStruct.destination, "", sizeof(opStruct.destination) - 1);
    opStruct.destination[sizeof(opStruct.destination) - 1] = '\0';

    if (process_opcode_struct(opStruct)) {
        return 1;
    } else {
        return 0;
    }
}