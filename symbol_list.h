#ifndef SYMBOL_LIST_H
#define SYMBOL_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/* Structure representing the (max of 2) possibles types of a symbol */
typedef struct {
    TypeWord type1; /* The first possible type of the symbol */
    TypeWord type2; /* The second possible type of the symbol */
} SymbolTypes;

/* Structure representing a symbol node */
typedef struct SymbolNode {
    char name[MAX_SYMBOL_NAME_LENGTH]; /* The name of the symbol */
    int value; /* The value (IC) of the symbol */
    SymbolTypes types; /* The types of the symbol */
    struct SymbolNode* next; /* Pointer to the next symbol */
} SymbolNode;

/* Structure representing a list of the extern labels 
 * (cant be double values of the same label in the symbol table) */
typedef struct ExternNode {
    char name[MAX_SYMBOL_NAME_LENGTH]; /* The name of the symbol */
    int value; /* The value (IC) of the symbol */
    struct ExternNode* next; /* Pointer to the next extern */
} ExternNode;

extern ExternNode* symbol_extern_list_head; /* The head of the extern list */
extern SymbolNode* symbol_list_head; /* The head of the symbol list */

/*   EXTERN   */
/**
 * Creates a new extern symbol node
 * @param name The name of the symbol
 * @param value The value of the symbol
 * @return The new symbol node
 */
ExternNode* create_extern_list_node(char* name, int value);

/**
 * Prints the extern list to a file
 * @param head The head of the extern list
 * @param filename The name of the file to print to
 * @return 0 if the list was printed successfully, -1 if error found or there is nothing to print 
 */
 int print_extern_list_to_file(ExternNode* head, char* filename);   
 
/**
 * Free the extern list
 * @param head The head of the extern list
 */
void free_extern_list(ExternNode* head);

/**
 * Add a new extern node to the extern list
 * @param head The head of the extern list
 * @param name The name of the symbol
 * @param value The value of the symbol to add
 */
void append_extern_node(ExternNode** head, char* name, int value);

/*   ENTRY    */

/**
 * Creates a new symbol node
 * @param name The name of the symbol
 * @param value The value of the symbol
 * @param type1 The first possible type of the symbol
 * @param type2 The second possible type of the symbol
 * @return The new symbol node
 */
SymbolNode* create_symbol_node(char* name, int value, TypeWord type1, TypeWord type2);

/**
 * Sorts the symbols by their value (IC)
 * @param head The head of the symbol list
 */
void sort_symbols_by_value(SymbolNode** head);

/**
 * Prints the entry symbols to a file
 * @param filename The name of the file to print to
 * @return 0 if the list was printed successfully, -1 if error found or there is nothing to print 
 */
 int print_entry_symbols_to_file(char* filename) ;

/**
 * frees the symbol list
 * @param head The head of the symbol list to free
 */
void free_symbols(SymbolNode* head);

/**
 * Appends a new symbol node to the symbol list
 * @param head The head of the symbol list
 * @param name The name of the symbol
 * @param value The value of the symbol
 * @param type1 The first possible type of the symbol
 * @param type2 The second possible type of the symbol
 * @return 0 if the symbol was added successfully, -1 otherwise
 */
int append_symbol(SymbolNode** head, char* name, int value, TypeWord type1, TypeWord type2);

/**
 * Finds a symbol by name
 * @param head The head of the symbol list
 * @param name The name of the symbol to find
 * @return The symbol if found, NULL otherwise
 */
SymbolNode* find_symbol(SymbolNode* head, char* name);

/**
 * Adds a new symbol to the symbol list or updates an existing one (if found)
 * @param head The head of the symbol list
 * @param name The name of the symbol
 * @param value The value of the symbol
 * @param type The type of the symbol
 * @return 0 if the symbol was added successfully, -1 otherwise
 */
int add_or_update_symbol(SymbolNode** head, char* name, int value, TypeWord type);

/**
 * Adds a new type to an existing symbol
 * @param head The head of the symbol list
 * @param name The name of the symbol
 * @param new_type The new type to add
 * @return 0 if the type was added successfully, -1 otherwise
 */
int add_type_to_symbol(SymbolNode* head, char* name, TypeWord new_type);

/**
 * Updates the value of an existing symbol
 * @param head The head of the symbol list
 * @param name The name of the symbol
 * @param new_value The new value to set
 * @return 0 if the value was updated successfully, -1 otherwise
 */
int update_symbol_value(SymbolNode* head, char* name, int new_value);

/**
 * Checks if a type conflict exists between two types (extern vs. entry)
 * @param existing_type The existing type
 * @param new_type The new type
 * @return 0 if no conflict exists, -1 otherwise
 */
int check_type_conflict(TypeWord existing_type, TypeWord new_type);

extern int IC; /* The instruction counter */
extern int DC; /* The data counter */

#endif