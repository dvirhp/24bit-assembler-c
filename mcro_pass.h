#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

#define MAX_MACRO_NAME_LENGTH 31 /* Max length of legal macro name */

/* Structure representing a macro */
typedef struct Macro {
    char name[MAX_MACRO_NAME_LENGTH]; /* The name of the macro */
    char* content; /* Pointer to the content of the macro */
    struct Macro* next; /* Pointer to the next macro */
} Macro;

/**
 * Checks if a word is a legal macro name 
 * @param name The name to check
 * @return 1 if the name is illegal, 0 otherwise
 */
int is_illegal_macro_name(char *name);

/**
 * Finds a macro by name from the list of macros
 * @param macros The list of macros
 * @param name The name of the macro to find
 * @return The macro if found, NULL otherwise
 */
Macro* find_macro(Macro* macros, char* name);

/**
 * Removes the extension from a file name (used to replace the .as extension with .am)
 * @param filename The file name to work on
 */
void remove_extension(char* filename);

/**
 * Adds a macro to the list of macros
 * @param macros The list of macros
 * @param name The name of the macro
 * @param content The content of the macro
 * @param error_found Pointer to a macro that indicates if an error was found and where
 * to allow full pass of the code and then return the error
 */
 void add_macro(Macro** macros, char* name, char* content, int* error_found);

/**
 * Expands macros in the input file
 * @param input_file The input file to expand
 * @return The output file name
 */
char* expand_macros(char* input_file);