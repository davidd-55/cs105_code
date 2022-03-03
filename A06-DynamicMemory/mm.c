/*
 * mm.c, a basic implicit list implementation
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

int mm_check(void);

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Real Student Athletes",
    /* First member's full name */
    "David D'Attile",
    /* First member's email address */
    "dada2018@mymail.pomona.edu",
    /* Second member's full name (leave blank if none) */
    "Roei Cohen",
    /* Second member's email address (leave blank if none) */
    "rcac2017@mymail.pomona.edu"};

#define ALIGNMENT 8
#define MIN_PAYLOAD 16
#define ALLOCATED_MASK 1
#define SIZE_MASK (~ALLOCATED_MASK)

#define WORD_SIZE 8
#define CHUNK_SIZE (1 << 12)

char *base;
char *top;
char *search; // where the previous search stopped

/*
 * Internally, all pointers are of type char*.
 *
 * These functions are helpers for navigating block structure.
 * mostly for type casts to extract values from fields.
 *
 * They are functions rather than macros to get more secure
 * semantics and better readability. They will be inlined by -O2.
 */

// given pointer to payload, return pointer to start of block
char *block_pointer(void *p) { return (char *)p - WORD_SIZE; }

// given pointer to block, return pointer to payload
char *payload_pointer(char *p) { return p + WORD_SIZE; }

// given pointer to block, return pointer to header
size_t *header_pointer(char *p) { return (size_t *)(p); }

// given pointer to block, return pointer to footer
size_t *footer_pointer(char *p, size_t size) {
  return (size_t *)(p + size - WORD_SIZE);
}

// given pointer to block, return pointer to footer of prev block
char *prev_footer_pointer(char *p) { return (char *)(p - WORD_SIZE); }

// given pointer to block, return allocation status
int is_allocated(char *p) { return *(header_pointer(p)) & ALLOCATED_MASK; }

// given pointer to block, return size of block
size_t block_size(char *p) { return *(header_pointer(p)) & SIZE_MASK; }

// TODO: remove
char *block_pointer_from_footer (char *p) {
 //	size_t *footer_pointer = header_pointer(p);
 //	return p - (*footer_pointer & SIZE_MASK) + WORD_SIZE; 
 return p - block_size(p) + WORD_SIZE;
 } 

// given pointer to block, return size of payload
size_t payload_size(size_t blocksize) { return blocksize - 2 * WORD_SIZE; }

// given size of payload, round up to multiple of alignment
size_t roundup(size_t size) {
  return (size + ALIGNMENT - 1) & (~(ALIGNMENT - 1));
}

// given size of payload, return total size of block
size_t adjusted_size(size_t size) {
  return 2 * WORD_SIZE + ((size < MIN_PAYLOAD) ? MIN_PAYLOAD : roundup(size));
}

// set block header and footer
void mark_block(char *p, size_t size, int allocated) {
  size_t tag = size | allocated;
  *(header_pointer(p)) = tag;
  *(footer_pointer(p, size)) = tag;
}

// initialize the heap
int mm_init(void) {
  // initialize heap with 1 block of size CHUNK_SIZE
  // (plus wiggle room for fixing the alignment
  base = (char *)mem_sbrk(CHUNK_SIZE + WORD_SIZE);
  if (base == (char *)(-1)) {
    return -1;
  }

  // define correctly-aligned base and top of heap
  base = (char *)(roundup((size_t)base));
  top = base + CHUNK_SIZE;

  // initialize the one block in heap as size=CHUNCK_SIZE, currently free
  mark_block(base, CHUNK_SIZE, 0);

  // initialize search pointer to free block
  search = base;

  return 0;
}

// find the first block of size >= totalsize
// return top if no suchblock exists
inline char *first_fit(size_t totalsize) {

  // search from beginning of heap to top
  char *p = base;
  while (p < top && (is_allocated(p) || block_size(p) < totalsize)) {
    p += block_size(p);
  }

  // if no available block, return top
  if (p >= top) {
    p = top;
  }

  return p;
}

// find the next block of size >= totalsize
// return top if no such block exists
inline char *next_fit(size_t totalsize) {
  // TODO #2: implement next_fit (14 lines of code in Prof Birrell's solution)
  // save pre-run search ptr
  char *p = search;

  // search from prev search position to top
  while (p < top && (is_allocated(p) || block_size(p) < totalsize)) {
    // get new potential search pointer
	  p += block_size(p);
  }

  // didn't find space between prev search position and top
  if (p >= top) {
    // reset p to base
    p = base;

	  // search from base to prev search position
    while (p < search && (is_allocated(p) || block_size(p) < totalsize)) {
      p += block_size(p);
    }

	  // if we've moved through whole heap, reset ptr to top
    if (p >= search) {
      p = top;
    }
  }
 
  // reset search to ptr and return
  search = p;
  return search;
}

// allocate memory
void *mm_malloc(size_t size) {
  size_t totalsize = adjusted_size(size);
  size_t blocksize;
  size_t excess;
  char *p;

  if (size == 0) {
    return NULL;
  }

  // search for a free block that fits this size
  //p = first_fit(totalsize); // TODO #2: switch to next_fit
  p = next_fit(totalsize);

  // if no available block, extend heap
  if (p == top) {
    // avoid making lots of small blocks
    if (totalsize < 256) { // value 256 was determined experimentally
      excess = 256;
      blocksize = totalsize + excess;
    } else {
      excess = 0;
      blocksize = totalsize;
    }

    // extend heap by blocksize
    mem_sbrk(blocksize);
    top += blocksize;
  } else { // compute excess space in selected block
    blocksize = block_size(p);
    excess = block_size(p) - totalsize;
  }

  // if excess isn't big enough, don't bother splitting
  if (excess <= 2 * WORD_SIZE + MIN_PAYLOAD) {
    totalsize = blocksize;
    mark_block(p, totalsize, 1);
  } else { // split block
    mark_block(p, totalsize, 1);
    mark_block(p + totalsize, excess, 0);
  }

  return payload_pointer(p);
}

// free memory
void mm_free(void *p) {
  //current block pointers
  char *bp = block_pointer(p);
  size_t size = block_size(bp);

  // coalesce
  // TODO #1: implement coalescing (11 lines of code in Prof Birrell's solution)

  //next and prev block pointers
  char *next_bp = bp + size;
  char *prev_bp = prev_footer_pointer(bp) - block_size(prev_footer_pointer(bp)) + WORD_SIZE; // find header of previous block

  char *new_head = bp;
  size_t new_size = size;

  // if next block (above) is free, keep same head ptr and increase size
  if (next_bp < top && !is_allocated(next_bp)) {
  	new_size += block_size(next_bp);

    // move search if it pointed to next block that will be coalesced
  	if (search == next_bp) {
  	  search = bp;
  	}
  }

  // if the prev block (below) is free, move header to prev block ptr and increase size
  if (prev_bp >= base && !is_allocated(prev_bp)) {
	new_head = prev_bp;
  	new_size += block_size(prev_bp);

    // move search if it pointed to the current block that will be coalesced
  	if (search == bp) {
  	  search = prev_bp;
  	}
  }

  mark_block(new_head, new_size, 0);

  /*
  if (next_bp < top && prev_bp > base && !is_allocated(next_bp) && !is_allocated(prev_bp)) { // adjacent blocks are free, set header of previous block and footer of next block

	// reset search if it was located at the current or next block pointer
    if (search == bp || search == next_bp) {
      search = prev_bp;
    }
    
 	mark_block(prev_bp, block_size(prev_bp) + size + block_size(next_bp), 0);
  } else if (next_bp < top && !is_allocated(next_bp)) { // next block is free, set header of current block and footer of next block

	// reset search if it was located at the next block pointer
	if (search == next_bp) {
	  search = bp;
	}
	
	mark_block(bp, size + block_size(next_bp), 0);
  } else if (prev_bp > base && !is_allocated(prev_bp)) { // previous block is free, set header of previous block and footer of current block

	// reset search if it was located at the current block pointer
    if (search == bp) {
      search = prev_bp;
    }

  	mark_block(prev_bp, block_size(prev_bp) + size, 0);
  } else {  
  	mark_block(bp, size, 0)	;
  }
  */
}

// ignore --- included for compatibility with test suite
void *mm_realloc(void *p, size_t newsize) { return NULL; }

// helper function for checking the heap
int mm_check(void) {
  int result = 0;
  int prev_is_free = 0;
  size_t blocksize;

  // loop through all the blocks in the heap
  char *p = base;
  while (p < top) {
    // check for contiguously allocated blocks
    if (prev_is_free && is_allocated(p)) {
      printf("contiguous allocated blocks!\n");
      result = -1;
    }

    // check that the tags match
    blocksize = block_size(p);
    if (*(size_t *)p != *(size_t *)(p + blocksize - WORD_SIZE)) {
      printf("tags differ!\n");
      result = -1;
    }
    p += blocksize;
  }

  // make sure heap ends at a block boundary
  if (p != top) {
    printf("missed the top!\n");
    result = -1;
  }

  return result;
}
