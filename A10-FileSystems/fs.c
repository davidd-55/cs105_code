/*
A10 by Emily Tomz and David D'Attile
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// We're often dealing with bytes, so lets have a nice typedef as a reminder
typedef char byte;

// A reference to a block is an int
typedef int block_index;

// An inode number is an int
typedef int inode_number;

// Parameters of the fake file system (not all are used here,
// some are only used when creating the fake file blocks).
#define BLOCK_SIZE 256
#define NUM_BLOCKS 1024
#define NUM_INODES 128
#define NUM_DIRECT_PTRS 8

// Block can contain inodes, directory entires, references to blocks, or data
#define INODES_PER_BLOCK (BLOCK_SIZE / sizeof(inode_t))
#define ENTRIES_PER_BLOCK (BLOCK_SIZE / sizeof(directory_entry_t))
#define REFS_PER_BLOCK (BLOCK_SIZE / sizeof(block_index))

#define MAX_FILEPATH_LEN 40
#define MAX_FILENAME_LEN 16
#define FIRST_INODE_BLOCK 3

// Representation of an inode in our fake file system.
typedef struct inode_params {
  char filename[MAX_FILENAME_LEN];
  long filesize;
  block_index direct_ptrs[NUM_DIRECT_PTRS];
  block_index indirect_ptr;
  block_index doubly_indirect_ptr;
} inode_t;

// Representation of directory entries in our fake file system.
typedef struct directory_entry {
  char name[MAX_FILENAME_LEN];
  int inumber;
  byte padding[12];
} directory_entry_t;

//  _____     _          _____ _ _        ____            _
// |  ___|_ _| | _____  |  ___(_) | ___  / ___| _   _ ___| |_ ___ _ __ ___
// | |_ / _` | |/ / _ \ | |_  | | |/ _ \ \___ \| | | / __| __/ _ \ '_ ` _ \
// |  _| (_| |   <  __/ |  _| | | |  __/  ___) | |_| \__ \ ||  __/ | | | | |
// |_|  \__,_|_|\_\___| |_|   |_|_|\___| |____/ \__, |___/\__\___|_| |_| |_|
//                                              |___/
//  ____  _     _       ___                       _   _
// |  _ \(_)___| | __  / _ \ _ __   ___ _ __ __ _| |_(_) ___  _ __  ___
// | | | | / __| |/ / | | | | '_ \ / _ \ '__/ _` | __| |/ _ \| '_ \/ __|
// | |_| | \__ \   <  | |_| | |_) |  __/ | | (_| | |_| | (_) | | | \__ \
// |____/|_|___/_|\_\  \___/| .__/ \___|_|  \__,_|\__|_|\___/|_| |_|___/
//                         |_|

// For the sake of simplicify, I only show the read operations.
// I removed all of the write operations used to create the block files
// found in ./disk

// Constructs the native filename for a data block stored on disk
void block_filename(block_index block_idx, int include_path,
                    char *filename_out) {
  if (include_path) {
    sprintf(filename_out, "disk/%x.bloc", block_idx);
  } else {
    sprintf(filename_out, "%x.bloc", block_idx);
  }
}

// Reads a data block into memory, given the block number
int read_block(block_index block_idx, byte *dest) {
  char filename[MAX_FILEPATH_LEN];
  block_filename(block_idx, 1, filename); // gets and set filename for the given block

  FILE *fp = fopen(filename, "rb"); // try to open the file, file pointer fp

  // If none, failed to open
  if (!fp) {
    fprintf(stderr, "fopen() failed to open '%s' in read_block\n", filename);
    return 1;
  }

  // Loops through each byte in given block
  for (int i = 0; i < BLOCK_SIZE; i++) {
    int byte = fgetc(fp); // gets an int from 0 to 255, representing an unsigned char
    dest[i] = byte; // puts the int in the destination location char pointer
  }

  fclose(fp);

  return 0;
}

// Loads an inode into memory, given an inode number
void read_inode(inode_number inumber, inode_t *dest) {
  // The inode number is offset by 3 to account for
  // 0: superblock
  // 1: inode bitmap
  // 2: data block bitmap
  block_index block_idx = FIRST_INODE_BLOCK + inumber / INODES_PER_BLOCK;
  int offset = inumber - (inumber / INODES_PER_BLOCK * INODES_PER_BLOCK);
  int offset_in_bytes = offset * sizeof(inode_t);

  byte block_data[BLOCK_SIZE];

  read_block(block_idx, block_data);
  memcpy(dest, block_data + offset_in_bytes, sizeof(inode_t));
}

//  _____     _          _____ _ _        ____            _
// |  ___|_ _| | _____  |  ___(_) | ___  / ___| _   _ ___| |_ ___ _ __ ___
// | |_ / _` | |/ / _ \ | |_  | | |/ _ \ \___ \| | | / __| __/ _ \ '_ ` _ \
// |  _| (_| |   <  __/ |  _| | | |  __/  ___) | |_| \__ \ ||  __/ | | | | |
// |_|  \__,_|_|\_\___| |_|   |_|_|\___| |____/ \__, |___/\__\___|_| |_| |_|
//                                              |___/
//  ____                      _
// / ___|  ___  __ _ _ __ ___| |__
// \___ \ / _ \/ _` | '__/ __| '_ \
//  ___) |  __/ (_| | | | (__| | | |
// |____/ \___|\__,_|_|  \___|_| |_|
//
//   ___                       _   _
//  / _ \ _ __   ___ _ __ __ _| |_(_) ___  _ __  ___
// | | | | '_ \ / _ \ '__/ _` | __| |/ _ \| '_ \/ __|
// | |_| | |_) |  __/ | | (_| | |_| | (_) | | | \__ \
//  \___/| .__/ \___|_|  \__,_|\__|_|\___/|_| |_|___/
//       |_|
//

// Given a filepath, returns an array of names in that filepath
// Arg 2 (len) should be the length of the string path
// Arg 3 should be a location for storing the number of files in the path
// The return argument (dirs) is a bunch of pointers into path
char **parse_path(char *path, size_t len, int *num_dirs) {
  // absolute filepaths only
  // assert(len > 0);
  // assert(path[0] == '/');
  // assert(path[len] == '\0');

  *num_dirs = 0;

  // Count number of directories in file path (no normalizing)
  for (int i = 0; i < len; i += 1) {
    // Each '/' indicates a subdirectory
    if (path[i] == '/') {
      *num_dirs += 1;
    }
  }

  // Allocate entries for each word, plus an extra one for null termination
  char **dirs = (char **)malloc(sizeof(char *) * (*num_dirs + 1));

  // Start current_dir at the beginning of path
  char *current_dir = path;

  // And current dir_index to 0
  int dir_index = 0;

  for (int i = 0; i <= len; i += 1) {
    // We've found a directory name of the end of the input string
    if (path[i] == '/' || path[i] == '\0') {
      // Change the path by replacing a '/' with a null terminator
      path[i] = '\0';

      // The current directory should point at the start of the directory name
      dirs[dir_index] = current_dir;

      dir_index += 1;

      // Move current_dir forward by the number of characters in the directory
      // name
      current_dir = path + i + 1;
    }
  }

  return dirs;
}

// Given the block number of a data block containing directory entries,
// searches through entries for the filename.
// Returns the inum associated with that file if found, else -1
int search_dir_data_block(block_index data_block_idx, char *filename,
                          int max_entries) {
  // TODO(Task 1a): search for the corresponding inode number

  // Read the DB file
  directory_entry_t dest[ENTRIES_PER_BLOCK];
  read_block(data_block_idx, (byte*)dest);

  // Searh the first max_entries # of directory entries in the block
  for(int i = 0; i < ENTRIES_PER_BLOCK; i++){
    // if hit max, return -1
    if (i == max_entries){
      return -1;
    }

    // Get entryname
    char * entryname = (dest + i)->name;

    // Check if filenames are the same
    if (strcmp(entryname, filename) == 0){
      // Return inum associated with the found file
      return (dest + i)->inumber;
    }
  }

  // Returns -1 if haven't found one
  return -1;
}

// Given the block number of an indirect file block, searches the data blocks
// pointed to by that indirect block for an entry matching filename.
// Returns the inum associated with that file if found, else -1
int search_dir_indirect(block_index indirect_block_idx, char *filename,
                        int max_entries) {
  // TODO(Task 4): search through indirect file blocks
  // Uses search_dir_data_block to examine each indirect entry
  block_index dest[REFS_PER_BLOCK];
  read_block(indirect_block_idx, (byte*)dest);

  // Loop through each indirect pointer
  for (int i = 0; i < REFS_PER_BLOCK; i++){
    // get block index
    block_index curr_block_index = dest[i];

    // search for inum
    int inum_result = search_dir_data_block(curr_block_index, filename, max_entries);

    // return inum for block if found
    if (inum_result != -1) {
      return inum_result;
    }
  }

  // inode number not found
  return -1;
}

// Given the block number of a doubly-indirect file block, searches the data
// blocks pointed to by that indirect block for an entry matching filename.
// Returns the inum associated with that file if found, else -1
int search_dir_doubly_indirect(block_index doubly_indirect_block_num,
                               char *filename, int max_entries) {
  // TODO(Task 4): ...
  block_index dest[REFS_PER_BLOCK];
  read_block(doubly_indirect_block_num, (byte*)dest);

  // Loop through each doubly indirect pointer
  for (int i = 0; i < REFS_PER_BLOCK; i++){
    // get block index
    block_index curr_block_index = dest[i];

    // search for inums of internal indirect pointers (nested loop since calling search_dir_indirect)
    int inum_result = search_dir_indirect(curr_block_index, filename, max_entries);

    // return inum for block if found
    if (inum_result != -1) {
      return inum_result;
    }
  }

  // inode number not found
  return -1;
}

// Given the inode number of a directory, returns the inode number of a matching
// file in that found in that directory
// Returns -1 if file not found.
int search_dir(inode_number dir_inum, char *filename) {
  int file_inum = -1;
  // TODO(Task 1b): implement assuming that no indirect pointers are used

  // Read the directory's inode
  inode_t dest;
  read_inode(dir_inum, &dest);

  // Get the number for total entries (each entry pointer is an int, which is 4 bytes)
  int entries_left = (dest.filesize) / 4;
  int block_entries = BLOCK_SIZE/(sizeof(directory_entry_t)); 

  // Loop through the directory and search for a data entry that matches filename
  for (int i = 0; i < NUM_DIRECT_PTRS; i ++){
    block_index data_block_idx = (dest.direct_ptrs)[i];
    entries_left -= block_entries;

    file_inum = search_dir_data_block(data_block_idx, filename, entries_left);
    
    // if we found the directory, break loop
    if (file_inum != -1){
      return file_inum;
    }
  }

  // TODO(Task 4): extend this to support the general case
  // indirect search only if block entries remain
  if (entries_left > 0) {
    // inum not found in direct case; it's -1, so run indirect search
    file_inum = search_dir_indirect(dest.indirect_ptr, filename, entries_left);

    // Decriment entries_left by all the indirect pointer blocks
    entries_left -= block_entries * REFS_PER_BLOCK;
  }

  // If still not found; still -1 and block entries remaining, run doubly indirect search
  if (file_inum == -1 && entries_left > 0) {
    file_inum = search_dir_doubly_indirect(dest.doubly_indirect_ptr, filename, entries_left);
  }

  return file_inum; // either -1 or the inum for found file
}

// Given an filepath, return the corresponding inode or -1 if the file
// is not found
// Note: the initial implementation only searches for FILES in the root
// directory
int search_path(char *filepath) {

  // num_dirs is a value set by parse_path
  int num_dirs;
  char **dirs = parse_path(filepath, strlen(filepath), &num_dirs);

  // TODO(Task 3): walk through path to find inode for file
  // For each subdirectory run search_dir
  for (int i = 2; i <= num_dirs + 2; i++) {
    // Search dirs
    int val = search_dir(i, dirs[num_dirs]);
    if (val != -1){
      return val;
    }
  }

  return -1;
}

//  ____            _   _
// |  _ \ ___  _ __| |_(_)_ __   __ _
// | |_) / _ \| '__| __| | '_ \ / _` |
// |  __/ (_) | |  | |_| | | | | (_| |
// |_|   \___/|_|   \__|_|_| |_|\__, |
//                              |___/
//   ___                       _   _
//  / _ \ _ __   ___ _ __ __ _| |_(_) ___  _ __  ___
// | | | | '_ \ / _ \ '__/ _` | __| |/ _ \| '_ \/ __|
// | |_| | |_) |  __/ | | (_| | |_| | (_) | | | \__ \
//  \___/| .__/ \___|_|  \__,_|\__|_|\___/|_| |_|___/
//       |_|
//

// Given a filepath, port the file from the fake file system to the real
// file system by saving the bytes using fopen, fputc, and fclose.
void port_fake_to_real(char *fake_filepath, char *real_filename) {
  // TODO(Task 2): implement this assuming that the filepath is of the form
  //          /filename.ext (i.e., that the file you are looking for is
  //          in the root directory and that all files/directories are small
  //          enough to fit in one data_block

  // Open a real file
  FILE *fp = fopen(real_filename, "wb");

  // Grab an inode number from search_path for fake file
  int inode_number = search_path(fake_filepath);
  
  // Read inode
  inode_t dest; 
  read_inode(inode_number, &dest);

  // get total filesize; use as counter for file read remaining
  long fsize_remaining = dest.filesize;

  // Loop through direct pointers, call read_block and store data in new file
  for (int i = 0; i < NUM_DIRECT_PTRS; i ++){
    // get the data block index
    block_index data_block_idx = (dest.direct_ptrs)[i];

    // allocate space for data from block on heap and read block into mem
    byte block_data_array[BLOCK_SIZE];
    read_block(data_block_idx, block_data_array);

    // for each byte in block, write to file
    for (int i = 0; i < BLOCK_SIZE && i < fsize_remaining; i++) {
      byte data_block_byte = block_data_array[i];
      fputc(data_block_byte, fp);
    }

    // decrement filesize by block size amount
    fsize_remaining -= BLOCK_SIZE;
  }

  // TODO(Task 5): extend this to support the general case
  // begin indirect data read if file was not stored completely in direct pointer data blocks
  if (fsize_remaining > 0) {
    // read array of block_indexes from indirect ptr block
    block_index dest_indirect[REFS_PER_BLOCK];
    read_block(dest.indirect_ptr, (byte*)dest_indirect);

    // loop through indirect ptrs
    for (int i = 0; i < REFS_PER_BLOCK; i++) {
      // isolate curr data block from indirect ptr
      block_index curr_indirect_index = dest_indirect[i];

      // read in data from isolated block
      byte indirect_block_data_array[BLOCK_SIZE];
      read_block(curr_indirect_index, indirect_block_data_array);

      for (int i = 0; i < BLOCK_SIZE && i < fsize_remaining; i++) {
        byte indirect_data_block_byte = indirect_block_data_array[i];
        fputc(indirect_data_block_byte, fp);
      }

      // decrement fsize by block size after each indirect block read
      fsize_remaining -= BLOCK_SIZE;
    }
  }

  // begin dbly indirect data read if file was not stored completely in direct pointer.indirect pointer data blocks
  if (fsize_remaining > 0) {
    // read array of block_indexes from dbly indirect ptr block
    block_index dest_dbly_indirect[REFS_PER_BLOCK];
    read_block(dest.doubly_indirect_ptr, (byte*)dest_dbly_indirect);

    // loop through dbly indirect indexes
    for (int i = 0; i < REFS_PER_BLOCK; i++) {
      // isolate current dbly indirect index
      block_index curr_dbly_indirect_index = dest_dbly_indirect[i];

      // get current indirect ptrs from curr dbly indirect block index
      block_index curr_indirect_block_ptrs[REFS_PER_BLOCK];
      read_block(curr_dbly_indirect_index, (byte*)curr_indirect_block_ptrs);

      // loop through indirect ptrs
      for (int i = 0; i < REFS_PER_BLOCK; i++) {
        // isolate curr data block from indirect ptr
        block_index curr_indirect_index = curr_indirect_block_ptrs[i];

        // read in data from isolated block
        byte indirect_block_data_array[BLOCK_SIZE];
        read_block(curr_indirect_index, indirect_block_data_array);

        for (int i = 0; i < BLOCK_SIZE && i < fsize_remaining; i++) {
          byte indirect_data_block_byte = indirect_block_data_array[i];
          fputc(indirect_data_block_byte, fp);
        }

        // decrement fsize by block size after each indirect block read
        fsize_remaining -= BLOCK_SIZE;
      }
    }
  }

  fclose(fp);
}

//  _____         _
// |_   _|__  ___| |_ ___
//   | |/ _ \/ __| __/ __|
//   | |  __/\__ \ |_\__ \
//   |_|\___||___/\__|___/
//

#define MAX_TEST_FILEPATH_LEN 128

int main(int argc, char **argv) {

  char fake_file_path[MAX_TEST_FILEPATH_LEN];

  // Each test-file test takes the following format
  // 1. Copy a filename into fake_file_path
  // 2. Check if we can get the file inode number (tests 1, 3, and 5)
  // 3. Check if we can read the file (tests 2, 4, and 6)

  // test case for Task 1: should return 3
  strncpy(fake_file_path, "/test.txt", MAX_TEST_FILEPATH_LEN);
  int test1 = search_path(fake_file_path);
  printf("inumber for /test.txt: %d\n", test1);

  // test case for Task 2: should result in readable text file
  strncpy(fake_file_path, "/test.txt", MAX_TEST_FILEPATH_LEN);
  port_fake_to_real(fake_file_path, "test1.txt");

  // test case for Task 3: should return 5
  strncpy(fake_file_path, "/example_dir/test2.txt", MAX_TEST_FILEPATH_LEN);
  int test2 = search_path(fake_file_path);
  printf("inumber for /example_dir/test2.txt: %d\n", test2);

  // test case for Task 3: should result in readable text file
  strncpy(fake_file_path, "/example_dir/test2.txt", MAX_TEST_FILEPATH_LEN);
  port_fake_to_real(fake_file_path, "test2.txt");

  // test case for Task 5: should return 6 (should work after task #3)
  strncpy(fake_file_path, "/example_dir/image.jpg", MAX_TEST_FILEPATH_LEN);
  int test3 = search_path(fake_file_path);
  printf("inumber for /example_dir/image.jpg: %d\n", test3);

  // test case for Task 5: should result in valid jpg file
  strncpy(fake_file_path, "/example_dir/image.jpg", MAX_TEST_FILEPATH_LEN);
  port_fake_to_real(fake_file_path, "test3.jpg");

  return 0;
}