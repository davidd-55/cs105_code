#
# Simple Makefile to build the programs
#
# Eleanor Birrell
# Pomona College
# September 8, 2020
#

CC = gcc

all: part1 part2 part3 part3asm

part1: part1.c
	$(CC) -g -o part1 part1.c

part2: part2.c
	$(CC) -g -o part2 part2.c

part3: part3.c
	$(CC) -g -o part3 part3.c

part3asm: part3.c
	$(CC) -g -O0 -o part3asm part3.c

clean:
	rm -rf part1 part2 part3 part3asm
	rm -rf *~
	rm -rf *.dSYM
