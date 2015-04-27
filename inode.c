#include "inode.h"

#ifdef DEBUG
#include <stdint.h>
#include <string.h>
#else
#include <linux/string.h>
#include <linux/types.h>
#endif

/* TODO inode get next location
 * returns the next available location index in the given inode
 * returns -1 if there is none
 * */
static int inode_entry_get_next(uint16_t inode);

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

/*
uint16_t inode_add_directory_entry(uint16_t inode_index, char* dirName){
  inode_t* parent_node = inode_get_pointer(inode_parent_index);
  
  // Create a new directory
  directory_entry_t new_dir;
  strcpy(new_dir.filename,filename);
  new_dir.index_node_number;
  
  uint8_t location_indexer = (parent_node->size)/(BLOCK_SIZE); // index into the location table
  uint8_t block_indexer = (parent_node->size % BLOCK_SIZE)/(sizeof(directory_entry_t)); // index into the block
  
  if(block_indexer==0){//need to allocate a new bloack
    parent_node->location[location_indexer]=block_get_free_index();
  }
  
  block_get_pointer(parent_node->location[location_indexer])->directories[block_indexer] = new_dir;

  parent_node->size += sizeof(directory_entry_t);
}*/