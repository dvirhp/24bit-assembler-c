#include "first_pass.h"

TypeWord handle_directive(char* word) {
    int len;
    char label[MAX_SYMBOL_NAME_LENGTH];
    len = strlen(word);

    if (word[0] == '.') { /* Dot case */
        return handle_word_point(word + 1);
    } else {
        if (word[len - 1] != ':') { /* Last char ":" */
            Opcode op = handle_word_opcode(word); /* From utils page */
            if (op != OP_NONE) 
                return OPCODE;
        } else {
            strncpy(label, word, len - 1);
            label[len - 1] = '\0'; /* Null terminate */
            return handle_word_label(label);
        }
    }
    return UNKNOWN;
}

TypeWord handle_word_point(char* word) {
    /* compare each word to the reserved words */
    if (strcmp(word, "data") == 0) { 
        return DATA;
    } else if (strcmp(word, "entry") == 0) {
        return ENTRY;
    } else if (strcmp(word, "extern") == 0) {
        return EXTERN;
    } else if (strcmp(word, "string") == 0) {
        return STRING;
    } else {
        return UNKNOWN;
    }
}

TypeWord handle_word_label(char* label) {
    if (is_valid_identifier(label)) { 
        return LABEL;
    } else {
        printf("Invalid label: %s at line %d\n", label, line_IC -100);
        return UNKNOWN;
    }
}

int handle_next_words_data(char* str) {
    char* trimmed_str;
    char temp_str[MAX_LINE_LENGTH + 1];
    char* token;
    int number;
    BitFields new_data;

    trimmed_str = trim_whitespace(str);  /* Trim the string */
    if (trimmed_str[0] == '\0') {  /* Check if the trimmed string is empty */
        printf("Error: Missing data after 'data' directive at line %d\n", line_IC -100);
        return -1;
    }
    strcpy(temp_str, trimmed_str);
    token = strtok(temp_str, ","); /* Split by comma */

    while (token != NULL) { 
        token = trim_whitespace(token);
        if (!is_valid_number(token)) {
            printf("Invalid data number: %s at line %d\n", token, line_IC -100);
            return -1;
        }
        number = atoi(token); /* Convert to int */

        fill_bitfields(&new_data, 
            (number & 0xFC0000) >> 18,  /* 6 bits for opcode */
            (number & 0x030000) >> 16,  /* 2 bits for source addressing method */
            (number & 0x00E000) >> 13,  /* 3 bits for source register */
            (number & 0x001800) >> 11,  /* 2 bits for destination addressing method */
            (number & 0x000700) >> 8,   /* 3 bits for destination register */
            (number & 0x0000F8) >> 3,   /* 5 bits for function */
            (number & 0x000007));       /* 3 bits for ARE */

        append_bits_fields_node(&bits_fields_head, new_data);
        DC++;

        token = strtok(NULL, ","); /* Get the next token */
        if (token != NULL && *token == '\0') {
            printf("Error: Missing number after comma at line %d\n", line_IC -100);
            return -1;
        }
    }
    IC += DC;
    IC--;
    DC = 0; /* Reset DC */
    return 1;
}

int handle_next_words_string(char* token) {
    int i;
    int token_length;
    BitFields new_data;
    BitFields null_data;
    unsigned int ascii_value;
    token = trim_whitespace(token);


    if (token[0] == '\0') { /* If empty */
        printf("Error: Missing data after 'data' directive at line %d\n", line_IC -100);
        return -1;
    }
    if (token[0] == '"' && token[strlen(token) - 1] == '"') { /* Check if string */
        DC += strlen(token) - 2 + 1;
        IC += DC;
        IC--;
        DC = 0;
        token_length = (int)strlen(token); 

        for (i = 1; i < token_length - 1; i++) {
            ascii_value = (unsigned int)token[i]; /* Get ASCII value for each char */

            /* Using opcode_process page functions */
            fill_bitfields(&new_data, 
                (ascii_value & 0xFC0000) >> 18,  /* 6 bits for opcode */
                (ascii_value & 0x030000) >> 16,  /* 2 bits for source addressing method */
                (ascii_value & 0x00E000) >> 13,  /* 3 bits for source register */
                (ascii_value & 0x001800) >> 11,  /* 2 bits for destination addressing method */
                (ascii_value & 0x000700) >> 8,   /* 3 bits for destination register */
                (ascii_value & 0x0000F8) >> 3,   /* 5 bits for function */
                (ascii_value & 0x000007));       /* 3 bits for ARE */

            append_bits_fields_node(&bits_fields_head, new_data);
        }

        fill_bitfields(&null_data, 0, 0, 0, 0, 0, 0, 0); /* Fill with 0 for second pass */

        append_bits_fields_node(&bits_fields_head, null_data);

        return 1;
    } else {
        printf("Invalid string: %s at line %d\n", token, line_IC -100);
        return 0;
    }
}

int handle_next_words_entry(char* token) {
    if (is_valid_identifier(token)) { /* Check if valid */
        if (add_or_update_symbol(&symbol_list_head, token, 0, ENTRY) != 0) {
            fprintf(stderr, "Error adding or updating symbol '%s' with type ENTRY at line %d\n", token, line_IC -100);
            return 0;
        }
        return 1;
    } else {
        printf("Invalid entry word: %s at line %d\n", token, line_IC -100);
        return 0;
    }
}

int handle_next_words_extern(char* token) {
    if (is_valid_identifier(token)) { /* Check if valid */
        if (add_or_update_symbol(&symbol_list_head, token, 0, EXTERN) != 0) {
            fprintf(stderr, "Error adding or updating symbol '%s' with type EXTERN at line %d\n", token, line_IC -100);
            return 0;
        }
        return 1;
    } else {
        printf("Invalid extern word: %s at line %d\n", token, line_IC -100);
        return 0;
    }
}

int process_string(char* str) {
    int len;
    char* token;
    char temp_str[MAX_LINE_LENGTH + 1];
    strcpy(temp_str, str);
    token = strtok(temp_str, " \t\n"); /* Split by space, tab or newline */

    if (token != NULL) {
        TypeWord type = handle_directive(token); 

        if (type == ENTRY) { /* Handle ENTRY */
            token = strtok(NULL, " \t\n"); 
            if (token != NULL) {
                if (!handle_next_words_entry(token)) return 0;
                token = strtok(NULL, " \t\n");
                if (token != NULL) {
                    printf("Extra word after entry: %s at line %d\n", token, line_IC -100);
                    return 0;
                }
            } else {
                return 0;
            }

        } else if (type == EXTERN) { /* Handle EXTERN */
            token = strtok(NULL, " \t\n");
            if (token != NULL) {
                if (!handle_next_words_extern(token)) return 0;
                token = strtok(NULL, " \t\n");
                if (token != NULL) {
                    printf("Extra word after extern: %s at line %d\n", token, line_IC -100);
                    return 0;
                }
            } else {
                return 0;
            }

        } else if (type == STRING) { /* Handle STRING */
            IC++;
            token = strtok(NULL, "\n");
            if (token != NULL) {
                return handle_next_words_string(token);
            } else {
                return 0;
            }

        } else if (type == OPCODE) { /* Handle OPCODE */
            IC++;
            if (!handle_opcode_line(str)) return 0;
            else return 1;

        } else if (type == DATA) { /* Handle DATA */
            IC++;
            token = strtok(NULL, "\n");
            if (token != NULL) {
                return handle_next_words_data(token);
            } else {
                return 0;
            }

        } else if (type == LABEL) { /* Handle LABEL */
            IC++;
            len = strlen(token);
            if (token[len - 1] == ':') { /* Swap ":" with null */
                token[len - 1] = '\0'; 
            }
            strcpy(temp_str, token);

            /* Check if the next word is ENTRY or EXTERN */
            token = strtok(NULL, " \t\n"); /* Split by space, tab or newline */
            if (token != NULL && strlen(trim_whitespace(token)) > 0) {

                TypeWord next_type = handle_directive(token);

                if (next_type == ENTRY) { /* Handle ENTRY */
                    token = strtok(NULL, " \t\n");
                    if (token != NULL) {
                        if (!handle_next_words_entry(token)) return 0;
                        token = strtok(NULL, " \t\n");
                        if (token != NULL) {
                            printf("Extra word after entry: %s at line %d\n", token, line_IC -100);
                            return 0;
                        }
                    } else {
                        return 0;
                    }

                } else if (next_type == EXTERN) { /* Handle EXTERN */
                    token = strtok(NULL, " \t\n");
                    if (token != NULL) {
                        if (!handle_next_words_extern(token)) return 0;
                        token = strtok(NULL, " \t\n");
                        if (token != NULL) {
                            printf("Extra word after extern: %s at line %d\n", token, line_IC -100);
                            return 0;
                        }
                    } else {
                        return 0;
                    }
                } else {
                    /* If not ENTRY or EXTERN, continue with the original operations */
                    /* Adding or updating symbol of type CODE */
                    if (add_or_update_symbol(&symbol_list_head, temp_str, 0, CODE) != 0) {
                        fprintf(stderr, "Error adding or updating symbol '%s' with type CODE at line %d\n", temp_str, line_IC -100);
                        return 0;
                    }
                    /* Updating symbol value */
                    if (update_symbol_value(symbol_list_head, temp_str, IC) != 0) {
                        fprintf(stderr, "Error updating symbol value for '%s' at line %d\n", temp_str, line_IC -100);
                        return 0;
                    }
                    if (next_type == STRING) { /* Handle STRING */
                        /* Adding or updating symbol of type DATA */
                        if (add_or_update_symbol(&symbol_list_head, temp_str, 0, DATA) != 0) {
                            fprintf(stderr, "Error adding or updating symbol '%s' with type DATA at line %d\n", temp_str, line_IC -100);
                            return 0;
                        }
                        token = strtok(NULL, "\n");
                        if (token != NULL) {
                            return handle_next_words_string(token);
                        } else {
                            return 0;
                        }

                    } else if (next_type == DATA) { /* Handle DATA */
                        /* Adding or updating symbol of type DATA */
                        if (add_or_update_symbol(&symbol_list_head, temp_str, 0, DATA) != 0) {
                            fprintf(stderr, "Error adding or updating symbol '%s' with type DATA at line %d\n", temp_str, line_IC -100);
                            return 0;
                        }
                        token = strtok(NULL, "\n");
                        if (token != NULL) {
                            return handle_next_words_data(token);
                        } else {
                            return 0;
                        }

                    } else if (next_type == OPCODE) { /* Handle OPCODE */
                        if (!handle_opcode_line(str + (token - temp_str))) return 0;
                    } else { 
                        printf("Invalid word after label: %s at line %d\n", token, line_IC -100);
                        return 0;
                    }
                }
            } else {
                printf("No word after label at line %d\n", line_IC -100);
                return 0;
            }
        } else {
            printf("Error: Unknown word: %s at line %d\n", token, line_IC -100);            
            return 0;
        }
    } else {
        printf("No words found in the string at line %d\n", line_IC -100);
        return 0;
    }
    return 1;
}
