#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "../block.h"
//#include "../constants.h"
#include "../inode.h"

inode_t* inode_head;
uint8_t* block_bitmap;
block_t* blocks;

/****************/
/** UNIT TESTS **/
/****************/
void get_block_test(void);
void add_entry_test(void);

int main()
{  
  
  //get_block_test();
  add_entry_test();
  return 0;
}

void add_entry_test(){
 
  //init blocks
  uint8_t block_bitmap_[NUM_BLOCKS/8];
  block_t blocks_[NUM_BLOCKS];
  block_initialize((char*) blocks_, block_bitmap_);
  //init inodes
  inode_t inode_head_[NUM_INODES];
  inode_initialize((char*) inode_head_);
   
  //
  const int num_max_inode_blocks = NUM_DIRECT_PTRS
                            + NUM_S_INDIRECT_PTRS * BLOCK_SIZE / 4
                            + NUM_D_INDIRECT_PTRS * BLOCK_SIZE * BLOCK_SIZE / 16;
                            //a total of 4234 blocks, including pointer blocks
   
  char buffer[FILENAME_SIZE];
  unsigned int i=0;
   
  /* CASE 1: No inode to allocate */
  // Occupy all the inodes
  for(i=1;i<NUM_INODES;i++){
    strcpy(inode_head[i].type,"dir");
  }
  // try adding a new directory entry
  assert(inode_add_entry(0,"dummy",0) == 0);
  // free allocated inodes
  for(i=1;i<NUM_INODES;i++){
    strcpy(inode_head[i].type,"");
  }
   
  /* CASE 2: No block to allocate */
  block_t* ptrs[NUM_BLOCKS];
  //allocate max number of blocks for single inode
  for(i = 0 ; i < NUM_BLOCKS; i++ ){
    ptrs[i] = block_get_free();
    //inode_head[0].size += BLOCK_SIZE;
  }
  // try adding a new directory entry, should fail and return 0
  assert(inode_add_entry(0,"dummy",0) == 0);
  //deallocate blocks
  for(i = 0 ; i < NUM_BLOCKS; i++ ){
    block_remove(ptrs[i]);
  }
   
  /* CASE 3: Adding new directory entries */
  for(i=0;i<NUM_INODES-1;i++){
    sprintf(buffer,"%d",i);
    inode_add_entry(0,buffer,0);
  }
  uint8_t block_offset;// = (i)/(BLOCK_SIZE); // index into the location table
  uint8_t in_block_offset;// = (parent_ptr->size % BLOCK_SIZE)/(sizeof(directory_entry_t)); // index into the block
 
  for(i=0;i<NUM_INODES-1;i++){
    sprintf(buffer,"%d",i);
    block_offset = i/(BLOCK_SIZE/sizeof(directory_entry_t));
    in_block_offset = i%(BLOCK_SIZE/sizeof(directory_entry_t));
    //printf("%s,%s -", buffer,inode_get_block(0,block_offset)->directories[in_block_offset].filename);
    assert(strcmp(buffer,inode_get_block(0,block_offset)->directories[in_block_offset].filename)==0);
  }
  
  /* CASE 4 */
  uint16_t eIndex;
  assert(inode_find_entry(0,"583",&eIndex)!=0); // 583 is an existing entry name
  assert(inode_find_entry(0,"dummy",&eIndex)==0); // dummy is not an existing entry name
  
  printf("inode_add_entry & inode_find_entry test passed.\n");
   
}

void get_block_test(){

  //init blocks
  uint8_t block_bitmap_[NUM_BLOCKS/8];
  block_t blocks_[NUM_BLOCKS];
  block_initialize((char*) blocks_, block_bitmap_);
  //init inodes
  inode_t inode_head_[NUM_INODES];
  inode_initialize((char*) inode_head_);
  
  // test inode_get_block
  const int num_max_inode_blocks = NUM_DIRECT_PTRS 
                            + NUM_S_INDIRECT_PTRS * BLOCK_SIZE / 4
                            + NUM_D_INDIRECT_PTRS * BLOCK_SIZE * BLOCK_SIZE / 16;
                            //a total of 4234 blocks, including pointer blocks
  block_t* ptrs[NUM_BLOCKS];
  //allocate max number of blocks for single inode
  int i, j;
  for(i = 0 ; i < num_max_inode_blocks; i++ ){
    ptrs[i] = inode_get_block(0, i);
    inode_head[0].size += BLOCK_SIZE;
    //check whether NULL
    assert(ptrs[i] != NULL);
    //check whether equals to any previous ptrs
    for(j = 0; j < i; j++){
      assert(ptrs[i] != ptrs[j]);
    }
  }
  //now read those blocks
  for(i = 0 ; i < num_max_inode_blocks; i++ ){
    assert(ptrs[i] == inode_get_block(0, i));
  }
  //check whether the first 4234 blocks are allocated
  for(i=0; i < 529; i++){
    assert(block_bitmap[i] == 255);
  }
  assert(block_bitmap[i] == 3);
  
  //reset
  block_initialize((char*)blocks_, block_bitmap_);
  inode_initialize((char*) inode_head_);
  //allocate all blocks
  for(i = 0 ; i < NUM_BLOCKS/8; i++ ){
    block_bitmap[i] = 255;
  }
  //try to get one more
  assert(inode_get_block(0, i) == NULL);
  
  printf("inode_get_block test passed.\n");
  
}
