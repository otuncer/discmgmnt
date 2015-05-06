#include "block.h"

#ifdef DEBUG
#include <stdbool.h>
#include <string.h>
#else
#include <linux/types.h>
#endif

uint8_t* block_bitmap;
block_t* blocks;
super_block_t* super_block;

static inline void block_set_bitmap(uint16_t block_index){
  block_bitmap[block_index/8] |= 1 << (block_index%8);
}

static inline void block_clear_bitmap(uint16_t block_index){
  block_bitmap[block_index/8] &= ~(1 << (block_index%8));
}

static inline bool block_is_free(uint16_t block_index){
  return ((block_bitmap[block_index/8] & (1<<(block_index%8))) == 0);
}

static inline uint16_t block_addr_to_index(block_t* addr){
  return ((uint32_t) addr - (uint32_t) blocks) / BLOCKSIZE;
}
  
static inline block_t* block_index_to_addr(uint16_t block_index){
  return &blocks[block_index];
}

void block_initialize(char* blocks_ptr,
                      uint8_t* bitmap_ptr,
                      super_block_t* super_block){
  uint16_t i;
  super_block->num_free_blocks = NUM_BLOCKS;
  blocks = (block_t*)blocks_ptr;
  block_bitmap = bitmap_ptr;
  for(i=0;i<NUM_BLOCKS;i++){
    block_clear_bitmap(i);
  }
}

block_t* block_get_free(){
  uint16_t i;
  for(i=0;i<NUM_BLOCKS;i++){
    if(block_is_free(i)){
      block_set_bitmap(i);
      super_block->num_free_blocks--;
      return block_index_to_addr(i);
    }
  }
  return NULL;
}
 
void block_remove(block_t* ptr){
  super_block->num_free_blocks++;
  block_clear_bitmap(block_addr_to_index(ptr));
}
