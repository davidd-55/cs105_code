#include <stdio.h>
#include <stdlib.h>

// Define a few types for better readability
typedef int physical_addr;
typedef int virtual_addr;
typedef char byte;

// An enum for defining our two VM strategies
enum strategies { RANDOM, LRU };

// Global page table
typedef struct page_table_entry {
  int frame_number; // physical frame number
  int valid;        // denotes validity of page
  // TODO: add member for LRU time usage marker
} page_table_entry_t;

// Global constants defining VM characteristics
// I recommend testing with (uncomment these three lines and comment
// the lines below).
//   const int PAGE_SIZE = 16;
//   const int PHYSICAL_MEMORY_SIZE = 64;
//   const int VIRTUAL_MEMORY_SIZE = 128;
const int PAGE_SIZE = 4096;              // 4 KB = 2^12 bytes
const int PHYSICAL_MEMORY_SIZE = 32768;  // 32 KB = 2^15 bytes
const int VIRTUAL_MEMORY_SIZE = 1048576; // 1 MB = 2^20 bytes

// Global simulation "constants" set in main
int INT_SIZE;
int INTS_PER_PAGE;
int NUM_FRAMES;
int NUM_PAGES;

// Global variables
page_table_entry_t *page_table;
byte *physical_memory;
long num_faults;
long num_accesses;
int strategy;

// TODO: add global marker for updating page LRU counters

// Initialize a "data" array with random numbers
int *gen_array(int n) {
  int *array = (int *)malloc(n * INT_SIZE);

  for (int i = 0; i < n; i++) {
    array[i] = rand() % 100000;
  }

  return array;
}

// Print states of the page table and virtual memory
// This function is useful for debugging (not as useful as gdb).
void print_simulation_state() {

  // Print page table
  printf("\nPage table: [");
  for (int i = 0; i < NUM_PAGES; i++) {
    page_table_entry_t *entry_ptr = page_table + i;

    printf(" %d(%d:%ld),", entry_ptr->frame_number, entry_ptr->valid,
           entry_ptr->lru_marker);
  }
  printf("]\n");

  // Print physical memory
  printf("num_frames = %d, ints_per_page= %d\n", NUM_FRAMES, INTS_PER_PAGE);
  printf("Physical memory: [");
  for (int i = 0; i < NUM_FRAMES; i++) {
    for (int j = 0; j < INTS_PER_PAGE; j++) {
      int *data_ptr = (int *)(physical_memory + i * PAGE_SIZE + j * INT_SIZE);
      printf(" %d,", *(data_ptr));
    }
  }
  printf("]\n\n");
}

// Initialize physical memory and the page table
// The initialization process handles pages stored in physical memory
//   as well as pages stored in files on the disk.
void initialize(int *data, int n) {

  int num_pages_in_use = (n * INT_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;

  // Initialize physical frames
  for (int page_num = 0; page_num < NUM_FRAMES; page_num++) {

    // Initialize physical frame from array of randomized data
    // This data belongs to page page_num
    for (int i = 0; i < INTS_PER_PAGE; i++) {
      int val = *(data + page_num * INTS_PER_PAGE + i);
      *((int *)(physical_memory + page_num * PAGE_SIZE + i * INT_SIZE)) = val;
    }

    page_table_entry_t *entry_ptr = page_table + page_num;

    // Initialize page table entry
    if (page_num < num_pages_in_use) {
      // Add valid page table entry
      entry_ptr->frame_number = page_num;
      entry_ptr->valid = 1;

      // TODO: initialize entry for LRU marker

      // TODO: update global LRU marker

    } else {
      // Add invalid page table entry
      entry_ptr->valid = 0;
    }
  }

  // Initialize paged-out virtual pages in files
  for (int page_num = NUM_FRAMES; page_num < NUM_PAGES; page_num++) {

    page_table_entry_t *entry_ptr = page_table + page_num;

    // Add table entry as initiallay invalid
    entry_ptr->valid = 0;

    if (page_num < num_pages_in_use) {
      // Page's filename
      char filename[42];
      sprintf(filename, "files/%030d.pg", page_num);

      // Grab pointer into data array
      byte *data_ptr = (byte *)(data + page_num * PAGE_SIZE);

      // Write virtual page to file
      FILE *page_fp = fopen(filename, "w");
      fwrite(data_ptr, PAGE_SIZE, 1, page_fp);
      fclose(page_fp);
    }
  }
}

// Convert a virtual address into a physical addres using the page table
physical_addr translate_addr(virtual_addr virt_addr) {
  // TODO #1: implement this function
  physical_addr phys_addr = NULL;

  return phys_addr;
}

// Simulate kernel code for handling page faults
void handle_page_fault(virtual_addr virt_addr) {

  int evicted_page_number;
  page_table_entry_t *evicted_entry_ptr = NULL;

  //
  // Step 1: Select page to evict
  //

  if (strategy == RANDOM) {

    // Loop until we find an invalid page
    do {
      evicted_page_number = rand() % NUM_PAGES;
      evicted_entry_ptr = page_table + evicted_page_number;
    } while (evicted_entry_ptr->valid == 0);

  } else if (strategy == LRU) {

    // TODO #2: implement this page eviction strategy

  } else {

    printf("INVALID PAGE REPLACEMENT STRATEGY\n");
    exit(1);
  }

  //
  // Step 2: Write evicted page to disk
  //

  // Grab pointer to data in physical memory
  int frame_number = evicted_entry_ptr->frame_number;
  byte *phys_memory_ptr = physical_memory + frame_number * PAGE_SIZE;

  // Create filename based on page number
  char filename[42];
  sprintf(filename, "files/%030d.pg", evicted_page_number);

  // Write evicted page to disk
  FILE *evicted_fp = fopen(filename, "w");
  fwrite(phys_memory_ptr, PAGE_SIZE, 1, evicted_fp);
  fclose(evicted_fp);

  // Mark evicted page as invalid
  evicted_entry_ptr->valid = 0;

  //
  // Step 3: Load page from file into physical memory
  //

  // Convert virtual address into a page number
  int loaded_page_number = virt_addr / PAGE_SIZE;

  // Compute the page entry from the page number
  page_table_entry_t *loaded_entry_ptr = page_table + loaded_page_number;

  // Read the page from disk into the evicted pages memory location
  sprintf(filename, "files/%030d.pg", loaded_page_number);
  FILE *loaded_fp = fopen(filename, "r");
  fread(phys_memory_ptr, PAGE_SIZE, 1, loaded_fp);
  fclose(loaded_fp);

  // Update page table
  loaded_entry_ptr->frame_number = frame_number;
  loaded_entry_ptr->valid = 1;
}

// Loads and store follow the same basic process
// 1. Compute address
// 2. Check for fault
// 3. Correct address if needed
// 4. Use address
int *access(virtual_addr virt_addr) {
  num_accesses++;

  physical_addr phys_addr = translate_addr(virt_addr);

  if (phys_addr == -1) {
    // Page fault
    num_faults++;

    handle_page_fault(virt_addr);
    phys_addr = translate_addr(virt_addr);
  }

  return (int *)(physical_memory + phys_addr);
}

// Load value from physical memory using virtual address
int load(virtual_addr virt_addr) {
  int *data_addr = access(virt_addr);
  return *data_addr;
}

// Store value in physical memory using virtual address
void store(virtual_addr virt_addr, int val) {
  int *data_addr = access(virt_addr);
  *data_addr = val;
}

// A helper function for testing and debugging
void print_array(int n) {
  for (int i = 0; i < n; i++) {
    virtual_addr virt_addr = INT_SIZE * i;
    int val = load(virt_addr);
    printf("%d, ", val);
  }
  printf("\n");
}

// Swap two values based on virtual addresses
void swap(int virt_addr1, int virt_addr2) {
  int temp = load(virt_addr1);
  store(virt_addr1, load(virt_addr2));
  store(virt_addr2, temp);
}

void selection_sort(int n) {
  int array_size = n * INT_SIZE;

  // Iterate through positions in the array
  for (int i = 0; i < array_size; i = i + INT_SIZE) {

    // Find int that belongs in that position
    int min_pos = i;

    for (int j = i; j < array_size; j = j + INT_SIZE) {
      if (load(j) < load(min_pos)) {
        min_pos = j;
      }
    }

    // swap that int into current position
    swap(i, min_pos);
  }
}

void insertion_sort(int n) {
  int array_size = n * INT_SIZE;

  // Iterate through positions in the array
  for (int i = 0; i < array_size; i += INT_SIZE) {

    // Swap int in that position into the right place
    int curr_pos = i;
    while (curr_pos > 0 && load(curr_pos - INT_SIZE) > load(curr_pos)) {
      swap(curr_pos - INT_SIZE, curr_pos);
      curr_pos -= INT_SIZE;
    }
  }
}

int parent(int i) { return (i - 1) / 2; }
int leftChild(int i) { return 2 * i + 1; }
int rightChild(int i) { return 2 * i + 2; }

int biggestChild(int i, int n) {
  int c1 = leftChild(i);
  int c2 = rightChild(i);
  if (c2 > n || load(c1 * INT_SIZE) > load(c2 * INT_SIZE)) {
    return c1;
  } else {
    return c2;
  }
}

void heapify(int n) {
  int i = 1;

  // While more elements to add to heap
  while (i < n) {
    int curr = i;
    int p = parent(curr);

    while (curr > 0 && load(curr * INT_SIZE) > load(p * INT_SIZE)) {
      swap(curr * INT_SIZE, p * INT_SIZE);
      curr = p;
      p = parent(curr);
    }

    i = i + 1;
  }
}

void heap_sort(int n) {
  heapify(n);

  n = n - 1;
  while (n > 0) {
    // Move root of heap to end
    swap(0, n * INT_SIZE);

    // Treat the beginning as a smaller heap and fix the heap
    n--;
    int curr = 0;
    int child = biggestChild(curr, n);

    // While curr has a bigger child, swap with bigger child
    while (child <= n && load(curr * INT_SIZE) < load(child * INT_SIZE)) {
      swap(curr * INT_SIZE, biggestChild(curr, n) * INT_SIZE);

      curr = child;
      child = biggestChild(curr, n);
    }
  }
}

int partition(int start, int end) {

  int v1 = load(start * INT_SIZE);
  int v2 = load(((end - start) / 2) * INT_SIZE);
  int v3 = load(end * INT_SIZE);

  int pivot;

  if ((v2 >= v1 && v1 >= v3) || (v2 <= v1 && v1 <= v3)) {
    pivot = v1;
  } else if ((v1 >= v2 && v2 >= v3) || (v1 <= v2 && v2 <= v3)) {
    pivot = v2;
    swap(start * INT_SIZE, (end - start) / 2 * INT_SIZE);
  } else {
    pivot = v3;
    swap(start * INT_SIZE, end * INT_SIZE);
  }

  int i = start;
  int j = end;
  while (i < j) {
    // Loop invariant: < i => < pivot, i = pivot, > t => > pivot
    int temp1 = load((i + 1) * INT_SIZE);
    int temp2 = load(i * INT_SIZE);

    if (load((i + 1) * INT_SIZE) <= load(i * INT_SIZE)) {
      swap((i + 1) * INT_SIZE, i * INT_SIZE); // move before pivot
      i++;
    } else {
      swap((i + 1) * INT_SIZE, j * INT_SIZE); // move after pivot
      j--;
    }
  }

  return i;
}

void quick_sort(int start, int end) {
  if (start < end) {
    int middle = partition(start, end);
    quick_sort(start, middle - 1);
    quick_sort(middle + 1, end);
  }
}

// Evaluate your virtual memory implementation
void evaluate() {

  // Generate array of random numbers
  int num_ints = VIRTUAL_MEMORY_SIZE / INT_SIZE;
  int *data = gen_array(num_ints);

  printf("       | ------------------------ Heap Sort ------------------------ "
         "| ----------------------- Quick Sort ------------------------\n"
         "       | ---------- Random ---------- | ------------ LRU ----------- "
         "| ---------- Random ---------- | ------------ LRU -----------\n"
         "   N   |   Miss  |  Access  |   Rate  |   Miss  |  Access  |   Rate  "
         "|   Miss  |  Access  |   Rate  |   Miss  |  Access  |   Rate \n"
         "-------|---------|----------|---------|---------|----------|---------"
         "|---------|----------|---------|---------|----------|--------\n");

  for (int len = 2; len <= num_ints; len *= 2) {

    strategy = RANDOM;
    initialize(data, len);
    num_faults = 0;
    num_accesses = 0;
    heap_sort(len);
    long heap_misses = num_faults;
    long heap_accesses = num_accesses;

    strategy = LRU;
    initialize(data, len);
    num_faults = 0;
    num_accesses = 0;
    heap_sort(len);
    long heap_misses_lru = num_faults;
    long heap_accesses_lru = num_accesses;

    strategy = RANDOM;
    initialize(data, len);
    num_faults = 0;
    num_accesses = 0;
    quick_sort(0, len - 1);
    long quick_misses = num_faults;
    long quick_accesses = num_accesses;

    strategy = LRU;
    initialize(data, len);
    num_faults = 0;
    num_accesses = 0;
    quick_sort(0, len - 1);
    long quick_misses_lru = num_faults;
    long quick_accesses_lru = num_accesses;

    printf("%6d | %7ld | %8ld | %.5f | %7ld | %8ld | %.5f | %7ld | %8ld | %.5f "
           "| %7ld | %8ld | %.5f\n",
           len, heap_misses, heap_accesses, (double)heap_misses / heap_accesses,
           heap_misses_lru, heap_accesses_lru,
           (double)heap_misses_lru / heap_accesses_lru, quick_misses,
           quick_accesses, (double)quick_misses / quick_accesses,
           quick_misses_lru, quick_accesses_lru,
           (double)quick_misses_lru / quick_accesses_lru);
  }

  free(data);
}

int main(int argc, char **argv) {

  // Initialize simulation constants
  INT_SIZE = (int)sizeof(int);
  INTS_PER_PAGE = PAGE_SIZE / INT_SIZE;
  NUM_FRAMES = PHYSICAL_MEMORY_SIZE / PAGE_SIZE;
  NUM_PAGES = VIRTUAL_MEMORY_SIZE / PAGE_SIZE;

  // Initialize simulation global variables
  int page_table_size = NUM_PAGES * sizeof(page_table_entry_t);
  page_table = (page_table_entry_t *)malloc(page_table_size);
  physical_memory = (byte *)malloc(PHYSICAL_MEMORY_SIZE);

  // TODO: initialize global LRU marker

  evaluate();

  free(page_table);
  free(physical_memory);
}
