/*****************************************************************
 * BUAA Fall 2021 Fundamentals of Computer Hardware
 * Project7 Assembler and Linker
 *****************************************************************
 * assembler.c
 * Top module to perform assembler
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler-src/assembler_utils.h"
#include "lib/translate_utils.h"
#include "lib/translate.h"
#include "my_assembler_utils.h"

/* First pass of the assembler. You should implement pass_two() first.
 *
 * This function should read each line, strip all comments, scan for labels,
 * and pass instructions to write_pass_one(). The input file may or may not
 * be valid. Here are some guidelines:
 *  **FOR .text**
 *  1. Only one label may be present per line. It must be the first token present.
 *      Once you see a label, regardless of whether it is a valid label or not,
 *      treat the NEXT token as the beginning of an instruction.
 *  2. If the first token is not a label, treat it as the name of an instruction.
 *  3. Everything after the instruction name should be treated as arguments to
 *      that instruction. If there are more than MAX_ARGS arguments, call
 *      raise_extra_arg_error() and pass in the first extra argument. Do not
 *      write that instruction to memory.
 *  4. Only one instruction should be present per line. You do not need to do
 *      anything else to detect this - it should be handled by guideline 3.
 *  5. A line containing only a label is valid. The address of the label should
 *      be the byte offset of the next instruction, regardless of whether there
 *      is a next instruction or not.
 *
 *  **FOR .data**
 *  1. Also only one label may be present per line.
 *  2. .space is the only valid directive used in data segment.
 *  3. The symbols are defined **strictly** in the following format:
 *      "label_name: .space byte(s)"
 *
 * Symbol definitions may appear in the .data segment or in the .text segment,
 * so in the function, the function read_data_segment() is called to handle symbols
 * in the .data segment, and in pass_one() to handle symbols defined in the .text segment
 *
 * Just like in pass_two(), if the function encounters an error it should NOT
 * exit, but process the entire file and return -1. If no errors were encountered,
 * it should return 0.
 */
static int pass_one(FILE *input, FILE *output, SymbolTable *symtbl) {
    if (!input) {
        printf("input file is not valid.");
        return -1;
    }
    int data_sz;
    if ((data_sz = read_data_segment(input, symtbl)) == -1) {
        printf(".data section is not valid.");
        return -1;
    }
    fprintf(output, ".data\n%d\n\n", data_sz);
    char buf[ASSEMBLER_BUF_SIZE];
    uint32_t input_line = 0, byte_offset = 0;
    int ret_code = 0;
    fgets(buf, ASSEMBLER_BUF_SIZE, input);
    if (strcmp(strtok(buf, ASSEMBLER_IGNORE_CHARS), ".text") != 0) {
        ret_code = -1;
    }
    fprintf(output, ".text\n");
    while (fgets(buf, ASSEMBLER_BUF_SIZE, input)) {
        input_line++;

        skip_comment(buf);

        char *name = NULL;
        char *token = strtok(buf, ASSEMBLER_IGNORE_CHARS);

        if (token == NULL) {
            continue;
        }

        int ret = add_if_label(input_line, token, byte_offset, symtbl);
        if (ret == -1) {
            ret_code = -1;
            printf("error adding label to symbol table.\n");
            name = strtok(NULL, ASSEMBLER_IGNORE_CHARS);
        } else if (ret == 1) {
            name = strtok(NULL, ASSEMBLER_IGNORE_CHARS);
        } else if (ret == 0) {
            name = token;
        }

        if (name == NULL) {
            continue;
        }

        char *args[MAX_ARGS];
        int num_args = 0;
        if (parse_args(input_line, args, &num_args) == -1) {
            ret_code = -1;
            continue;
        }
        unsigned int lines_written = write_pass_one(output, name, args, num_args);
        if (lines_written == 0) {
            raise_inst_error(input_line, name, args, num_args);
            ret_code = -1;
        }
        byte_offset += lines_written * 4;
    }
    return ret_code;
}

/* Reads an intermediate file and translates it into machine code. You may assume:
    1. The input file contains no comments
    2. The input file contains no labels
    3. The input file contains at maximum one instruction per line
    4. All instructions have at maximum MAX_ARGS arguments
    5. The symbol table has been filled out already

   If an error is reached, DO NOT EXIT the function. Keep translating the rest of
   the document, and at the end, return -1. Return 0 if no errors were encountered.
   */
static int pass_two(FILE *input, FILE *output, SymbolTable *symtbl, SymbolTable *reltbl) {
    char buf[ASSEMBLER_BUF_SIZE];
    uint32_t input_line = 0;
    uint32_t byte_offset = 0;
    int ret_code = 0;

    while (fgets(buf, ASSEMBLER_BUF_SIZE, input)) {
        char *name = strtok(buf, ASSEMBLER_IGNORE_CHARS);
        if (name == NULL) {
            fprintf(output, "\n");
        } else if (strcmp(name, ".text")==0) {
            fprintf(output, ".text\n");
            break;
        } else {
            fprintf(output, "%s\n", buf);
        }
    }

    while (fgets(buf, ASSEMBLER_BUF_SIZE, input)) {
        input_line++;

        char *name = strtok(buf, ASSEMBLER_IGNORE_CHARS);

        if (!name) {
            continue;
        }

        /* Parse for instruction arguments.
         * Extra arguments should be filtered out in pass_one()
         */
        char *args[MAX_ARGS];
        int num_args = 0;
        if (parse_args(input_line, args, &num_args) == -1) {
            ret_code = -1;
        }


        /* Use translate_inst() to translate the instruction and write to output file.*/
        if (translate_inst(output, name, args, num_args, byte_offset, symtbl, reltbl) == -1) {
            ret_code = -1;
            raise_inst_error(input_line, name, args, num_args);
        } else {
            byte_offset += 4;
        }
    }

    return ret_code;
}

static int open_files(FILE **input, FILE **output, const char *input_name,
                      const char *output_name) {

    *input = fopen(input_name, "r");
    if (!*input) {
        printf("Error: unable to open input file: %s\n", input_name);
        return -1;
    }
    *output = fopen(output_name, "w");
    if (!*output) {
        printf("Error: unable to open output file: %s\n", output_name);
        fclose(*input);
        return -1;
    }
    return 0;
}

static void close_files(FILE *input, FILE *output) {
    fclose(input);
    fclose(output);
}

/* Runs the two-pass assembler. Most of the actual work is done in pass_one()
 * and pass_two().
 */
static int assemble(const char *in_name, const char *tmp_name, const char *out_name) {
    FILE *src, *dst;
    int err = 0;
    SymbolTable *symtbl = create_table(SYMTBL_UNIQUE_NAME);
    SymbolTable *reltbl = create_table(SYMTBL_NON_UNIQUE);
    if (in_name) {
        printf("Running pass one: %s -> %s\n", in_name, tmp_name);
        if (open_files(&src, &dst, in_name, tmp_name) != 0) {
            free_table(symtbl);
            free_table(reltbl);
            exit(1);
        }

        if (pass_one(src, dst, symtbl) != 0) {
            err = 1;
        }
        close_files(src, dst);
    }

    if (out_name) {
        printf("Running pass two: %s -> %s\n", tmp_name, out_name);
        if (open_files(&src, &dst, tmp_name, out_name) != 0) {
            free_table(symtbl);
            free_table(reltbl);
            exit(1);
        }

        if (pass_two(src, dst, symtbl, reltbl) != 0) {
            err = 1;
        }
        fprintf(dst, "\n.symbol\n");
        write_table(symtbl, dst);
        fprintf(dst, "\n.relocation\n");
        write_table(reltbl, dst);

        close_files(src, dst);
    }
    free_table(symtbl);
    free_table(reltbl);
    return err;
}

static void print_usage_and_exit(char *argv0) {
    printf("Usage:\n");
    printf("  Run both passes: %s <input file> <intermediate file> <output file>\n", argv0);
    printf("  Run pass #1:     %s -p1 <input file> <intermediate file>\n", argv0);
    printf("  Run pass #2:     %s -p2 <intermediate file> <output file>\n", argv0);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        print_usage_and_exit(argv[0]);
    }

    int mode = 0;
    if (strcmp(argv[1], "-p1") == 0) {
        mode = 1;
    } else if (strcmp(argv[1], "-p2") == 0) {
        mode = 2;
    }

    char *input, *inter, *output;
    if (mode == 1) {
        input = argv[2];
        inter = argv[3];
        output = NULL;
    } else if (mode == 2) {
        input = NULL;
        inter = argv[2];
        output = argv[3];
    } else {
        input = argv[1];
        inter = argv[2];
        output = argv[3];
    }

    int err = assemble(input, inter, output);

    return err;
}
