#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_COMMANDS 17 /* Number of commands */
#define MAX_COMMAND_NAME_LENGTH 6 /* Max length of a command name */
#define MAX_LINE_LENGTH 80 /* Max length of a line */
#define MAX_SOURCE_LENGTH 80 /* Max length of a source */
#define MAX_DESTINATION_LENGTH 80 /* Max length of destenation */
#define MAX_RESERVE_ARRAY_LENGTH 4 /* Max length of reserved words array */
#define MAX_SYMBOL_NAME_LENGTH 31 /* Max length of a symbol name */

extern int line_IC;
/* Enum representing different types of words in the assembler */
typedef enum {
    DATA,
    ENTRY,
    EXTERN,
    STRING,
    OPCODE,
    LABEL,
    CODE,
    UNKNOWN
} TypeWord;

/* Enum representing different opcodes */
typedef enum {
    MOV,
    CMP,
    ADD,
    SUB,
    LEA,
    CLR,
    NOT,
    INC,
    DEC,
    JMP,
    BNE,
    JSR,
    RED,
    PRN,
    RTS,
    STOP,
    OP_NONE
} Opcode;

/* Structure representing a command element */
typedef struct CommandLookupElement {
    Opcode opcode_name; /* The opcode converted to Opcode typedef */
    int opcode_num; /* The numeric value that represent the opcode */
    int opcode_funct; /* The funct value that match the opcode */
} CommandLookupElement;

/* Structure representing an instruction element */
typedef struct InstructionLookupItem {
    char *name;
    int type;/* Number represent the type of the instruction  */
} InstructionLookupItem;

/* Structure representing a register */
typedef struct Register {
    char *name;
} Register;

/* Enum representing a reserved word */
typedef enum {
    ZERO,
    ONE,
    TWO,
    THREE,
    INVALID
} ReservedWord;

/* Structure representing an opcode line with the 2 values included */
typedef struct {
    Opcode opcode;
    char source[MAX_SOURCE_LENGTH];
    char destination[MAX_DESTINATION_LENGTH];
} OpcodeStruct;

/* Structure representing the opcode options */
typedef struct {
    ReservedWord words[MAX_RESERVE_ARRAY_LENGTH]; /* Array of reserved words */
    int count; /* The count of reserved words */
} OpcodeOptions;

/* Structure representing the options for each opcode using the OpcodeOptions struct */
typedef struct {
    char name[MAX_COMMAND_NAME_LENGTH]; /* The name of the command */
    OpcodeOptions OpcodeOptionsSource; /* The source options allowed */
    OpcodeOptions OpcodeOptionsDestination; /* The destination options allowed */
} Command;

extern char* opcodeStrings[]; /* Array of opcodes in string form */
extern char* reservedWordStrings[];  /* Array of reserved words in string form */
extern Command commands[]; /* Array of commands using the struct form */
extern Register register_lookup_table[]; /* Array of registers using the struct form */
extern InstructionLookupItem instructions_lookup_table[]; /* Array of instructions using the struct form */
extern CommandLookupElement cmd_lookup_table[]; /* Array of commands using the struct form */

/** 
 * Trims whitespace from the beginning and end of a string
 * @param str The string to trim
 * @return Pointer to the trimmed string.
 */
char* trim_whitespace(char* str);

/**
 * Checks if a word is a reserved word
 * @param word The word to check
 * @return The correct opcode if the word is a reserved word, OP_NONE otherwise
 */    
Opcode handle_word_opcode(char* word);

/**
 * Checks if a word is a label according to the assigment
 * @param word The word to check
 * @return 1 if the word is a label, 0 otherwise
 */
int is_valid_identifier(char* word);

/**
 * Checks if a word is a number according to the assigment
 * @param str The word to check
 * @return 1 if the word is a legit number, 0 otherwise
 */
int is_valid_number(char* str);

/**
 * Concatenates two strings
 * @param str The first string
 * @param suffix The second string
 * @return The concatenated string
 */
char* strallocat(char* str, char* suffix);

/**
 * Allocates memory and copies a string to prevent memory leaks
 * @param str The string to copy
 * @return The copied string
 */
 char *allocate_and_copy_string(char *str) ;

/**
 * Converts a string to uppercase
 * @param str The string to convert
 */
void to_upper(char* str) ;

/**
 * Converts a string to lowercase
 * @param str The string to convert
 */
void to_lower(char* str);

#endif