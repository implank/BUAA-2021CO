/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * translate_utils.h
 *****************************************************************/

#ifndef TRANSLATE_UTILS_H
#define TRANSLATE_UTILS_H

#include <stdint.h>

/* Writes the instruction as a string to OUTPUT. NAME is the name of the
 * instruction, and its arguments are in ARGS. NUM_ARGS is the length of
 * the array.
 */
void write_inst_string(FILE *output, const char *name, char **args, int num_args);

/* Writes the instruction to OUTPUT in hexadecimal format. */
void write_inst_hex(FILE *output, uint32_t instruction);

/* Returns 1 if the label is valid and 0 if it is invalid. A valid label is one
 * where the first character is a character or underscore and the remaining
 * characters are either characters, digits, or underscores.
 */
int is_valid_label(const char *str);

/* Translate the input string into a signed number. The number is then
 *  checked to be within the correct range (note bounds are INCLUSIVE)
 *  ie. NUM is valid if LOWER_BOUND <= NUM <= UPPER_BOUND.
 *
 *  The input may be in either positive or negative, and be in either
 *  decimal or hexadecimal format. It is also possible that the input is not
 *  a valid number. Fortunately, the library function strtol() can take
 *  care of all that.
 *
 *  You should store the result into the location that OUTPUT points to. The
 *  function returns 0 if the conversion proceeded without errors, or -1 if an
 *  error occurred.
 */
int translate_num(long int *output, const char *str, long int lower_bound,
                  long int upper_bound);

/* Translates the register name to the corresponding register number.
 *
 * Returns the register number of STR or -1 if the register name is invalid.
 */
int translate_reg(const char *str);

#endif
