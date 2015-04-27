#include "block.h"

#ifdef DEBUG
#include <stdbool.h>
#include <string.h>
#else
#include <linux/types.h>
#endif

void block_initialize(char* blocks_ptr, uint8_t* bitmap_ptr){
  blocks = (block_t*)blocks_ptr;
  block_bitmap = bitmap_ptr;
  uint16_t i;
  for(i=0;i<NUM_BLOCKS;i++){
    block_clear_bitmap(i);
  }
}

block_t* block_get_free(){
  uint16_t i;
  for(i=0;i<NUM_BLOCKS;i++){
    if(block_is_free(i)){
      block_set_bitmap(i);
      return block_index_to_addr(i);
    }
  }
  return NULL;
}

void block_remove(block_t* ptr){
  block_clear_bitmap(block_addr_to_index(ptr));
}

void block_set_bitmap(uint16_t block_index){
  block_bitmap[block_index/8] |= 1 << (block_index%8);
}

void block_clear_bitmap(uint16_t block_index){
  block_bitmap[block_index/8] &= ~(1 << (block_index%8));
}

bool block_is_free(uint16_t block_index){
  return ((block_bitmap[block_index/8] & (1<<(block_index%8))) == 0);
}

uint16_t block_addr_to_index(block_t* addr){
  return ((uint32_t) addr - (uint32_t) blocks) / BLOCK_SIZE;
}
  
block_t* block_index_to_addr(uint16_t block_index){
  return &blocks[block_index];
}
