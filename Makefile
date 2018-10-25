CC=/opt/amiga/bin/m68k-amigaos-gcc
CFLAGS=-I.

AmiDiff: Diff.o
	$(CC) -o AmiDiff_gcc Diff.o 