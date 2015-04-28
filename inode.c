#include "inode.h"

#ifdef DEBUG
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#else
#include <linux/string.h>
#include <linux/types.h>
#endif

/*
 * Set type="",
 * size=0
 * */
void inode_initialize(char* inode_partition){
  int i,j;
  
  inode_head = (inode_t*)inode_partition;
  for(i=0; i<NUM_INODES; i++){
    strcpy(inode_head[i].type,"");
    inode_head[i].size=0;
  }
  // Initialize root 
  strcpy(inode_head[0].type,"dir");
  inode_head[0].size=0;
}

uint16_t inode_get_free_index(void){
  uint16_t i;
  for(i=0; i<NUM_INODES; i++){
    if(!strcmp(inode_head[i].type,""))
      return i;
  }
  return 0;
}

inode_t* inode_get_pointer(uint16_t index){
  if(index>=NUM_INODES)
    return NULL;
  else
    return &inode_head[index];
}

block_t* inode_get_block(uint16_t inode, uint16_t block_offset){
  //find whether adding a new block 
  uint16_t num_full_blocks = inode_head[inode].size / 256;
  bool adding = (block_offset >= num_full_blocks);
  block_t* block_ptr;
  if(adding){
    block_ptr = block_get_free();
    if(block_ptr == NULL){ // no available block
      return NULL;
    }
  }
  //find the corresponding location pointer in the inode
  uint16_t ptrs_per_blk = (BLOCK_SIZE/4);
  block_t** base_loc = inode_head[inode].location;
  if(block_offset < NUM_DIRECT_PTRS){ // direct
    if(adding){
      base_loc[block_offset] = block_ptr;
    } else {
      block_ptr = base_loc[block_offset];
    }
  }else if(block_offset < NUM_DIRECT_PTRS 
                          + NUM_S_INDIRECT_PTRS * ptrs_per_blk) {
                            //single indirect
    //remove direct ptr offsets
    block_offset -= NUM_DIRECT_PTRS;
    base_loc = &(base_loc[NUM_DIRECT_PTRS]);
    uint16_t s_index = block_offset / ptrs_per_blk;
    uint16_t s_offset = block_offset % ptrs_per_blk;

    if(!adding){
      block_ptr = base_loc[s_index]->block_ptr[s_offset];
    } else if(s_offset != 0) { //adding
      base_loc[s_index]->block_ptr[s_offset] = block_ptr;
    } else { //creating a new singly indirect block
      block_t* s_ptr = block_get_free();
      if(s_ptr == NULL){ // no available block
        block_remove(block_ptr);
        return NULL;
      } else {
        base_loc[s_index] = s_ptr;
        base_loc[s_index]->block_ptr[s_offset] = block_ptr;
      }
    }
  }else if(block_offset < NUM_DIRECT_PTRS 
                        + NUM_S_INDIRECT_PTRS * ptrs_per_blk
                        + NUM_D_INDIRECT_PTRS * ptrs_per_blk * ptrs_per_blk){
                          //double indirect
    //remove direct & single indirect ptr offsets
    block_offset -= (NUM_DIRECT_PTRS + NUM_S_INDIRECT_PTRS * ptrs_per_blk);
    base_loc = &(base_loc[NUM_DIRECT_PTRS + NUM_S_INDIRECT_PTRS]);
    uint16_t d_index = block_offset / (ptrs_per_blk * ptrs_per_blk);
    uint16_t d_offset = (block_offset % (ptrs_per_blk * ptrs_per_blk)) / ptrs_per_blk;
    uint16_t s_offset = block_offset % ptrs_per_blk;

    if(!adding){
      block_ptr = base_loc[d_index]->block_ptr[d_offset]->block_ptr[s_offset];
    } else if(s_offset != 0){ //adding
      base_loc[d_index]->block_ptr[d_offset]->block_ptr[s_offset] = block_ptr;
    } else if(d_offset != 0 && s_offset == 0){ //new single indirect pointer
      block_t* s_ptr = block_get_free();
      if(s_ptr == NULL){ // no available block
        block_remove(block_ptr);
        return NULL;
      } else {
        base_loc[d_index]->block_ptr[d_offset] = s_ptr;
        base_loc[d_index]->block_ptr[d_offset]->block_ptr[s_offset] = block_ptr;
      }
    } else { //new double pointer
      block_t* d_ptr = block_get_free();
      block_t* s_ptr = block_get_free();
      if(s_ptr == NULL){//no available block
        if(d_ptr != NULL){
          block_remove(d_ptr);
        }
        block_remove(block_ptr);
        return NULL;
      } else {
        base_loc[d_index] = d_ptr;
        base_loc[d_index]->block_ptr[d_offset] = s_ptr;
        base_loc[d_index]->block_ptr[d_offset]->block_ptr[s_offset] = block_ptr;
      }
    }
  } else { //outside block boundaries
    return NULL;
  }
  return block_ptr;
}