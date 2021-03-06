#include "inode.h"

#ifdef DEBUG
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#else
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#endif

inode_t* inode_head;

/*
 * Set type="",
 * size=0
 * */
void inode_initialize(char* inode_partition, super_block_t* sb){
  int i;
  sb->num_inodes = NUM_INODES;
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
  inode_t* parent_ptr;
  uint16_t a;
  inode_t child_node;
  uint16_t child_node_index;
  directory_entry_t new_dir;
  uint8_t block_offset;
  uint8_t in_block_offset;
  block_t* target_block;
  
  parent_ptr = inode_get_pointer(parent_inode);
  
  // Check if the filename already exists
  if(inode_find_entry(parent_inode, name, &(a))) return 0;
 
  // Create a new child inode
  child_node_index = inode_get_free_index();
  if(child_node_index==0) return 0; // Could not allocate inode
  child_node.size=0;
  
  // Create a new directory
  strcpy(new_dir.filename,name);
  new_dir.index_node_number = child_node_index;
  
  // Find a target block for adding a new entry
  block_offset = (parent_ptr->size)/(BLOCKSIZE); // index into the location table
  in_block_offset = (parent_ptr->size % BLOCKSIZE)/(sizeof(directory_entry_t)); // index into the block
  target_block = inode_get_block(parent_inode, block_offset);
  
  if(target_block==NULL) return 0; // Could not a allocate block
  
  // Add newly created entry to the parent node
  target_block->directories[in_block_offset] = new_dir;
  parent_ptr->size += sizeof(directory_entry_t);
  //update superblock
  super_block->num_inodes--;
  
  // inode is allocated when the type field is set
  (isFile==1) ? (strcpy(child_node.type,"reg")):(strcpy(child_node.type,"dir"));
  inode_head[child_node_index] = child_node;
  return child_node_index;
}

block_t* inode_get_block(uint16_t inode, uint16_t block_offset){
  //find whether adding a new block
  uint16_t num_used_blocks;
  block_t* block_ptr;
  uint16_t ptrs_per_blk;
  block_t** base_loc;
  uint16_t s_index, s_offset;
  uint16_t d_index, d_offset;
  block_t* d_ptr, *s_ptr;
  bool adding;
  block_ptr = NULL;
  num_used_blocks = inode_head[inode].size / 256
                    + ((inode_head[inode].size % 256) != 0);
  adding = (block_offset >= num_used_blocks);
  if(adding){
    block_ptr = block_get_free();
    if(block_ptr == NULL){ // no available block
      return NULL;
    }
  }
  //find the corresponding location pointer in the inode
  ptrs_per_blk = (BLOCKSIZE/4);
  base_loc = inode_head[inode].location;
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
    s_index = block_offset / ptrs_per_blk;
    s_offset = block_offset % ptrs_per_blk;

    if(!adding){
      block_ptr = base_loc[s_index]->block_ptr[s_offset];
    } else if(s_offset != 0) { //adding
      base_loc[s_index]->block_ptr[s_offset] = block_ptr;
    } else { //creating a new singly indirect block
      s_ptr = block_get_free();
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
    d_index = block_offset / (ptrs_per_blk * ptrs_per_blk);
    d_offset = (block_offset % (ptrs_per_blk * ptrs_per_blk)) / ptrs_per_blk;
    s_offset = block_offset % ptrs_per_blk;

    if(!adding){
      block_ptr = base_loc[d_index]->block_ptr[d_offset]->block_ptr[s_offset];
    } else if(s_offset != 0){ //adding
      base_loc[d_index]->block_ptr[d_offset]->block_ptr[s_offset] = block_ptr;
    } else if(d_offset != 0 && s_offset == 0){ //new single indirect pointer
      s_ptr = block_get_free();
      if(s_ptr == NULL){ // no available block
        block_remove(block_ptr);
        return NULL;
      } else {
        base_loc[d_index]->block_ptr[d_offset] = s_ptr;
        base_loc[d_index]->block_ptr[d_offset]->block_ptr[s_offset] = block_ptr;
      }
    } else { //new double pointer
      d_ptr = block_get_free();
      s_ptr = block_get_free();
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
  inode_t* child_ptr, *parent_ptr;
  int32_t i;
  uint16_t num_entries;
  uint16_t entry_per_block;
  block_t* dest_blk, *src_blk;
  
  //check entry existence
  if((child_inode = inode_find_entry(parent_inode, name, &entry_index)) == 0){
    return false;
  }
  child_ptr = inode_get_pointer(child_inode);
  //if folder, check if root or non-empty
  if(strcmp(child_ptr->type, "dir") == 0){
    if(child_ptr->size != 0 || child_inode == 0){
      return false;
    }
  } else { // if file, free all blocks
    uint16_t num_used_blocks = child_ptr->size / 256
                                + ((child_ptr->size % 256) != 0);
    for(i = (int32_t) num_used_blocks - 1; i >= 0; i--){
      block_remove(inode_get_block(child_inode, i));
    }
  }
  
  //free inode
  child_ptr->type[0] = '\0';
  
  //shift all entries back
  parent_ptr = inode_get_pointer(parent_inode);
  num_entries = (parent_ptr->size)/sizeof(directory_entry_t);
  entry_per_block = BLOCKSIZE/sizeof(directory_entry_t);
  dest_blk = inode_get_block(parent_inode, 
                                     entry_index / entry_per_block);
  src_blk = dest_blk;
  for(i = entry_index; i < num_entries - 1; i++){
    if((i+1) % entry_per_block == 0){
      src_blk = inode_get_block(parent_inode, (i+1) / entry_per_block);
    }
    dest_blk->directories[i%entry_per_block] = 
                                  src_blk->directories[(i+1)%entry_per_block];
    dest_blk = src_blk;
  }
  //delete last block if it is not being used
  if((i+1) % entry_per_block == 0){
    block_remove(src_blk);
  }
  
  //reduce parent size
  parent_ptr->size -= sizeof(directory_entry_t);
  //update superblock
  super_block->num_inodes++;
  
  return true;
}

uint16_t inode_find_entry(uint16_t parent_inode, char* name, uint16_t* entryIndex){
  inode_t* parent_ptr;
  uint16_t num_dir_entries;
  uint16_t num_entry_per_block;
  uint16_t i;
  uint16_t cur_block_index;
  block_t* cur_block_ptr;
  
  parent_ptr = inode_get_pointer(parent_inode);

  // Check if directory or a file
  if(strcmp(parent_ptr->type, "dir"))
    return 0;

  // Go through the location field of the inode to search entries
  num_dir_entries = (parent_ptr->size)/sizeof(directory_entry_t);
  num_entry_per_block = BLOCKSIZE/sizeof(directory_entry_t);
  cur_block_index=0;
  cur_block_ptr=NULL;
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

int inode_read_bytes(uint16_t file_inode, char* buffer, int num_bytes, uint32_t f_pos){
  // find the number of bytes that can actually be read
  const int file_size = inode_get_pointer(file_inode)->size;
  int num_bytes_feasible;
  int i;
  uint32_t block_offset;
  uint32_t in_block_offset;
  block_t* cur_block;
  
  num_bytes_feasible = ((file_size-f_pos)>=num_bytes) ? num_bytes 
                                                      : (file_size-f_pos);
  // read bytes into the buffer
  for(i=0;i<num_bytes_feasible;i++){

    // identify target bytes index
    block_offset = (f_pos+i)/BLOCKSIZE;
    in_block_offset = (f_pos+i)%BLOCKSIZE;
    cur_block = inode_get_block(file_inode,block_offset);
    
    // cannot find the block (not sure if that would occur anyway)
    if(cur_block == NULL) goto fail;
    
    // read fromthe inode block into the buffer
#ifndef DEBUG
    if(copy_to_user(&buffer[i], &(cur_block->data[in_block_offset]),1) != 0){
      goto fail;
    }
#else
    buffer[i] = cur_block->data[in_block_offset];
#endif
  }
  return num_bytes_feasible;
  fail:
    return -1;
}

int inode_write_bytes(uint16_t file_inode, char* buffer, int num_bytes, uint32_t f_pos){
  
  // find the number of bytes that can actually be written
  const int max_size = BLOCKSIZE*(NUM_DIRECT_PTRS 
                            + NUM_S_INDIRECT_PTRS * BLOCKSIZE / 4
                            + NUM_D_INDIRECT_PTRS * BLOCKSIZE * BLOCKSIZE / 16);

  int num_bytes_feasible;
  int i;
  uint32_t block_offset;
  uint32_t in_block_offset;
  block_t* cur_block;
  
  num_bytes_feasible = ((max_size-f_pos)>=num_bytes) ? num_bytes : (max_size-f_pos);
  
  // read bytes into the buffer
  for(i=0;i<num_bytes_feasible;i++){
    // identify target bytes index
    block_offset = (f_pos+i)/BLOCKSIZE;
    in_block_offset = (f_pos+i)%BLOCKSIZE;
    cur_block = inode_get_block(file_inode,block_offset);
    
    // cannot allocate a free block
    if(cur_block == NULL) { return i;} // return number of bytes written so far
    
    // read from the inode block into the buffer
#ifndef DEBUG
    if(copy_from_user(&(cur_block->data[in_block_offset]), &buffer[i], 1) != 0){
      return -1;
    }
#else
    cur_block->data[in_block_offset] = buffer[i];
#endif
    // update the size of the file
    if((f_pos+i)==(inode_get_pointer(file_inode)->size)){
      inode_get_pointer(file_inode)->size += 1; // increments in bytes
    }
  }
  return num_bytes_feasible;
}
