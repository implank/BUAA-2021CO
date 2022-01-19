/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * linker_utils.h
 *****************************************************************/

#ifndef LINKER_UTILS_H
#define LINKER_UTILS_H

#include "../lib/tables.h"

#define LINKER_BUF_SIZE 1024
#define LINKER_IGNORE_CHARS " \n\r\t\v\f"

typedef struct {
    SymbolTable *table;
    int text_size;
    int data_size;
} RelocData;

/*
 * Given an instruction that needs relocation, relocate the instruction based on
 * the given symbol and relocation table.
 *
 * Note that we need to handle the relocation symbols for the .data segment and
 * the relocation symbols for the .text segment separately.
 *
 * For the .text segment, the symbols will only appear in the jump instruction
 *
 * For .data, it will only appear in the lui and ori instructions,
 * and we have processed it in the assembler as label@Hi/Lo
 *
 * You should return error if:
 * (1). the addr is not in the relocation table.
 * (2). the symbol name is not in the symbol table.
 * Otherwise, assume that the relocation will perform successfully.
 *
 * Return:
 * the relocated instruction, -1 if error.
 *
 * Arguments:
 * inst: an instruction that needs relocate.
 * offset: the byte offset of the instruction in the current file.
 * symtbl: the symbol table.
 * reltbl: the relocation table.
 */
int32_t relocate_inst(uint32_t inst, uint32_t offset, SymbolTable *symtbl, SymbolTable *reltbl);


/*
 * Compute number of bytes that the text section takes up. This fuction assumes
 * that when called, the file pointer is currently at the beginning of the text
 * section. It also assumes that there will be one instruction per line, and
 * that the .text section ends with a blank line.
 *
 * Return value:
 * size of the text section in bytes.
 *
 * Arguments:
 * input: file pointer, must be pointing to the beginning of the text section.
 */
int calc_text_size(FILE *input);


/*
 * Calculates the number of bytes occupied by the .data of the current file.
 * Note that after the assembler has processed it, we have already calculated
 * the number of bytes in the .data segment. It is saved at the beginning of
 * the .out file as ".data \n bytes".
 *
 * Return value:
 * Returns the number of bytes occupied by the .data of the file INPUT
 */
int calc_data_size(FILE *input);

/*
 * Add entries from the .symbol OR .relocation section into the SymbolTable.
 * Each line must be of the format "<number>\t<string>\n", and the section ends
 * with a blank line. The file pointer must be at the beginning of the section
 * (the line immediately after .symbol or .relocation).
 *
 * Similarly, when we add symbols to the symbol table, we need to process the symbols
 * in the .data segment and the .text segment separately. The symbols in the .data
 * segment should have an offset based on base_data_offset, and the symbols in the
 * .text segment should have an offset based on base_text_offset. The two are
 * distinguished by the character % that the assembler adds to the beginning of the
 * .data segment symbols
 * The function will DELETE the % if it is a .data segment symbol. Then add the symbol
 * to the table.
 * 
 * Return:
 * 0 if no errors, -1 if error.
 */
int add_to_symbol_table(FILE *input, SymbolTable *table, uint32_t base_text_offset, uint32_t base_data_offset);

/*
 * Builds the symbol table and relocation data for a single file.
 * Read the .data, .text, .symbol, .relocation segments in that order.
 * The size of the .data and .text segments are read and saved in the
 * relocation table of the current file. For the .symbol and .relocation
 * segments, save the symbols in them in the corresponding locations.
 *
 * Return value:
 * 0 if no errors, -1 if error.
 *
 * Arguments:
 * input:            file pointer.
 * symtbl:           symbol table.
 * reldt:            pointer to a Relocdata struct.
 * base_text_offset: base text offset.
 * base_data_offset: base data offset.
 */
int fill_data(FILE *input, SymbolTable *symtbl, RelocData *reldt, uint32_t base_text_offset, uint32_t base_data_offset);

#endif
