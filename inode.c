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

uint16_t inode_add_entry(uint16_t parent_inode, char* name, uint16_t isFile){
 
  inode_t* parent_ptr = inode_get_pointer(parent_inode);
 
  // Create a new child inode
  inode_t child_node;
  uint16_t child_node_index = inode_get_free_index();
  if(child_node_index==0) return 0; // Could not allocate inode
  child_node.size=0;
  (isFile==1) ? (strcpy(child_node.type,"reg")):(strcpy(child_node.type,"dir"));
  
  // Create a new directory
  directory_entry_t new_dir;
  strcpy(new_dir.filename,name);
  new_dir.index_node_number = child_node_index;
  
  // Find a target block for adding a new entry
  uint8_t block_offset = (parent_ptr->size)/(BLOCK_SIZE); // index into the location table
  uint8_t in_block_offset = (parent_ptr->size % BLOCK_SIZE)/(sizeof(directory_entry_t)); // index into the block
  block_t* target_block = inode_get_block(parent_inode, block_offset);
  
  if(target_block==NULL) return 0; // Could not a allocate block
  
  // Add newly created entry to the parent node
  target_block->directories[in_block_offset] = new_dir;
  parent_ptr->size += sizeof(directory_entry_t);
  
  return child_node_index;
}

block_t* inode_get_block(uint16_t inode, uint16_t block_offset){
  //find whether adding a new block
  uint16_t num_used_blocks = inode_head[inode].size / 256
                            + ((inode_head[inode].size % 256) != 0);
  bool adding = (block_offset >= num_used_blocks);
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

bool inode_remove_entry(uint16_t parent_inode, char* name){
  uint16_t entry_index;
  uint16_t child_inode;
  
  //check entry existence
  if((child_inode = inode_find_entry(parent_inode, name, &entry_index)) == 0){
    return false;
  }
  
  //free inode
  inode_head[child_inode].type[0] = '\0';
  
  //shift all entries back
  inode_t* parent_ptr = inode_get_pointer(parent_inode);
  uint16_t num_entries = (parent_ptr->size)/sizeof(directory_entry_t);
  uint16_t entry_per_block = BLOCK_SIZE/sizeof(directory_entry_t);
  block_t* dest_blk = inode_get_block(parent_inode, 
                                     entry_index / entry_per_block);
  block_t* src_blk = dest_blk;
  uint16_t it;
  for(it = entry_index; it < num_entries - 1; it++){
    if((it+1) % entry_per_block == 0){
      src_blk = inode_get_block(parent_inode, (it+1) / entry_per_block);
    }
    dest_blk->directories[it%entry_per_block] = 
                                  src_blk->directories[(it+1)%entry_per_block];
    dest_blk = src_blk;
  }
  //reduce parent size
  parent_ptr->size -= sizeof(directory_entry_t);
  
  //delete last block if it is not being used
  if((it+1) % entry_per_block == 0){
    block_remove(src_blk);
  }
  return true;
}

uint16_t inode_find_entry(uint16_t parent_inode, char* name, uint16_t* entryIndex){
   
  inode_t* parent_ptr = inode_get_pointer(parent_inode);
   
  // Check if directory or a file
  if(!strcmp(parent_ptr->type, "dir"))
    return 0;
     
  // Go through the location field of the inode to search entries
  uint16_t num_dir_entries = (parent_ptr->size)/sizeof(directory_entry_t);
  uint16_t num_entry_per_block = BLOCK_SIZE/sizeof(directory_entry_t);
  uint16_t i=0;
  uint16_t cur_block_index=0;
  block_t* cur_block_ptr;
   
  for(i=0;i<num_dir_entries;i++){
     
    if((i%num_entry_per_block)==0){ // Move on to the next block?
      cur_block_ptr = inode_get_block(parent_inode, cur_block_index);
      cur_block_index++;
    }
     
    if(!strcmp(cur_block_ptr->directories[i%num_entry_per_block].filename, name)){ // check if filename matches
      *entryIndex = i;
      return cur_block_ptr->directories[i%num_entry_per_block].index_node_number;
    }
  }
  return 0; // No matching filename found within the entries of this inode
}
