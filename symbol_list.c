#include "symbol_list.h"

SymbolNode* symbol_list_head = NULL; /* The head of the symbol list  */
ExternNode* symbol_extern_list_head = NULL; /* The head of the extern list */

int IC = 99; /* Instruction Counter for the first pass */
int DC = 0; /* Data Counter for the first pass */
int IC_second = 99; /* Instruction Counter for the second pass */
int DC_second = 0;/* Data Counter for the second pass */
int temp_DC = 0;/* Temporary Data Counter for the second pass */



SymbolNode* create_symbol_node(char* name, int value, TypeWord type1, TypeWord type2) {
    SymbolNode* new_symbol = (SymbolNode*)malloc(sizeof(SymbolNode));
    if (!new_symbol) { /* Allocation failed */
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    /* Setting the values */
    strncpy(new_symbol->name, name, MAX_SYMBOL_NAME_LENGTH);
    new_symbol->name[MAX_SYMBOL_NAME_LENGTH - 1] = '\0'; 
    new_symbol->value = value;
    new_symbol->types.type1 = type1;
    new_symbol->types.type2 = type2;
    new_symbol->next = NULL;
    return new_symbol;
}

int append_symbol(SymbolNode** head, char* name, int value, TypeWord type1, TypeWord type2) {
    SymbolNode* new_symbol = create_symbol_node(name, value, type1, type2);
    if (!new_symbol) {  /* Allocation failed */
        return -1;
    }

    if (*head == NULL) {
        *head = new_symbol;
    } else {
        SymbolNode* temp = *head;
        while (temp->next != NULL) {/* Find the last node */
            temp = temp->next;
        }
        temp->next = new_symbol; /* Append the new node */
    }

    return 0;
}

int check_type_conflict(TypeWord existing_type, TypeWord new_type) {
    /* Check if there is a conflict between the types */
    if ((existing_type == EXTERN && new_type == ENTRY) || (existing_type == ENTRY && new_type == EXTERN)) {
        return 1;
    }
    return 0; 
}

int add_type_to_symbol(SymbolNode* head, char* name, TypeWord new_type) {
    SymbolNode* temp = head;

    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            if (check_type_conflict(temp->types.type1, new_type) || check_type_conflict(temp->types.type2, new_type)) {
                fprintf(stderr, "Error: Conflict between types for symbol '%s' at line %d\n", name, IC - 100);
                return -1;
            }
            /* Add the new type by the diffrent cases */
            if (temp->types.type1 == UNKNOWN) {
                temp->types.type1 = new_type;
            } else if (temp->types.type2 == UNKNOWN) {
                temp->types.type2 = new_type;
            } else if (temp->types.type1 == CODE) {
                temp->types.type1 = new_type;
            } else if (temp->types.type2 == CODE) {
                temp->types.type2 = new_type;
            } else {
                fprintf(stderr, "Error: Symbol '%s' already has two types  at line %d\n", name, IC - 100);
                return -1;
            }
            return 0;
        }

        temp = temp->next; /* Move to the next node */
    }

    fprintf(stderr, "Error: Symbol '%s' not found at line %d\n", name, IC - 100); 
    return -1;
}

int update_symbol_value(SymbolNode* head, char* name, int new_value) {
    SymbolNode* temp = head;

    while (temp != NULL) { 
        if (strcmp(temp->name, name) == 0) { /* Symbol found */
            temp->value = new_value; /* Update */
            return 0;
        }
        temp = temp->next;
    }
    fprintf(stderr, "Error: Symbol '%s' not found at line %d\n", name, IC - 100);
    return -1;
}

void free_symbols(SymbolNode* head) {
    SymbolNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

SymbolNode* find_symbol(SymbolNode* head, char* name) {
    SymbolNode* current = head;

    while (current != NULL) { /* Iterate over the list */
        if (strcmp(current->name, name) == 0) {
            return current; /* Symbol found */
        }
        current = current->next;
    }
    return NULL;
}

int add_or_update_symbol(SymbolNode** head, char* name, int value, TypeWord type) {
    SymbolNode* symbol = find_symbol(*head, name);
    if (symbol == NULL) { /* Symbol not found */
        return append_symbol(head, name, value, type, UNKNOWN);
    } else { /* Symbol found , update */
        return add_type_to_symbol(*head, name, type);
    }
}

void sort_symbols_by_value(SymbolNode** head) {
    SymbolNode* sorted;
    SymbolNode* current;
    SymbolNode* next;

    if (*head == NULL || (*head)->next == NULL) {
        return;
    }

    sorted = NULL; /* Initialize sorted list */
    current = *head; 

    while (current != NULL) {
        next = current->next;
        if (sorted == NULL || sorted->value > current->value) {
            current->next = sorted;
            sorted = current;
        } else {
            SymbolNode* temp = sorted;
            /* Find the correct position */
            while (temp->next != NULL && temp->next->value <= current->value) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = next;
    }

    *head = sorted;
}

int print_entry_symbols_to_file(char* filename) {
    FILE* file; /* File pointer for the output file */
    SymbolNode* temp; /* Temporary pointer to traverse the symbol list */
    int printed; /* Flag to indicate if any entries were printed */

    file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s' for writing at line %d\n", filename, IC - 100);
        return -1;
    }

    sort_symbols_by_value(&symbol_list_head); /* defined in the start of the page */

    temp = symbol_list_head;
    printed = 0; /* Flag */

    while (temp != NULL) {
        /* Check if the symbol is an entry */
        if (temp->types.type1 == ENTRY || temp->types.type2 == ENTRY) { 
            fprintf(file, "%s %07d\n", temp->name, temp->value);
            printed = 1; /* Set the flag to indicate that something was printed */
        }
        temp = temp->next;
    }

    fclose(file);

    if (!printed) {
        /* If nothing was printed, remove the file and return -1 */
        remove(filename);
        return -1;
    }

    return 0;
}

ExternNode* create_extern_list_node(char* name, int value){
    ExternNode* new_node = (ExternNode*)malloc(sizeof(ExternNode));

    if (!new_node) {
        fprintf(stderr, "Error: Could not allocate memory for new node\n");
        return NULL;
    }
    strncpy(new_node->name, name, MAX_SYMBOL_NAME_LENGTH);
    new_node->name[MAX_SYMBOL_NAME_LENGTH - 1] = '\0'; /* Ensure \0 */
    new_node->value = value;
    new_node->next = NULL;
    return new_node;
}

void append_extern_node(ExternNode** head, char* name, int value) {
    ExternNode* new_node = create_extern_list_node(name, value);

    if (!new_node) { /* Allocation failed */
        return;
    }
    if (*head == NULL) { 
        *head = new_node;
    } else {
        ExternNode* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

int print_extern_list_to_file(ExternNode* head, char* filename) {
    FILE* file; /* File pointer for the output file */
    ExternNode* temp;/* Temporary pointer to traverse the extern list */
    int printed;/* Flag to indicate if any entries were printed */

    file = fopen(filename, "w");
    if (!file) { /* File opening failed */
        fprintf(stderr, "Error: Could not open file '%s' for writing at line %d\n", filename, IC - 100);
        return -1;
    }

    temp = head;
    printed = 0; /* Flag */

    while (temp != NULL) {
        fprintf(file, "%s %07d\n", temp->name, temp->value); /* Padding with zeros if not a 7 digits */
        printed = 1; /* Set the flag to indicate that something was printed */
        temp = temp->next;
    }

    fclose(file);

    if (!printed) {
        /* If nothing was printed, remove the file and return -1 */
        remove(filename);
        return -1;
    }

    return 0;
}

void free_extern_list(ExternNode* head) {
    ExternNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}