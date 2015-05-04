#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../file_operations.h"
#include "../file_desc_table.h"

inode_t* inode_head;
uint8_t* block_bitmap;
block_t* blocks;

int reg1, dir1, dir1_dir1, dir1_reg1;

void fresh_setup(){
  block_initialize((char*) blocks, block_bitmap);
  inode_initialize((char*) inode_head);
}

int test_check_path(void){
  fresh_setup();
  
  //add test files & directories
  dir1 = inode_add_entry(0, "dir1", 0);
  reg1 = inode_add_entry(0, "reg1", 1);
  dir1_dir1 = inode_add_entry(dir1, "dir1_dir1", 0);
  dir1_reg1 = inode_add_entry(dir1, "dir1_reg1", 1);
  
  int parent;
  char leaf_name[FILENAME_SIZE];
  assert(rd_check_path("no_root", &parent, leaf_name) == -1);
  assert(rd_check_path("/", &parent, leaf_name) == 0);
  assert(rd_check_path("/dir1", &parent, leaf_name) == dir1);
  assert(parent == 0);
  assert(rd_check_path("/dir1/dir1_dir1", &parent, leaf_name) == dir1_dir1);
  assert(parent == dir1);
  assert(strcmp(leaf_name, "dir1_dir1") == 0);
  assert(rd_check_path("/dir1/dir1_reg1", &parent, leaf_name) == dir1_reg1);
  assert(parent == dir1);
  assert(strcmp(leaf_name, "dir1_reg1") == 0);
  assert(rd_check_path("/dir2/dir1_reg1", &parent, leaf_name) == -1);
  assert(parent == -1);
  assert(strcmp(leaf_name, "dir1_reg1") == 0);
  assert(rd_check_path("/dir1/dir1_reg2", &parent, leaf_name) == -1);
  assert(parent == dir1);
  assert(strcmp(leaf_name, "dir1_reg2") == 0);
  
  printf("rd_check_path test passed.\n");
}

void test_file_operations(){
  
  // Create a sample tree of directories and files
  assert(0==rd_mkdir("/sys"));
  assert(0==rd_mkdir("/etc"));
  assert(0==rd_mkdir("/sys/class"));
  assert(0==rd_mkdir("/sys/class/devices"));
  assert(0==rd_creat("/sys/class/devices/f1"));
  assert(0==rd_creat("/sys/class/devices/f2"));
  assert(0==rd_creat("/sys/class/devices/f3"));
  assert(0==rd_creat("/sys/class/devices/f4"));
  assert(0==rd_creat("/sys/class/devices/f5"));
  
  /*** TEST 1: Open a file ***/
  // Tests: rd_open
  int fp1 = rd_open("/sys/class/devices/f1");
  assert(fp1!=-1);
  
  /*** TEST 2: Write to a file till its size reaches to maximum ***/
  // Tests: rd_read, rd_write, rd_lseek
   const int max_size = BLOCKSIZE*(NUM_DIRECT_PTRS 
                            + NUM_S_INDIRECT_PTRS * BLOCKSIZE / 4
                            + NUM_D_INDIRECT_PTRS * BLOCKSIZE * BLOCKSIZE / 16);
  char write_data='d';
  char read_data='a';
  char readbuffer[900];
  uint32_t i;

  // fill in the file with characters
  for(i=0;i<max_size;i++){
    assert(rd_write(fp1, &write_data, 1)==1);
  }

  // Set file pointer to point to the beginning of the file
  assert(0==rd_lseek(fp1,0));
  
  // now read back the written data
  for(i=0;i<max_size;i++){
    assert(1==rd_read(fp1, &read_data, 1));
    assert(read_data==write_data);
  }
  
  /*** TEST 3: Creation of existing files and directories ***/
  // Tests: rd_mkdir, rd_creat
  assert(-1==rd_creat("/sys/class/devices/f3"));
  assert(-1==rd_mkdir("/sys/class/devices"));
  assert(-1==rd_mkdir("/etc"));
    
  /*** TEST 4: Accessing a closed and re-opened file ***/
  // Tests: rd_close, rd_read, rd_write, rd_lseek
  assert(0==rd_lseek(fp1,0));
  assert(rd_close(fp1)==0);
  assert(rd_read(fp1,&read_data,1)==-1);  // read/write should fail as the file was closed
  assert(rd_write(fp1,&read_data,1)==-1);
  fp1 = rd_open("/sys/class/devices/f1"); // re-open the file
  assert(rd_read(fp1,&read_data,1)==1);   // read/write should now go through without error
  assert(rd_write(fp1,&read_data,1)==1);
  assert(rd_close(fp1)==0);
  
  /*** TEST 5: Removing a file from the file system ***/
  // Tests: rd_unlink, rd_open 
  assert(-1==rd_unlink("/sys/class"));
  assert(-1==rd_unlink("/nosuchfile"));
  assert(0==rd_unlink("/sys/class/devices/f5"));
  int fp4 = rd_open("/sys/class/devices/f5");
  assert(fp4==-1);                              // open should fail as file is removed
  assert(0==rd_creat("/sys/class/devices/f5")); // create the file again
  fp4 = rd_open("/sys/class/devices/f5");
  assert(fp4!=-1);                              // open should work now
  assert(0==rd_close(fp4));                     // close the file
  
  /*** TEST 6: Getting a list of subdirectories ***/
  // Tests: rd_readdir, rd_open, rd_close
  int f_root = rd_open("/");
  assert(f_root!=-1);

  char buffer[16]; 
  uint16_t a;
  // Testing wuth the subdirectories of the root folder
  assert(rd_readdir(f_root, buffer)==1); // reading sys folder (name null terminated)
  assert(strcmp("sys", buffer)==0);     // compare filename
  assert((int)buffer[14]==inode_find_entry(0,"sys", &a)); //compare inode index ASSUMING SINGLE DIGIT INDEX
  
  assert(rd_readdir(f_root, buffer)==1); // reading etc folder (name null terminated)
  assert(strcmp("etc", buffer)==0);      // compare filename
  assert((int)buffer[14]==inode_find_entry(0,"etc", &a)); //compare inode index ASSUMING SINGLE DIGIT INDEX
  
  assert(rd_readdir(f_root, buffer)==0); // should return 0 since reached end of entries
  assert(rd_close(f_root)==0);
  
  // testing with the subdirectories of the devices folder
  int dev_fp = rd_open("/sys/class/devices/");
  assert(dev_fp!=-1);
  
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f1", buffer)==0);        // compare filename
  assert((int)buffer[14]==inode_find_entry(file_get_fd((uint16_t)dev_fp)->inode, "f1", &a));
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f2", buffer)==0);        // compare filename
  assert((int)buffer[14]==inode_find_entry(file_get_fd((uint16_t)dev_fp)->inode, "f2", &a));
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f3", buffer)==0);        // compare filename
  assert((int)buffer[14]==inode_find_entry(file_get_fd((uint16_t)dev_fp)->inode, "f3", &a));
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f4", buffer)==0);        // compare filename
  assert((int)buffer[14]==inode_find_entry(file_get_fd((uint16_t)dev_fp)->inode, "f4", &a));
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f5", buffer)==0);        // compare filename
  assert((int)buffer[14]==inode_find_entry(file_get_fd((uint16_t)dev_fp)->inode, "f5", &a));
  assert(rd_readdir(dev_fp, buffer)==0); // should return 0 since reached end of entries
  assert(rd_close(dev_fp)==0);
  
  printf("File operations test passed.\n");
}

int main(){
  //init
  uint8_t block_bitmap_[NUM_BLOCKS/8];
  block_bitmap = block_bitmap_;
  block_t blocks_[NUM_BLOCKS];
  blocks = blocks_;
  inode_t inode_head_[NUM_INODES];
  inode_head = inode_head_;
  fresh_setup();
  
  test_file_operations();
  
  test_check_path();
  return 0;
}

