#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcro_pass.h"
#include "first_pass.h"
#include "symbol_list.h"
#include "machine_code.h"
#include "second_pass.h"

#define MAX_LINE_LENGTH 80
int line_IC = 100; /* Set local IC to 100 */

/**
 * Executes the first pass on the given output file
 * @param output_file The name of the output file to process
 * @return int 0 if successful, 1 otherwise
 */
int firstpass(char *output_file);

/**
 * Executes the second pass on the given output file
 * @param output_file The name of the output file to process
 * @return int 0 if successful, 1 otherwise.
 */
int secondpass(char *output_file);

/**
 * Frees allocated memory for file names and lists
 * @param expanded_file The expanded file name
 * @param output_file The output file name
 * @param ob_file The object file name
 * @param ent_file The entry file name
 * @param ext_file The extern file name
 */
void free_allocated_memory(char *expanded_file, char *output_file, char *ob_file, char *ent_file, char *ext_file) {
    free(expanded_file);
    free(output_file);
    free(ob_file);
    free(ent_file);
    free(ext_file);
    free_symbols(symbol_list_head);
    free_extern_list(symbol_extern_list_head);
    free_list_bits_fields(bits_fields_head);
}

/**
 * Main function to process input files include macro expansion, first pass, and second pass
 * @param argc The number of command-line arguments
 * @param argv The array of command-line arguments
 * @return int 0 if successful, 1 otherwise.
 */
int main(int argc, char *argv[]) {
    int i; /* Loop index */
    char *input_file; /* Input file name */
    char *output_file; /* Output file name */
    char *ob_file; /* Object file name */
    char *ent_file; /* Entry file name */
    char *ext_file; /* Extern file name */
    char *expanded_file; /* Expanded file name */
    int firstpass_error; /* First pass error flag */
    int secondpass_error; /* Second pass error flag */
    int ent_printed; /* Entry file printed flag */
    int ext_printed; /* Extern file printed flag */
    FILE *file; /* File pointer */

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file1> <input file2> ...\n", argv[0]);
        return 1;
    }
    for (i = 1; i < argc; ++i) {
        input_file = argv[i];
        
        /* Allocate memory for file names */
        output_file = (char*)malloc(strlen(input_file) + 4); /* +4 for ".am" and null terminator */
        ob_file = (char*)malloc(strlen(input_file) + 4); /* +4 for ".ob" and null terminator */
        ent_file = (char*)malloc(strlen(input_file) + 5); /* +5 for ".ent" and null terminator */
        ext_file = (char*)malloc(strlen(input_file) + 5); /* +5 for ".ext" and null terminator */

        if (!output_file || !ob_file || !ent_file || !ext_file) {
            fprintf(stderr, "Error: Memory allocation failed for file names.\n");
            free_allocated_memory(NULL, output_file, ob_file, ent_file, ext_file);
            return 1;
        }

        strcpy(output_file, input_file);
        strcat(output_file, ".as"); /* Add the .as extension */

        file = fopen(output_file, "r");
        if (!file) {
            fprintf(stderr, "Error: File '%s' does not exist or is not a valid file. Skipping to next file.\n", input_file);
            free_allocated_memory(NULL, output_file, ob_file, ent_file, ext_file);
            continue;
        }
        fclose(file);

        expanded_file = expand_macros(output_file);
        if (expanded_file == NULL) {
            fprintf(stderr, "Error expanding macros for '%s'. Skipping to next file.\n", input_file);
            free_allocated_memory(NULL, output_file, ob_file, ent_file, ext_file);
            continue;
        }

        firstpass_error = firstpass(expanded_file); /* Execute the first pass */
        secondpass_error = secondpass(expanded_file); /* Execute the second pass */

        if (firstpass_error != 0 || secondpass_error != 0) {
            fprintf(stderr, "Error processing file '%s'. Skipping file creation.\n", expanded_file);
            free_allocated_memory(expanded_file, output_file, ob_file, ent_file, ext_file);
            continue;
        }

        /* Create output file names */
        strcpy(ob_file, input_file);
        strcat(ob_file, ".ob");
        strcpy(ent_file, input_file);
        strcat(ent_file, ".ent");
        strcpy(ext_file, input_file);
        strcat(ext_file, ".ext");

        /* Print the bit fields, entry symbols, and extern list to files */
        if (print_bits_fields_to_file(ob_file) != 0) {
            fprintf(stderr, "Error printing bit fields to file '%s'. Skipping file creation.\n", ob_file);
            free_allocated_memory(expanded_file, output_file, ob_file, ent_file, ext_file);
            continue;
        }

        ent_printed = print_entry_symbols_to_file(ent_file); 
        ext_printed = print_extern_list_to_file(symbol_extern_list_head, ext_file);

        printf("Output files created:\n");
        printf("  %s.am\n", input_file);
        printf("  %s.ob\n", input_file);
        if (ent_printed == 0) {
            printf("  %s.ent\n", input_file);
        }
        if (ext_printed == 0) {
            printf("  %s.ext\n", input_file);
        }

        /* Free allocated memory */
        free_allocated_memory(expanded_file, output_file, ob_file, ent_file, ext_file);
    }
    return 0;
}

int firstpass(char *output_file) {
    char line[MAX_LINE_LENGTH + 2]; /* Allow space for newline and null terminator */
    FILE* outfile;
    char* trimmed_line;
    int error_found = 0; /* Flag */
    int is_empty; /* Flag */
    int i;
    line_IC = 100; /* Set local IC to 100 */

    outfile = fopen(output_file, "r");
    if (!outfile) {
        fprintf(stderr, "Error opening output file '%s'.\n", output_file);
        return 1; 
    }
    while (fgets(line, sizeof(line), outfile)) {
        line_IC++;
        line[strcspn(line, "\n")] = '\0'; /* Remove newline character */
        trimmed_line = trim_whitespace(line);
        if (trimmed_line == NULL) {
            fprintf(stderr, "Error: Could not allocate memory for trimmed line.\n");
            error_found = 1;
            continue;
        }
        is_empty = 1; /* Flag */

        for (i = 0; trimmed_line[i] != '\0'; i++) {
            if (!isspace((char)trimmed_line[i])) {
                is_empty = 0;
                break;
            }
        }
    
        if (is_empty) {
            continue;
        }
        if (trimmed_line[0] == ';') { /* Skip ";" lines */
            continue;
        }
        if (process_string(trimmed_line) == 0) {
            error_found = 1; /* Set the error flag */
        }
    }

    fclose(outfile);
    return error_found; 
}

int secondpass(char *output_file) {
    char line[MAX_LINE_LENGTH];
    FILE* outfile;
    char* trimmed_line;
    int error_found = 0; /* Flag */
    int is_empty; /* Flag */
    int i;
    line_IC = 100; /* Reset the local IC */

    outfile = fopen(output_file, "r");
    if (!outfile) {
        fprintf(stderr, "Error opening output file '%s' for second pass.\n", output_file);
        return 1; 
    }
    while (fgets(line, MAX_LINE_LENGTH, outfile)) {
        line_IC++;
        line[strcspn(line, "\n")] = '\0'; /* Remove newline character */
        trimmed_line = trim_whitespace(line);
        if (trimmed_line == NULL) {
            fprintf(stderr, "Error: Could not allocate memory for trimmed line.\n");
            fclose(outfile);
            return 1;
        }

        is_empty = 1;

        for (i = 0; trimmed_line[i] != '\0'; i++) {
            if (!isspace((char)trimmed_line[i])) {
                is_empty = 0;
                break;
            }
        }
    
        if (is_empty) {
            continue;
        }
        if (trimmed_line[0] == ';') { /* Skip ";" lines*/
            continue;
        }
        if (process_line_second_pass(trimmed_line) == 0) {
            error_found = 1; /* Set the error flag */
        }
    }

    fclose(outfile);
    return error_found;
}