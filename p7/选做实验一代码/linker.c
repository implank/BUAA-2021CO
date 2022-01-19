/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * linker.c
 * Top module to perform linker
 *****************************************************************/

#include "linker-src/linker_utils.h"
#include "lib/translate_utils.h"
#include "my_linker_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#define BASE_ADDR ((uint32_t)0x00400000)
#define BASE_DATA ((uint32_t)0x10010000)

/*
 * Find the start of the .text section of the input file by reading each line
 * and check to see if we find ".text"
 *
 * Return value:
 * return 0 on success and -1 on failure.
 */
static int find_text_section(FILE *input) {
    char buf[LINKER_BUF_SIZE];
    while (fgets(buf, LINKER_BUF_SIZE, input)) {
        char *token = strtok(buf, LINKER_IGNORE_CHARS);
        if (token != NULL && strcmp(token, ".text") == 0) {
            return 0;
        }
    }
    return -1;
}

/*
 * Main function in pass two.
 * Write the contents of the .text section of the input file to the output file.
 *
 * Return value:
 * return 0 if no error, -1 otherwise.
 *
 * Arguments:
 * input:  the input file pointer.
 * output: the output file pointer.
 * symtbl: the symbol table which you can pass into relocate_inst() if needed.
 * reltbl: the relocation table, which you can also pass into relocate_inst() if needed.
 */
static int write_machine_code(FILE *input, FILE *output, SymbolTable *symtbl, SymbolTable *reltbl) {
    if (find_text_section(input) < 0) {
        printf("write_machine_code: cannot find .text section!\n");
        return -1;
    }
    char buf[LINKER_BUF_SIZE];
    uint32_t offset = 0;
    while (fgets(buf, LINKER_BUF_SIZE, input)) {
        char *token = strtok(buf, LINKER_IGNORE_CHARS);
        if (!token) {
            break;
        }

        int pos = 0;
        while (token[pos] != '\0') {
            if (isdigit(token[pos]) || (token[pos] >= 'a' && token[pos] <= 'f')) {
                pos += 1;
            } else {
                return -1;
            }
        }

        int32_t inst;
        sscanf(token, "%" PRIx32, &inst);
        if (inst_needs_relocation(reltbl, offset)) {
            int32_t res = my_relocate_instruction(inst, offset, symtbl, reltbl);
            if (res == -1) {
                printf("write_machine_code: relocate instruction %08"
                PRIx32
                " failed!\n", inst);
                return -1;
            }
            inst = res;
        }
        write_inst_hex(output, inst);

        offset += 4;
    }
    return 0;
}

/*
 * Given an array of file names, open each file. If a file cannot be opened,
 * return -1.
 *
 * Arguments:
 * input_name: an array of file names.
 * arr_len:    length of the array.
 * files:      file pointers to these provided files.
 */
static int open_files(char **input_name, int arr_len, FILE **files) {
    files = (FILE **) memset(files, 0, sizeof(FILE * ) * arr_len);
    for (int i = 0; i < arr_len; ++i) {
        FILE *fl = fopen(input_name[i], "r");
        if (!fl) {
            printf("open_files: unable to open input file: %s\n", input_name[i]);
            return -1;
        }
        files[i] = fl;
    }
    return 0;
}

/*
 * Given an array of file pointers, close each file.
 */
static void close_files(FILE **files, int arr_len) {
    for (int i = 0; i < arr_len; ++i) {
        if (files[i]) {
            fclose(files[i]);
        }
    }
}

/*
 * Main function in pass one
 * Build global symbol table, and relocation data for each file.
 *
 * Return value:
 * return 0 on success and -1 on failure.
 *
 * Arguments:
 * input_name: an array of input file names.
 * arr_len:    the length of input file name array.
 * symtbl:     the symbol table.
 * reldt:      an array of relocation data struct.
 */
static int build_table(char **input_name, int arr_len, uint32_t g_text_offset, uint32_t g_data_offset,
                       SymbolTable *symtbl, RelocData *reldt) {
    FILE **files = (FILE **) malloc(sizeof(FILE * ) * arr_len);
    if (open_files(input_name, arr_len, files) < 0) {
        close_files(files, arr_len);
        free(files);
        return -1;
    }

    for (int i = 0; i < arr_len; ++i) {
        fill_data(files[i], symtbl, reldt + i, g_text_offset, g_data_offset);
        g_text_offset += (reldt + i)->text_size;
        g_data_offset += (reldt + i)->data_size;
    }

    close_files(files, arr_len);
    free(files);
    return 0;
}

/*
 * Create a global symbol table and an array of relocation data whose length is
 * based on arr_len.
 * Notice that the symbol table has the SYMTBL_UNIQUE_NAME mode, and symbol table
 * in relocation data has the SYMTBL_NON_UNIQUE mode. Same as in assembler!
 *
 * Return value:
 * 0 on success. If allocation failed the linker will exit within the function
 * create_table() with code 1.
 *
 * Arguments:
 * symtbl:  the SymbolTable pointer to the allocated struct.
 * reldt:   the RelocData array pointer to the allocated struct array.
 * arr_len: the length of the RelocData array. (A file corresponds to a structure).
 */
static int create_tables(SymbolTable **symtbl, RelocData **reldt, int arr_len) {
    *symtbl = create_table(SYMTBL_UNIQUE_NAME);
    *reldt = (RelocData *) malloc(sizeof(RelocData) * arr_len);
    for (int i = 0; i < arr_len; ++i) {
        (*reldt)[i].table = create_table(SYMTBL_NON_UNIQUE);
    }
    return 0;
}

static int free_tables(SymbolTable *symtbl, RelocData *reldt, int arr_len) {
    free_table(symtbl);
    for (int i = 0; i < arr_len; ++i) {
        free_table(reldt[i].table);
    }
    free(reldt);
    return 0;
}

/*
 * The function performs the pass one.
 * Build SymbolTable and RelocData, which will be used in pass two.
 *
 * BASE_ADDR is the start address of the instruction (the default value is 0x0040_0000)
 * and BASE_DATA is the start address of the data segment (the default value is 0x1001_0000).
 * Then simply invokes the function build_table() to perform the actual table building.
 *
 * Return value:
 * return 0 on success, -1 on fail.
 *
 * Arguments:
 * input_name: an array of input file names.
 * arr_len:    the length of input file name array.
 * symtbl:     pointer to the SymbolTable struct.
 * reldt:      an **ARRAY** of RelocData struct.
 */
static int pass_one(char **input_name, int arr_len, SymbolTable *symtbl, RelocData *reldt) {
    if (build_table(input_name, arr_len, BASE_ADDR, BASE_DATA, symtbl, reldt) < 0) {
        return -1;
    }
    return 0;
}

/*
 * The function performs the pass two.
 * Generate the final machine code based on the symbol table and relocation data
 * built in pass one.
 *
 * It invokes function open_files() to open each input file, and fopen() to open output file.
 * Then, for each input file, invoke write_machine_code which relocates and writes final machine
 * code to the output file.
 *
 * Return value:
 * return 0 on success and -1 on failure.
 *
 * Arguments:
 * input_name:  an array of input file names.
 * output_name: output file name.
 * arr_len:     the length of input file name array.
 * symtbl:      pointer to the SymbolTable struct (generated in pass one).
 * reldt:       an **ARRAY** of RelocData struct (generated in pass one).
 */
static int pass_two(char **input_name, char *output_name, int arr_len, SymbolTable *symtbl, RelocData *reldt) {
    FILE **input_files = (FILE **) malloc(sizeof(FILE * ) * arr_len);
    open_files(input_name, arr_len, input_files);
    FILE *output_file = fopen(output_name, "w");
    if (!output_file) {
        close_files(input_files, arr_len);
        free(input_files);
        printf("unable to open output file: %s\n", output_name);
        return -1;
    }

    for (int i = 0; i < arr_len; ++i) {
        if (write_machine_code(input_files[i], output_file, symtbl, reldt[i].table) < 0) {
            close_files(input_files, arr_len);
            fclose(output_file);
            free(input_files);
            printf("write_machine_code error!\n");
            return -1;
        }
    }
    close_files(input_files, arr_len);
    fclose(output_file);
    free(input_files);

    return 0;
}

/*
 * Our linker first opens each input file, reads, and stores the symbol and relocation
 * data (found in the .symbol and .relocation section of each object file). During
 * this pass, it also computes the size of each text segment so that the final addresses
 * of each symbol is known. The function fill_data() performs this operation on a
 * single file. The function build_table() calls fill_data() on each file
 *
 * At this step, the symbol table contains the **absolute address** of every symbol
 * as well as the **relative byte offsets** of each item that needs relocation.
 * The linker then copies the contents of each .text segment into the same executable.
 * However, certain instructions need relocation. The inst_need_relocation() function
 * checks whether an instruction needs relocation. If so, the relocate_inst() function
 * will perform the relocation.
 *
 * Return value:
 * return 0 on success and -1 on failure.
 *
 * Arguments:
 * input_name:  an array of input file names.
 * output_name: output file name.
 * arr_len:     the length of input file name array.
 */
static int linker(char **input_name, char *output_name, int arr_len) {
    SymbolTable *symtbl = NULL;
    RelocData *reldt = NULL;
    create_tables(&symtbl, &reldt, arr_len);

    int res;

    res = pass_one(input_name, arr_len, symtbl, reldt);
    if (res < 0) {
        printf("pass one, build tables failed!\n");
        return -1;
    }

    res = pass_two(input_name, output_name, arr_len, symtbl, reldt);
    if (res < 0) {
        printf("pass two, write machine code failed!\n");
        return -1;
    }

    free_tables(symtbl, reldt, arr_len);
    return 0;
}

static void print_usage_and_exit(char *argv0) {
    printf("Usage: %s <input file1> <input file2> ... <output file>\n", argv0);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        print_usage_and_exit(argv[0]);
    }

    int arr_len = argc - 2;             // number of input files
    char *output_name = argv[argc - 1]; // output file name
    char **input_name = &argv[1];       // input file name array

    int res = linker(input_name, output_name, arr_len);
    if (res < 0) {
        printf("Linking failed!\n");
        exit(1);
    }
    printf("Linking completed successfully!\n");

    return 0;
}
