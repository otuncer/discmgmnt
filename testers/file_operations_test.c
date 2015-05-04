#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../file_operations.h"

inode_t* inode_head;
uint8_t* block_bitmap;
block_t* blocks;

int reg1, dir1, dir1_dir1, dir1_reg1;

void fresh_setup(){
  block_initialize((char*) blocks, block_bitmap);
  inode_initialize((char*) inode_head);
  //init root
  strcpy(inode_head->type, "dir");
  inode_head->size = 0;
  //add test files & directories
  dir1 = inode_add_entry(0, "dir1", 0);
  reg1 = inode_add_entry(0, "reg1", 1);
  dir1_dir1 = inode_add_entry(dir1, "dir1_dir1", 0);
  dir1_reg1 = inode_add_entry(dir1, "dir1_reg1", 1);
}

int test_check_path(void){
  fresh_setup();
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

int main(){
  //init
  uint8_t block_bitmap_[NUM_BLOCKS/8];
  block_bitmap = block_bitmap_;
  block_t blocks_[NUM_BLOCKS];
  blocks = blocks_;
  inode_t inode_head_[NUM_INODES];
  inode_head = inode_head_;
  fresh_setup();
  
  
  
  test_check_path();
  return 0;
}

