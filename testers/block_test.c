#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "../block.h"

uint8_t* block_bitmap;
block_t* blocks;

int main()
{
  //init blocks
  uint8_t block_bitmap_[NUM_BLOCKS/8];
  block_t blocks_[NUM_BLOCKS];
  block_initialize((char*) blocks_, block_bitmap_);
  int i;
  //check whether all are free
  for(i=0;i<NUM_BLOCKS/8;i++){
    assert(block_bitmap[i] == 0);
  }
  //request all
  for(i=0;i<NUM_BLOCKS;i++){
    assert(block_get_free() == &blocks[i]);
  }
  //check whether all are occuppied
  for(i=0;i<NUM_BLOCKS/8;i++){
    assert(block_bitmap[i] == 255);
  }
  //free all
  for(i=0;i<NUM_BLOCKS;i++){
    block_remove(&blocks[i]) ;
  }
  //check whether all are free
  for(i=0;i<NUM_BLOCKS/8;i++){
    assert(block_bitmap[i] == 0);
  }
  //fill first half
  for(i=0;i<NUM_BLOCKS/16;i++){
    block_bitmap[i] = (char) 0xFF;
  }
  //request new
  assert(block_get_free() == &blocks[NUM_BLOCKS/2]);
  printf("block test passed.\n");
  
  return 0;
}
