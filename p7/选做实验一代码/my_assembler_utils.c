/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * my_assembler_utils.c
 * Assembler Submission
 *****************************************************************/
#include "my_assembler_utils.h"
#include "assembler-src/assembler_utils.h"
#include "lib/translate_utils.h"

#include <string.h>
#include <stdlib.h>

#include <limits.h>

const char *MY_IGNORE_CHARS=" \f\n\r\t\v,():";

/*
 * This function reads .data symbol from INPUT and add them to the SYMTBL
 * Note that in order to distinguish in the symbol table whether a symbol
 * comes from the .data segment or the .text segment, we append a % to the
 * symbol name when adding the .data segment symbol. Since only underscores and
 * letters will appear in legal symbols, distinguishing them by adding % will
 * not cause a conflict between the new symbol and the symbol in the assembly file.
 *
 * Return value:
 * Return the number of bytes in the .data segment.
 */
int read_data_segment(FILE *input, SymbolTable *symtbl) {
	char buf[ASSEMBLER_BUF_SIZE];
	while(fgets(buf, ASSEMBLER_BUF_SIZE, input)){
		if (strcmp(strtok(buf, ASSEMBLER_IGNORE_CHARS), ".data") == 0) {
			break;
		}
	}
	uint32_t addrs = 0;
	while(fgets(buf, ASSEMBLER_BUF_SIZE, input)){
		if(strlen(buf)==1)break;
		skip_comment(buf);
		int len=strlen(buf),f=0;
		for(int i=0;i<len;++i){
			if(buf[i]==':')
				f=1;
		}
		if(!f)continue;

		if(symtbl -> len == symtbl -> cap){
			symtbl -> cap = symtbl -> cap * SCALING_FACTOR;
			symtbl -> tbl = (Symbol*) realloc(symtbl -> tbl, symtbl -> cap * sizeof(Symbol));
		}
		((symtbl -> tbl) + symtbl -> len) -> addr = addrs ;

		char *token;
		token = strtok(buf, MY_IGNORE_CHARS);
		char tmp[3000]={"%"};
		((symtbl -> tbl) + symtbl -> len) -> name = create_copy_of_str(strcat(tmp, token));

		token = strtok(NULL, MY_IGNORE_CHARS);
		long int space;
		token = strtok(NULL, MY_IGNORE_CHARS);
		translate_num(&space, token, 0, INT_MAX);
		
		addrs += space;
		symtbl -> len += 1;

	}
	
	return addrs ;
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE.
 * ADDR is given as the byte offset from the first ins. The SymbolTable
 * must be able to resize itself as more elements are added.
 *
 * Note that NAME may point to a temporary array, so it is not safe to simply
 * store the NAME pointer. You must store a copy of the given string.
 *
 * If ADDR is not word-aligned, you should call addr_alignment_incorrect() and
 * return -1. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists
 * in the table, you should call name_already_exists() and return -1. If memory
 * allocation fails, you should call allocation_failed().And alloction_failed()
 * will print error message and exit with error code 1.
 *
 * Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable *table, const char *name, uint32_t addr) {
	if(table -> len == table -> cap){
		table -> cap = table -> cap * SCALING_FACTOR;
		table -> tbl = (Symbol*) realloc(table -> tbl, table -> cap * sizeof(Symbol));
	}
	
	((table -> tbl) + (table -> len)) -> name = create_copy_of_str(name);
	((table -> tbl) + (table -> len)) -> addr = addr;

	(table -> len) += 1;

	return 0;
}

/*
 * Convert lui inss to machine code. Note that for the imm field of lui,
 * it may be an immediate number or a symbol and needs to be handled separately.
 * Output the ins to the **OUTPUT**.(Consider using write_inst_hex()).
 * 
 * Arguments:
 * opcode:     op segment in MIPS machine code
 * args:       args[0] is the $rt register, and args[1] can be either an imm field or 
 *             a .data segment label. The other cases are illegal and need not be considered
 * num_args:   length of args array
 * addr:       Address offset of the current ins in the file
 */
int write_lui(uint8_t opcode, FILE *output, char **args, size_t num_args, uint32_t addr, SymbolTable *reltbl) {
	uint32_t ins = opcode<<26;
	long int imm;
	ins |= translate_reg(args[0])<<16;
	int err = translate_num(&imm, args[1], 0, UINT16_MAX);

	if (err == -1) {
		add_to_table(reltbl, args[1], addr);
		imm = 0;
	}
	ins |= imm;
	
	write_inst_hex(output,ins);
	return 0;
}

