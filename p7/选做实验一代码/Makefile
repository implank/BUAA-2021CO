CC = gcc
CFLAGS = -g -std=c99 -Wall

ASSEMBLER_FILES =  my_assembler_utils.c
LINKER_FILES =  my_linker_utils.c

all: assembler linker

assembler:  $(ASSEMBLER_FILES)
	$(CC) $(CFLAGS) -o assembler assembler.c $(ASSEMBLER_FILES) libP7.a

linker:  $(LINKER_FILES)
	$(CC) $(CFLAGS) -o linker linker.c $(LINKER_FILES) $(ASSEMBLER_FILES) libP7.a

clean:
	rm *.o linker assembler
