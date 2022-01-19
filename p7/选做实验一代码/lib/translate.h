/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * translate.h
 *****************************************************************/

#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <stdint.h>

/*  Writes instructions during the assembler's first pass to OUTPUT. The case
 *  for general instructions has already been completed, but you need to write
 *  code to translate the li and other pseudoinstructions. Your pseudoinstruction
 *  expansions should not have any side effects.
 *
 *  NAME is the name of the instruction, ARGS is an array of the arguments, and
 *  NUM_ARGS specifies the number of items in ARGS.
 *
 *  Error checking for regular instructions are done in pass two. However, for
 *  pseudoinstructions, you must make sure that ARGS contains the correct number
 *  of arguments. You do NOT need to check whether the registers / label are
 *  valid, since that will be checked in part two.
 *
 *  Also for li:
 *   - make sure that the number is representable by 32 bits. (Hint: the number
 *       can be both signed or unsigned).
 *   - if the immediate can fit in the imm field of an addiu instruction, then
 *       expand li into a single addiu instruction. Otherwise, expand it into
 *       a lui-ori pair.
 *
 *  For mul, quo, and rem, the expansions should be pretty straight forward if
 *  you paid attention to $hi and $lo registers.
 *
 *  For la:
 *   - The la instruction will only use the symbols defined in the .data segment.
 *     Also, the symbols defined in the .data segment will only be used in the la instruction.
 *   - The la instruction is converted into a lui-ori pair to load
 *     the absolute address of the symbol into the corresponding register
 *   - Similar to read_data_segment(), we append a @Hi/Lo flag to the symbols that appear in la
 *     to indicate whether the load address is 16 bits higher or 16 bits lower at the time of
 *     conversion. In our implementation, all "la $reg, label" instructions are converted to
 *     "lui $at, label@Hi" followed by an "ori $reg, $at, label@Lo"
 *
 *  IMPORTANT
 *  MARS has slightly different translation rules for li, and it allows numbers
 *  larger than the largest 32 bit number to be loaded with li. You should follow
 *  the above rules if MARS behaves differently.
 *
 *  Use fprintf() to write. If writing multiple instructions, make sure that
 *  each instruction is on a different line.
 *
 *  Returns the number of instructions written (so 0 if there were any errors).
 */
unsigned write_pass_one(FILE *output, const char *name, char **args, int num_args);

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.
 *
 * NAME is the name of the instruction, ARGS is an array of the arguments, and
 * NUM_ARGS specifies the number of items in ARGS.
 *
 * The symbol table (SYMTBL) is given for any symbols that need to be resolved
 * at this step. If a symbol should be relocated, it should be added to the
 * relocation table (RELTBL), and the fields for that symbol should be set to
 * all zeros.
 *
 * You must perform error checking on all instructions and make sure that their
 * arguments are valid. If an instruction is invalid, you should not write
 * anything to OUTPUT but simply return -1.
 *
 * Returns 0 on success and -1 on error.
 */
int translate_inst(FILE *output, const char *name, char **args, size_t num_args,
                   uint32_t addr, SymbolTable *symtbl, SymbolTable *reltbl);

/* A helper function for writing most R-type instructions. You should use
 * translate_reg() to parse registers and write_inst_hex() to write to
 * OUTPUT. Both are defined in translate_utils.h.
 */
int write_rtype(uint8_t funct, FILE *output, char **args, size_t num_args);

/* A helper function for writing shift instructions. You should use
 * translate_num() to parse numerical arguments. translate_num() is defined
 * in translate_utils.h.
 */
int write_shift(uint8_t funct, FILE *output, char **args, size_t num_args);

int write_jr(uint8_t funct, FILE *output, char **args, size_t num_args);

int write_addiu(uint8_t opcode, FILE *output, char **args, size_t num_args);

int write_ori(uint8_t opcode, FILE *output, char **args, size_t num_args, uint32_t addr, SymbolTable *reltbl);

int write_mem(uint8_t opcode, FILE *output, char **args, size_t num_args);

int write_branch(uint8_t opcode, FILE *output, char **args, size_t num_args,
                 uint32_t addr, SymbolTable *symtbl);

int write_jump(uint8_t opcode, FILE *output, char **args, size_t num_args,
               uint32_t addr, SymbolTable *reltbl);

#endif
