#include "second_pass.h"

int handle_next_words_data_second_pass(char* str) {
    char* token;
    char temp_str[MAX_LINE_LENGTH + 1];
    strcpy(temp_str, str);
    token = strtok(temp_str, ","); /* Split by comma*/
    while (token != NULL) {
        token = trim_whitespace(token); /* Trim*/
        DC_second++; /* Increment data counter */
        temp_DC++; 
        token = strtok(NULL, ","); /* Next token*/
    }
    IC_second += DC_second; /* Update instruction counter with data counter*/
    IC_second--; 
    DC_second = 0; /* Reset*/
    return 1; 
}

int handle_next_words_string_second_pass(char* token) {
    token = trim_whitespace(token);
    if (token[0] == '"' && token[strlen(token) - 1] == '"') { /* Check if the string is valid*/
        DC_second += strlen(token) - 2 + 1;
        temp_DC = DC_second;
        IC_second += DC_second; /* Update instruction counter with data counter*/
        IC_second--;
        DC_second = 0;  /* Reset*/
        return 1;
    } 
    return 0;
}

int process_line_second_pass(char* str) {
    char* token;
    char temp_str[MAX_LINE_LENGTH + 1];
    strcpy(temp_str, str);
    token = strtok(temp_str, " \t\n"); /* Split by space, tab or newline*/

    if (token != NULL) { 
        TypeWord type = handle_directive(token);
        switch (type) {
            case ENTRY: /* Dot operator handeled before*/
                break;
            case EXTERN:
                break;
            case STRING:
                /* Handle STRING*/
                IC_second++; /* Increment IC*/
                token = strtok(NULL, "\n");
                if (token != NULL) {
                    return handle_next_words_string_second_pass(token); 
                } else {
                    return 0;
                }
            case OPCODE:
                /* Handle OPCODE*/
                IC_second++; /* Increment IC*/
                if (!handle_opcode_line_second_pass(str)) 
                return 0;
                else return 1;
                break;
            case DATA:
                /* Handle DATA*/
                IC_second++; /* Increment IC*/
                token = strtok(NULL, "\n");
                if (token != NULL) {
                    return handle_next_words_data_second_pass(token);
                } else {
                    return 0;
                }
            case LABEL:
                /* Handle LABEL*/
                IC_second++; /* Increment IC*/
                token = strtok(NULL, " \t\n");
                if (token != NULL && strlen(trim_whitespace(token)) > 0) {
                    TypeWord next_type = handle_directive(token);
                    switch (next_type) {
                        case ENTRY:
                        break;
                        case EXTERN:
                        break;
                        case STRING:
                        /* Handle STRING option*/
                            token = strtok(NULL, "\n");
                            if (token != NULL) {
                                return handle_next_words_string_second_pass(token);
                            } else {
                                return 0;
                            }
                        case DATA:
                        /* Handle DATA option*/
                            token = strtok(NULL, "\n");
                            if (token != NULL) {
                                return handle_next_words_data_second_pass(token);
                            } else {
                                return 0;
                            }
                        case OPCODE:
                        /* Handle OPCODE option*/
                            if (!handle_opcode_line_second_pass((str + (token - temp_str)))) 
                            return 0;
                            break;
                        default:
                            return 0;
                    }
                } else {
                    return 0;
                }
                break;
            default:
                return 0;
        }
    } else {
        return 0;
    }
    return 1;
}

int handle_opcode_line_second_pass(char* line) {
    char* token; /* Token to hold parts of the line */
    Opcode opcode; /* Opcode of the instruction */
    char* rest_of_line; /* The rest of the line after the opcode */
    char* source; /* Source operand */
    char* destination; /* Destination operand */
    OpcodeStruct opStruct, opStruct_no_operands; /* Structures to hold opcode and operands */
    char temp_str[MAX_LINE_LENGTH + 1]; /* Temporary string to hold the line */

    /* Copy the line to a temporary string */
    strcpy(temp_str, line);

    /* Get the first token (the opcode) */
    token = strtok(temp_str, " \t\n");
    opcode = handle_word_opcode(token);

    /* Get the rest of the line */
    rest_of_line = strtok(NULL, "\n");

    if (rest_of_line != NULL) {
        rest_of_line = trim_whitespace(rest_of_line);
        source = strtok(rest_of_line, ",");
        destination = strtok(NULL, ",");

        if (source != NULL) { /* At least 1 operand*/
            source = trim_whitespace(source);
            if (destination != NULL) {
                destination = trim_whitespace(destination);
            }
            opStruct.opcode = opcode;
            strncpy(opStruct.source, source, sizeof(opStruct.source) - 1);
            opStruct.source[sizeof(opStruct.source) - 1] = '\0';

            if (destination != NULL) { /* 2 operands*/
                strncpy(opStruct.destination, destination, sizeof(opStruct.destination) - 1);
                opStruct.destination[sizeof(opStruct.destination) - 1] = '\0';
            } else {
                strncpy(opStruct.destination, opStruct.source, sizeof(opStruct.destination) - 1);
                opStruct.destination[sizeof(opStruct.destination) - 1] = '\0';
                strncpy(opStruct.source, "", sizeof(opStruct.source) - 1);
                opStruct.source[sizeof(opStruct.source) - 1] = '\0';
            }

            if (process_opcode_struct_second_pass(opStruct)) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    
    opStruct_no_operands.opcode = opcode;
    strncpy(opStruct_no_operands.source, "", sizeof(opStruct_no_operands.source) - 1);
    opStruct_no_operands.source[sizeof(opStruct_no_operands.source) - 1] = '\0';
    strncpy(opStruct_no_operands.destination, "", sizeof(opStruct_no_operands.destination) - 1);
    opStruct_no_operands.destination[sizeof(opStruct_no_operands.destination) - 1] = '\0';

    if (process_opcode_struct_second_pass(opStruct_no_operands)) {
        return 1;
    } else {
        return 0;
    }
}

void process_symbol_bitfields(BitFields* data, SymbolNode* symbol, int are, int ic_offset) {
    unsigned int value;
    /* If the symbol conatins an extern type*/
    if (symbol->types.type1 == EXTERN || symbol->types.type2 == EXTERN) {
        fill_bitfields(data, 0, 0, 0, 0, 0, 0, 1);
        value = (unsigned int)symbol->value;
        value = (value & 0x1FFFFF) << 3; /* Shift bits*/
        fill_bitfields(data, (value & 0xFC0000) >> 18, (value & 0x030000) >> 16, (value & 0x00E000) >> 13, (value & 0x001800) >> 11, (value & 0x000700) >> 8, (value & 0x0000F8) >> 3, 1);
        append_extern_node(&symbol_extern_list_head, symbol->name, IC_second + ic_offset);
    } else {
        /* If the symbol contains an entry type*/
        fill_bitfields(data, 0, 0, 0, 0, 0, 0, are); 
        value = (unsigned int)symbol->value;
        value = (value & 0x1FFFFF) << 3; /* Shift bits*/
        fill_bitfields(data, (value & 0xFC0000) >> 18, (value & 0x030000) >> 16, (value & 0x00E000) >> 13, (value & 0x001800) >> 11, (value & 0x000700) >> 8, (value & 0x0000F8) >> 3, are);
    }
}

ReservedWord check_reserved_word_second(char* str) {
    /* Cataloging by addressing methods*/
    if(str[0] == '\0') {
        return INVALID;
    }
    if (str[0] == '#' && is_valid_number(str + 1)) { /* Check the sign and the number*/
        IC++;
        return ZERO;
    }
    if (is_valid_identifier(str)) { /* Check if the string is a valid*/
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
    return INVALID;
}

int process_opcode_struct_second_pass(OpcodeStruct opStruct) {
    Opcode opcode; /* Opcode of the instruction */
    ReservedWord word1; /* Reserved word type of the source operand */
    ReservedWord word2; /* Reserved word type of the destination operand */
    char* temp_str2; /* Temporary string pointer */
    int count_index; /* Index of the command in the commands array */
    int count_op_makor; /* Number of valid source operand types for the opcode */
    int count_op_dest; /* Number of valid destination operand types for the opcode */
    BitFields data1, data2, null_data; /* BitFields structures to hold data */
    SymbolNode* symbol1; /* Symbol node for the source operand */
    SymbolNode* symbol2; /* Symbol node for the destination operand */
    int i, j; /* Loop counters */

    /* Get the opcode from the OpcodeStruct */
    opcode = opStruct.opcode;

    /* Check the reserved word type of the source and destination operands */
    word1 = check_reserved_word_second(opStruct.source); 
    word2 = check_reserved_word_second(opStruct.destination);

    /* Get the index of the command in the commands array */
    count_index = get_command_index(opcode);

    /* Get the number of valid source and destination operand types for the opcode */
    count_op_makor = commands[count_index].OpcodeOptionsSource.count;
    count_op_dest = commands[count_index].OpcodeOptionsDestination.count;

    /* Loop through the valid source operand types */
    for (i = 0; i < count_op_makor; i++) {
        if (strcmp(reservedWordStrings[commands[count_index].OpcodeOptionsSource.words[i]], reservedWordStrings[word1]) == 0) {
            /* Loop through the valid destination operand types */
            for (j = 0; j < count_op_dest; j++) {
                if (strcmp(reservedWordStrings[commands[count_index].OpcodeOptionsDestination.words[j]], reservedWordStrings[word2]) == 0) {
                    if (word1 == ZERO) IC_second++;  /* Increment IC for source operand */
                    if (word2 == ZERO) IC_second++; /* Increment IC for destination operand */

                    if (word1 == ONE && word2 == ONE) { 
                        IC_second += 2; /* Increment IC for both operands */
                        symbol1 = find_symbol(symbol_list_head, opStruct.source);
                        symbol2 = find_symbol(symbol_list_head, opStruct.destination);
                        if (symbol1 == NULL || symbol2 == NULL) {
                            printf("Error: Symbol '%s' not found in symbol list at line %d.\n", (symbol1 == NULL) ? opStruct.source : opStruct.destination, IC_second - 100);                            
                            return 0;
                        }

                        /* Process symbol bitfields for both operands */
                        process_symbol_bitfields(&data1, symbol1, 2, -1);
                        process_symbol_bitfields(&data2, symbol2, 2, 0);

                        replace_nodes_with_data(IC_second, data1, data2);
                        return 1;
                    }

                    /* Handle case where only the source operand is a symbol */
                    if (word1 == ONE && word2 != ONE) {
                        IC_second++;

                        symbol1 = find_symbol(symbol_list_head, opStruct.source);

                        if (symbol1 == NULL) {
                            printf("Error: Symbol '%s' not found in symbol list at line %d.\n", opStruct.source, IC_second - 100);                            
                            return 0;
                        }

                        /* Process symbol bitfields for source operand */
                        process_symbol_bitfields(&data1, symbol1, 2, 0);

                        memset(&null_data, 0, sizeof(BitFields));

                        replace_nodes_with_data(IC_second, data1, null_data);
                        return 1;
                    }

                    /* Handle case where only the destination operand is a symbol */
                    if (word2 == ONE && word1 != ONE) {
                        IC_second++;

                        symbol2 = find_symbol(symbol_list_head, opStruct.destination);

                        if (symbol2 == NULL) {
                            printf("Error: Symbol '%s' not found in symbol list at line %d.\n", opStruct.destination, IC_second - 100);                            
                            return 0;
                        }

                        /* Process symbol bitfields for destination operand */
                        process_symbol_bitfields(&data2, symbol2, 2, 0);

                        memset(&null_data, 0, sizeof(BitFields));

                        replace_nodes_with_data(IC_second, data2, null_data);
                        return 1;
                    }
                    
                    /* Handle case where the destination operand is an indirect symbol */
                    if (word2 == TWO && word1 == INVALID) {
                        IC_second++;

                        temp_str2 = opStruct.destination + 1;
                        symbol2 = find_symbol(symbol_list_head, temp_str2);

                        if (symbol2 == NULL) {
                            printf("Error: Symbol '%s' not found in symbol list at line %d.\n", temp_str2, IC_second - 100);
                            return 0;
                        }

                        /* Process symbol bitfields for destination operand with offset */
                        process_symbol_bitfields(&data2, symbol2, 4, -1);

                        memset(&null_data, 0, sizeof(BitFields));
                        replace_nodes_with_data(IC_second, data2, null_data);
                        return 1;
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}