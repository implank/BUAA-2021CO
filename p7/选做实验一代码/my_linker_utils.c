/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * my_linker_utils.c
 * Linker Submission
 *****************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lib/tables.h"
#include "linker-src/linker_utils.h"

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
int
fill_data(FILE *input, SymbolTable *symtbl, RelocData *reldt, uint32_t base_text_offset, uint32_t base_data_offset) {
   char buf[LINKER_BUF_SIZE];

    fgets(buf,LINKER_BUF_SIZE,input);
    int data_size=calc_data_size(input);
    reldt ->data_size=data_size;
    
    fgets(buf,LINKER_BUF_SIZE,input);
    int text_size=calc_text_size(input);
    reldt->text_size=text_size;

    fgets(buf,LINKER_BUF_SIZE,input);
    add_to_symbol_table(input,symtbl,base_text_offset,base_data_offset);

    fgets(buf,LINKER_BUF_SIZE,input);
    add_to_symbol_table(input,reldt->table,0,0);
    return 0;
}

/*
 * Detect whether the given instruction with offset OFFSET needs relocation.
 *
 * Return value:
 * 1 if the instruction needs relocation, 0 otherwise.
 *
 * Arguments:
 * offset: Address offset of the instruction in the current file.
 * reltbl: relocation table corresponding to the current file.
 */
int inst_needs_relocation(SymbolTable *reltbl, uint32_t offset) {
    uint32_t i;
    for(i=0;i<(reltbl->len);++i){
        if((((reltbl->tbl)+i)->addr)==offset){
            return 1;
        }
    }
    return 0;
}

int32_t my_relocate_instruction(uint32_t inst,uint32_t offset,SymbolTable *symtbl,SymbolTable *reltbl){
    char *syb_name=create_copy_of_str(get_symbol_for_addr(reltbl,offset));
    int name_len=strlen(syb_name);
    char *rel_data=strtok(syb_name,"@");
    int name_len2=strlen(rel_data);
    int64_t ins_addr=get_addr_for_symbol(symtbl,rel_data);
    if(name_len==name_len2){
        inst+=ins_addr/4;
    }
    else{
        rel_data=strtok(NULL,"@");
        if(strcmp(rel_data,"Hi")==0)
            inst+=(ins_addr>>16)&0xffff;
        else
            inst+=ins_addr&0xffff;
    }
    return inst;
}
