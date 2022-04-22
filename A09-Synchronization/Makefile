#
# Makefile for the CS 105 Synchronization Memory Homework, Spring 2020
#
# Eleanor Birrell, Anthony Clark
# Pomona College
# April 14, 2022
#

all: sync0 sync1 sync2 sync3

sync0: sync0_starter.c
	gcc -pthread -o sync0 sync0_starter.c

sync1: sync1_sync.c
	gcc -pthread -o sync1 sync1_sync.c

sync2: sync2_starve.c
	gcc -pthread -o sync2 sync2_starve.c

sync3: sync3_capacity.c
	gcc -pthread -o sync3 sync3_capacity.c

clean:
	rm -rf sync0 sync1 sync2 sync3
