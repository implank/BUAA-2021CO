/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * assembler_utils.h
 *****************************************************************/
#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

#include <stdint.h>
#include "../lib/tables.h"

extern const char *ASSEMBLER_IGNORE_CHARS; //" \f\n\r\t\v,()"

extern const int MAX_ARGS;

extern const int ASSEMBLER_BUF_SIZE;

/* Reads STR and determines whether it is a label (ends in ':'), and if so,
 * whether it is a valid label, and then tries to add it to the symbol table.
 *
 * INPUT_LINE is which line of the input file we are currently processing. Note
 * that the first line is line 1 and that empty lines are included in this count.
 *
 * BYTE_OFFSET is the offset of the NEXT instruction (should it exist).
 *
 * Four scenarios can happen:
 *  1. STR is not a label (does not end in ':'). Returns 0.
 *  2. STR ends in ':', but is not a valid label. Returns -1.
 *  3a. STR ends in ':' and is a valid label. Addition to symbol table fails.
 *      Returns -1.
 *  3b. STR ends in ':' and is a valid label. Addition to symbol table succeeds.
 *      Returns 1.
 */
int add_if_label(uint32_t input_line, char *str, uint32_t byte_offset, SymbolTable *symtbl);

void raise_label_error(uint32_t input_line, const char *label);

/* Call this function if more than MAX_ARGS arguments are found while parsing
 * arguments.
 *
 * INPUT_LINE is which line of the input file that the error occurred in. Note
 * that the first line is line 1 and that empty lines are included in the count.
 *
 * EXTRA_ARG should contain the first extra argument encountered.
 */
void raise_extra_arg_error(uint32_t input_line, const char *extra_arg);

/* You should call this function if write_pass_one() or translate_inst()
   returns -1.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.
 */
void raise_inst_error(uint32_t input_line, const char *name, char **args, int num_args);

/*  A helpful helper function that parses instruction arguments. It raises an error
 *  if too many arguments have been passed into the instruction.
 */
int parse_args(uint32_t input_line, char **args, int *num_args);

/* Truncates the string at the first occurrence of the '#' character. */
void skip_comment(char *str);

#endif