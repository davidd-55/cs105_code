#
# Simple Makefile to build the programs
#
# Eleanor Birrell and Anthony Clark
# Pomona College
# 2022
#
CC = gcc

all:
	echo "no default action; specify a program"

part2: part2-tester.c
	$(CC) -g -o part2-tester part2-tester.c

part3: part3.c
	$(CC) -o part3 part3.c

clean:
	rm -rf part2-tester part3
	rm -rf *~
	rm -rf *.dSYM
