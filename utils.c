#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* opcodeStrings[] = {
    "MOV",
    "CMP",
    "ADD",
    "SUB",
    "LEA",
    "CLR",
    "NOT",
    "INC",
    "DEC",
    "JMP",
    "BNE",
    "JSR",
    "RED",
    "PRN",
    "RTS",
    "STOP",
    "OP_NONE"
};

char* reservedWordStrings[] = {
    "ZERO",
    "ONE",
    "TWO",
    "THREE",
    "INVALID"
};

Command commands[] = { 
    {"MOV_m", {{ZERO, ONE, THREE}, 3}, {{ONE, THREE}, 2}},
    {"CMP_m", {{ZERO, ONE, THREE}, 3}, {{ZERO, ONE, THREE}, 3}},
    {"ADD_m", {{ZERO, ONE, THREE}, 3}, {{ONE, THREE}, 2}},
    {"SUB_m", {{ZERO, ONE, THREE}, 3}, {{ONE, THREE}, 2}},
    {"LEA_m", {{ONE}, 1}, {{ONE, THREE}, 2}},
    {"CLR_m", {{INVALID}, 1}, {{ONE, THREE}, 2}},   
    {"NOT_m", {{INVALID}, 1}, {{ONE, THREE}, 2}},   
    {"INC_m", {{INVALID}, 1}, {{ONE, THREE}, 2}},   
    {"DEC_m", {{INVALID}, 1}, {{ONE, THREE}, 2}},   
    {"JMP_m", {{INVALID}, 1}, {{ONE, TWO}, 2}},     
    {"BNE_m", {{INVALID}, 1}, {{ONE, TWO}, 2}},     
    {"JSR_m", {{INVALID}, 1}, {{ONE, TWO}, 2}},     
    {"RED_m", {{INVALID}, 1}, {{ONE, THREE}, 2}}, 
    {"PRN_m", {{INVALID}, 1}, {{ZERO, ONE, THREE}, 3}}, 
    {"RTS_m", {{INVALID}, 1}, {{INVALID}, 1}},
    {"STOP_m", {{INVALID}, 1}, {{INVALID}, 1}},
    {"NONE_m", {{INVALID}, 1}, {{INVALID}, 1}}
};

Register register_lookup_table[] = {
    {"r0"},
    {"r1"},
    {"r2"},
    {"r3"},
    {"r4"},
    {"r5"},
    {"r6"},
    {"r7"},
    {0}
};

InstructionLookupItem instructions_lookup_table[] = {
    {"string", 1},
    {"data", 2},
    {"entry", 3},
    {"extern", 4},
    {0, 0}
};

CommandLookupElement cmd_lookup_table[] = {
    {MOV, 0, -1},
    {CMP, 1, -1},
    {ADD, 2, 1},
    {SUB, 2, 2},
    {LEA, 4, -1},
    {CLR, 5, 1},
    {NOT, 5, 2},
    {INC, 5, 3},
    {DEC, 5, 4},
    {JMP, 9, 1},
    {BNE, 9, 2},
    {JSR, 9, 3},
    {RED, 12, -1},
    {PRN, 13, -1},
    {RTS, 14, -1},
    {STOP, 15, -1},
    {OP_NONE, -1, -1}
};

void to_upper(char* str) {
    int i;
    for (i = 0; str[i] != '\0'; i++) { 
        str[i] = toupper(str[i]); /* Convert to uppercase */
    }
}

void to_lower(char* str) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]); /* Convert to lowercase */
    }
}

char* trim_whitespace(char* str) {
    char *end;

    while (isspace((unsigned char)*str)) str++; /* Trim leading space */

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--; /* Trim trailing space */

    end[1] = '\0'; /* Null terminate */

    return str; /* Trimmed */
}

Opcode handle_word_opcode(char* word) {
    if (strcmp(word, "mov") == 0) return MOV;
    if (strcmp(word, "cmp") == 0) return CMP;
    if (strcmp(word, "add") == 0) return ADD;
    if (strcmp(word, "sub") == 0) return SUB;
    if (strcmp(word, "lea") == 0) return LEA;
    if (strcmp(word, "clr") == 0) return CLR;
    if (strcmp(word, "not") == 0) return NOT;
    if (strcmp(word, "inc") == 0) return INC;
    if (strcmp(word, "dec") == 0) return DEC;
    if (strcmp(word, "jmp") == 0) return JMP;
    if (strcmp(word, "bne") == 0) return BNE;
    if (strcmp(word, "jsr") == 0) return JSR;
    if (strcmp(word, "red") == 0) return RED;
    if (strcmp(word, "prn") == 0) return PRN;
    if (strcmp(word, "rts") == 0) return RTS;
    if (strcmp(word, "stop") == 0) return STOP;
    return OP_NONE;
}

int is_valid_number(char* str) {
    long number;
    if (*str == '\0') { /* Empty */
        return 0;
    }
    if (*str == '+' || *str == '-') { /* Check for sign */
        str++;
    }
    while (*str != '\0') { /* Check for digits */
        if (!isdigit(*str)) {
            return 0; 
        }
        str++;
    }

    number = atol(str);  /* Convert the string to a number */

    /* Convert the string to a number and check if it fits in 24 bits */
    if (number < -(1 << 23) || number >= (1 << 23)) {
        printf("Error: Number %ld is too large to fit in 24 bits.\n", number);
        return 0;
    }

    return 1; 
}

int is_valid_identifier(char* word) {
    int length = strlen(word);
    int i;
    char upper_word[MAX_SYMBOL_NAME_LENGTH + 1];
    char lower_word[MAX_SYMBOL_NAME_LENGTH + 1];

    /* Check if the length is within the allowed limit */
    if (length == 0 || length > MAX_SYMBOL_NAME_LENGTH) {
        return 0;
    }

    /* Check if the first character is an alphabetic letter (uppercase or lowercase) */
    if (!isalpha(word[0])) {
        return 0;
    }

    /* Check the rest of the characters */
    for (i = 1; word[i] != '\0'; i++) {
        if (!isalnum(word[i])) {
            return 0;
        }
    }

    /* Check if the word is not one of the opcode */
    for (i = 0; i < NUM_COMMANDS ; i++) {
        strcpy(upper_word, word);
        strcpy(lower_word, word);
        to_upper(upper_word);
        to_lower(lower_word);

        /* Compare to the opcode strings */
        if (strcmp(upper_word, opcodeStrings[i]) == 0 || strcmp(lower_word, opcodeStrings[i]) == 0) { 
            return 0;
        }
    }

    /* Check if the word is not one of the register names */
    for (i = 0; register_lookup_table[i].name != NULL; i++) {
        strcpy(upper_word, word);
        strcpy(lower_word, word);
        to_upper(upper_word);
        to_lower(lower_word);

        /* Compare to the register strings */
        if (strcmp(upper_word, register_lookup_table[i].name) == 0 || strcmp(lower_word, register_lookup_table[i].name) == 0) {
            return 0;
        }
    }

    /* Check if the word is not one of the instruction names */
    for (i = 0; instructions_lookup_table[i].name != NULL; i++) { 
        strcpy(upper_word, word);
        strcpy(lower_word, word);
        to_upper(upper_word);
        to_lower(lower_word);

        /* Compare to the instruction strings */
        if (strcmp(upper_word, instructions_lookup_table[i].name) == 0 || strcmp(lower_word, instructions_lookup_table[i].name) == 0) {
            return 0;
        }
    }

    return 1;
}

char* strallocat(char* str, char* suffix) {
    int len1 = strlen(str);
    int len2 = strlen(suffix);
    char* result = (char*)malloc(len1 + len2 + 1); /* Allocate */ 

    if (result == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for string concatenation\n");
        return NULL;
    }
    
    strcpy(result, str);
    strcat(result, suffix);
    return result;
}

char *allocate_and_copy_string(char *str) {
    int len;
    char *new_str;
    if (str == NULL) {
        return NULL; 
    }

    len = strlen(str);
    new_str = (char *)malloc((len + 1) * sizeof(char)); /* Allocate */

    if (new_str == NULL) { 
        fprintf(stderr, "Error: Could not allocate memory for string copy\n");
        return NULL; 
    }

    strcpy(new_str, str); /* Copy the string */
    return new_str;
}