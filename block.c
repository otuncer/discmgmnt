#include "block.h"

#ifdef DEBUG
#include <string.h>
#else
#include <linux/string.h>
#endif

void block_initialize(char* blocks_partition_address){
  
  blocks = (block_t*)blocks_partition_address;
  uint16_t i;
  for(i=0;i<NUM_BLOCKS;i++){
    strcpy(blocks[i].data,"");
    block_clear_bitmap(i);
  }
}

void block_set_bitmap(uint16_t block_index){
  block_bitmap[block_index/8] |= 1 << (block_index%8);
}

void block_clear_bitmap(uint16_t block_index){
  block_bitmap[block_index/8] &= ~(1 << (block_index%8));
}

uint8_t block_check_bitmap(uint16_t block_index){
  return ((block_bitmap[block_index/8] & (1<<(block_index%8))) != 0);
}

block_t* block_get_pointer(uint16_t block_index){
  return &blocks[block_index];
}

uint16_t block_get_free_index(){
  uint16_t i;
  for(i=0;i<NUM_BLOCKS;i++){
    if(!block_check_bitmap(i)){
      block_set_bitmap(i);
      return i;
    }
  }
  return -1;
}