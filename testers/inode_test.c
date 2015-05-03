#include <assert.h>
#include <stdint.h>
#include <stdio.h>

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

int main()
{  
  
  get_block_test();
  return 0;
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
