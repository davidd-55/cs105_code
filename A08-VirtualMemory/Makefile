#
# Makefile for the CS 105 Virtual Memory Homework
#
# Eleanor Birrell, Anthony Clark
# Pomona College
# April 8, 2022
#

all: vm

vm: vm.c
	rm -rf files
	mkdir files
	gcc -o vm $<

clean:
	rm -rf files
	rm -rf vm

