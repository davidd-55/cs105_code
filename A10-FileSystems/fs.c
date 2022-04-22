#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parameters of the fake file system (not all are used here,
// some are only used when creating the fake file blocks).
#define BLOCK_SIZE 256
#define NUM_BLOCKS 1024
#define NUM_INODES 128

int first_inode_block = 3;
int first_data_block = 35;

// Representation of an inode in our fake file system.
typedef struct inode_params {
  char filename[16];
  long filesize;
  int direct_ptrs[8];
  int indirect_ptr;
  int doubly_indirect_ptr;
} inode_t;

// Representation of directory entries in our fake file system.
typedef struct directory_entry {
  char name[16];
  int inumber;
  char padding[12];
} directory_entry_t;

// Constructs the native filename for a data block stored on disk
char *block_filename(int block_num, int include_path) {
  int max_filename_len = 32;
  int max_path_len = max_filename_len + 8;
  char *filename = (char *)malloc(max_path_len);
  if (include_path) {
    sprintf(filename, "disk/%x.bloc", block_num);
  } else {
    sprintf(filename, "%x.bloc", block_num);
  }

  return filename;
}

// Writes a block to disk, given the block number
int write_block(int block_num, char *source) {
  char *filename = block_filename(block_num, 1);

  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "fopen() failed to open file %s in write_block\n",
            filename);
    return 1;
  }

  for (int i = 0; i < BLOCK_SIZE; i++) {
    fputc(*(source + i), fp);
  }

  fclose(fp);
  free(filename);
  return 0;
}

// Reads a data block into memory, given the block number
int read_block(int block_num, char *dest) {
  char *filename = block_filename(block_num, 1);

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "fopen() failed to open file %s in read_block\n", filename);
    return 1;
  }

  for (int i = 0; i < BLOCK_SIZE; i++) {
    int byte = fgetc(fp);
    *(dest + i) = (char)byte;
  }

  fclose(fp);
  free(filename);
  return 0;
}

// Initializes a data block of zero bytes
int initialize_block(int block_num) {
  char *zeros = (char *)malloc(BLOCK_SIZE);
  memset(zeros, 0, BLOCK_SIZE);

  char *filename = block_filename(block_num, 1);
  write_block(block_num, zeros);

  free(filename);
  free(zeros);
  return 0;
}

// Writes an inode to disk
int write_inode(int inumber, inode_t inode) {
  if (inumber >= NUM_INODES) {
    fprintf(stderr, "Can't write inode %d: NUM_INODES=%d\n", inumber,
            NUM_INODES);
    return -1;
  }
  int inodes_per_block = BLOCK_SIZE / sizeof(inode_t);
  int block_num = 3 + inumber / inodes_per_block;
  int offset = inumber - (inumber / inodes_per_block * inodes_per_block);

  inode_t *block_data = (inode_t *)malloc(BLOCK_SIZE);

  int ret = read_block(block_num, (char *)block_data);
  if (ret == -1) {
    return ret;
  }
  memcpy(block_data + offset, &inode, sizeof(inode));
  ret = write_block(block_num, (char *)block_data);
  if (ret == -1) {
    return ret;
  }

  free(block_data);
  return 0;
}

// Loads an inode into memory, given an inode number
int read_inode(int inumber, inode_t *dest) {
  int inodes_per_block = BLOCK_SIZE / sizeof(inode_t);
  int block_num = 3 + inumber / inodes_per_block;
  int offset = inumber - (inumber / inodes_per_block * inodes_per_block);

  inode_t *block_data = (inode_t *)malloc(BLOCK_SIZE);

  read_block(block_num, (char *)block_data);
  memcpy(dest, block_data + offset, sizeof(inode_t));

  free(block_data);
  return 0;
}

// Initializes contents of an inode in memory
void initialize_inode(char *filename, inode_t *inode) {
  strncpy(inode->filename, filename, 16);
  inode->filesize = 0;

  for (int i = 0; i < 8; i++) {
    inode->direct_ptrs[i] = 0;
  }
  inode->indirect_ptr = 0;
  inode->doubly_indirect_ptr = 0;
}

// Given a filepath, returns an array of files in that filepath
// Arg 2 (len) should be the length of the string path
// Arg 3 should be a location for storing the number of files in the path
char **parse_path(char *path, size_t len, int *num_dirs) {
  // absolute filepaths only
  // assert(len > 0);
  // assert(path[0] == '/');
  // assert(path[len] == '\0');

  // count number of directories in file path (no normalizing)
  *num_dirs = 0;
  for (int i = 0; i < len; i += 1) {
    if (path[i] == '/') {
      *num_dirs += 1;
    }
  }

  // allocate entries for each word, plus an extra one for NULL termination
  char **dirs = (char **)malloc(sizeof(char *) * (*num_dirs + 1));

  char *current_dir = path;
  int dir_index = 0;
  for (int i = 0; i <= len; i += 1) {
    if (path[i] == '/' || path[i] == '\0') {
      // save the current directory, null terminated
      path[i] = '\0';
      dirs[dir_index] = current_dir;

      dir_index += 1;
      current_dir = path + i + 1;
    }
  }

  return dirs;
}

// Given the block number of a data block containing directory entries,
// searches through entries for the filename.
// Returns the inum associated with that file if found, else -1
int search_dir_data_block(int data_block_num, char *filename, int max_entries) {
  // TODO(Task 1a): search for the corresponding inode number
  return -1;
}

// Given the block number of an indirect file block, searches the data blocks
// pointed to by that indirect block for an entry matching filename.
// Returns the inum associated with that file if found, else -1
int search_dir_indirect(int indirect_block_num, char *filename,
                        int max_entries) {
  // TODO(Task 4): search through indirect file blocks
  return -1;
}

// Given the block number of a doubly-indirect file block, searches the data
// blocks pointed to by that indirect block for an entry matching filename.
// Returns the inum associated with that file if found, else -1
int search_dir_doubly_indirect(int doubly_indirect_block_num, char *filename,
                               int max_entries) {
  // TODO(Task 4): ...
  return -1;
}

// Given inumber of a directory, returns the inumber of a file in that dir
// Returns -1 if file not found.
int search_dir(int dir_inum, char *filename) {
  int file_inum = -1;

  // TODO(Task 1b): implement assuming that no indirect pointers are used

  // TODO(Task 4): extend this to support the general case

  return file_inum;
}

// Given an filepath, return the corresponding inode or -1 if the file
// is not found
// Note: the initial implementation only searches for FILES in the root
// directory
int search_path(char *filepath) {
  int num_dirs;
  char **dirs = parse_path(filepath, strlen(filepath), &num_dirs);
  return search_dir(2, dirs[1]);

  // TODO(Task 3): walk through path to find inode for file
}

// Given a filepath, port the file from the fake file system to the real
// file system by saving the bytes using fopen, fputc, and fclose.
void port_fake_to_real(char *fake_filepath, char *real_filename) {
  // TODO(Task 2): implement this assuming that the filepath is of the form
  //          /filename.ext (i.e., that the file you are looking for is
  //          in the root directory and that all files/directories are small
  //          enough to fit in one data_block

  // TODO(Task 5): extend this to support the general case
}

int main(int argc, char **argv) {

  char fake_file_path[128];

  // Each test-file test takes the following format
  // 1. Copy a filename into fake_file_path
  // 2. Check if we can get the file inode number (tests 1, 3, and 5)
  // 3. Check if we can read the file (tests 2, 4, and 6)

  // test case for Task 1: should return 3
  strncpy(fake_file_path, "/test.txt", 128);
  int test1 = search_path(fake_file_path);
  printf("inumber for /test.txt: %d\n", test1);

  // test case for Task 2: should result in readable text file
  strncpy(fake_file_path, "/test.txt", 128);
  port_fake_to_real(fake_file_path, "test1.txt");

  // test case for Task 3: should return 5
  strncpy(fake_file_path, "/example_dir/test2.txt", 128);
  int test2 = search_path(fake_file_path);
  printf("inumber for /example_dir/test2.txt: %d\n", test2);

  // test case for Task 3: should result in readable text file
  strncpy(fake_file_path, "/example_dir/test2.txt", 128);
  port_fake_to_real(fake_file_path, "test2.txt");

  // test case for Task 5: should return 6 (should work after task #3)
  strncpy(fake_file_path, "/example_dir/image.jpg", 128);
  int test3 = search_path(fake_file_path);
  printf("inumber for /example_dir/image.jpg: %d\n", test3);

  // test case for Task 5: should result in valid jpg file
  strncpy(fake_file_path, "/example_dir/image.jpg", 128);
  port_fake_to_real(fake_file_path, "test3.jpg");

  return 0;
}
