#include "mcro_pass.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int is_illegal_macro_name(char *name) {
    int i;
    if (!isalpha(name[0]) && name[0] != '_') { /* Illegal */
        return 1;
    }
    if (strlen(name) > MAX_MACRO_NAME_LENGTH) { /* Illegal */
        return 1;
    }
    for (i = 0; i < NUM_COMMANDS; i++) {
        if (strcmp(name, opcodeStrings[cmd_lookup_table[i].opcode_name]) == 0) { /* Opcode name is illegal */
            return 1;
        }
    }
    for (i = 1; name[i] != '\0'; i++) {
        if (!isalnum(name[i]) && name[i] != '_') { /* Non alphanumeric is illegal */
            return 1;
        }
    }
    for (i = 0; instructions_lookup_table[i].name != NULL; i++) { /* Instrucion type name is illegal */
        if (strcmp(name, instructions_lookup_table[i].name) == 0) {
            return 1;
        }
    }
    for (i = 0; register_lookup_table[i].name; i++) {
        if (strcmp(name, register_lookup_table[i].name) == 0) { /* Register name is illegal */
            return 1;
        }
    }
    return 0;
}

Macro* find_macro(Macro* macros, char* name) {
    while (macros != NULL) {
        if (strcmp(macros->name, name) == 0) {
            return macros;
        }
        macros = macros->next; /* Next macro */
    }
    return NULL;
}

void add_macro(Macro** macros, char* name, char* content, int* error_found) {
    Macro* new_macro; /* New macro */

    if (is_illegal_macro_name(name)) {
        *error_found = 1;
        return;
    }
    new_macro = (Macro*)malloc(sizeof(Macro)); /* Allocate memory */
    if (!new_macro) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        *error_found = 1;
        return;
    }
    strcpy(new_macro->name, name); /* Copying */
    if (content == NULL) {
        content = ""; /* Set content to an empty string if it is NULL */
    }
    new_macro->content = (char*)malloc(strlen(content) + 1);
    if (!new_macro->content) {
        fprintf(stderr, "Error: Memory allocation failed for macro content.\n");
        free(new_macro);
        *error_found = 1; /* Set the flag */
        return;
    }

    strcpy(new_macro->content, content);
    new_macro->next = *macros; /* Next macro */
    *macros = new_macro; /* Set the new macro */
}

void remove_extension(char* filename) {
    char* dot = strrchr(filename, '.'); /* Pointer to the dot */
    if (dot != NULL) {
        *dot = '\0'; /* Replace the dot with \0 */
    }
}

char* expand_macros(char* input_file) {
    FILE* infile; /* Input file pointer */
    FILE* outfile; /* Output file pointer */
    char* output_file; /* Output file name */
    char* ptr;  /* Temporary pointer */
    char line[MAX_LINE_LENGTH + 2];
    Macro* macros = NULL; /* Linked list of macros */
    int in_macro = 0; /* Flag indicating if currently inside a macro definition */
    char macro_name[MAX_MACRO_NAME_LENGTH]; 
    char* macro_content = NULL;
    int macro_content_size = 0; /* Size of the macro content buffer */
    int line_number = 0; /* Current line number */
    int error_found = 0; /* Flag indicating if an error was found */

    /* Open the input file for reading */
    infile = fopen(input_file, "r");
    if (!infile) {
        fprintf(stderr, "Error opening input file '%s'.\n", input_file);
        return NULL;
    }

    output_file = (char*)malloc(strlen(input_file) + 4); /* Allocate memory */
    if (!output_file) {
        fprintf(stderr, "Error: Memory allocation failed for output file name.\n");
        fclose(infile);
        return NULL;
    }

    strcpy(output_file, input_file);
    remove_extension(output_file); /* Remove the existing extension */
    strcat(output_file, ".am"); /* Add the new extension */

    outfile = fopen(output_file, "w");
    if (!outfile) {
        fprintf(stderr, "Error opening output file '%s'.\n", output_file);
        fclose(infile);
        free(output_file);
        return NULL;
    }

    while (fgets(line, sizeof(line), infile)) {
        if (strlen(line) > MAX_LINE_LENGTH) {
            fprintf(stderr, "Error: Line %d is too long (more than %d characters).\n", line_number + 1, MAX_LINE_LENGTH);
            error_found = 1;
            continue;
        }
        line_number++; /* Increment line number */

        ptr = trim_whitespace(line); /* Trim */

        if (ptr[0] == '\0' || ptr[0] == ';') { /* Skip empty lines and comments */
            continue;
        }


        if (strncmp(ptr, "mcro ", 5) == 0 && !in_macro) {
            ptr += 5; /* Skip the opener */
            ptr = trim_whitespace(ptr); /* Trim */
            if (*ptr == '\0' || *ptr == '\n') { /* If there are no chars to define */
                fprintf(stderr, "Error: Invalid macro definition '%s' at line %d.\n", line, line_number);
                error_found = 1;
                continue;
            }
        
            in_macro = 1; /* Set the flag */
            strcpy(macro_name, trim_whitespace(ptr));
            macro_name[strcspn(macro_name, "\n")] = '\0'; /* Replace the newline with \0 */
            if (is_illegal_macro_name(macro_name)) {
                error_found = 1;
                fprintf(stderr, "Error: Illegal macro name '%s' at line %d.\n", macro_name, line_number);                
                in_macro = 0; /* Reset the flag */
                continue;
            }
            macro_content_size = 0;
            if (macro_content) {
                free(macro_content);
                macro_content = NULL;
            }
              
        } else if ((strncmp(ptr, "mcroend", 7) == 0 )&& in_macro) {
            ptr += 7; /* Skip the opener */
            ptr = trim_whitespace(ptr); /* Trim */
            if (*ptr != '\0' && *ptr != '\n') { /* No more chars after the mcroend allowed */
                fprintf(stderr, "Error: Invalid macro end definition '%s' at line %d.\n", line, line_number);
                error_found = 1;
                continue;
            }

            in_macro = 0; /* Reset the flag */
            add_macro(&macros, macro_name, macro_content, &error_found); /* Add the macro */
            free(macro_content);
            macro_content = NULL;

        } else if (in_macro) { /* In the macro */
            int line_length = strlen(line);
            macro_content = (char*)realloc(macro_content, macro_content_size + line_length + 2); /* +2 for newline and null terminator */
            if (!macro_content) {
                fprintf(stderr, "Error: Memory allocation failed for macro content.\n");
                error_found = 1;
                break;
            }
            strcpy(macro_content + macro_content_size, line);
            macro_content_size += line_length;
            strcat(macro_content, "\n"); /* Add newline character */
            macro_content_size += 1; /* Increment size for newline */
        }
    }

    rewind(infile);
    line_number = 0; /* Reset line number */

    while (fgets(line, MAX_LINE_LENGTH, infile)) {
        char* original_ptr; /* Original pointer */
        Macro* macro; 
        line_number++; /* Increment line number */

        if (strlen(line) > MAX_LINE_LENGTH) {
            fprintf(stderr, "Error: Line %d is too long (more than %d characters).\n", line_number + 1, MAX_LINE_LENGTH);
            error_found = 1;
            continue;
        }

        ptr = trim_whitespace(line); 

        if (ptr[0] == '\0' || ptr[0] == ';') { /* Skip empty lines and comments */
            continue;
        }


        if (strncmp(ptr, "mcro ", 5) == 0) { 
            while (fgets(line, MAX_LINE_LENGTH, infile)) {
                if (strlen(line) > MAX_LINE_LENGTH) {
                    fprintf(stderr, "Error: Line %d is too long (more than %d characters).\n", line_number + 1, MAX_LINE_LENGTH);
                    error_found = 1;
                    continue;
                }

                ptr = trim_whitespace(line);

                if (strncmp(ptr, "mcroend", 7) == 0) {
                    break;
                }
            }
            continue;
        }

        ptr[strcspn(ptr, "\n")] = '\0'; /* Replace the newline with \0 */
        original_ptr = allocate_and_copy_string(ptr);
        
        macro = find_macro(macros, ptr);
        if (macro) {
            fputs(macro->content, outfile);
        } else {
            fputs(line, outfile); /* Copy the line */
            fputs("\n", outfile); 
        } 

        free(original_ptr);
    }

    fclose(infile);
    fclose(outfile);

    while (macros) {
        Macro* temp = macros;
        macros = macros->next;
        free(temp->content); /* Free the temp */
        free(temp);
    }

    return output_file;
}