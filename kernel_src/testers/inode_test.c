#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../inode.h"

super_block_t* super_block;
inode_t* inode_head;
uint8_t* block_bitmap;
block_t* blocks;

/****************/
/** UNIT TESTS **/
/****************/
void get_block_test(void);
void add_entry_test(void);
void remove_entry_test(void);
void byte_read_write_test(void);

int main()
{ 
  super_block_t super_blk;
  super_block = &super_blk;
  get_block_test();
  add_entry_test(); //add & find
  remove_entry_test();
  byte_read_write_test();
  return 0;
}

void byte_read_write_test(){
  
  //init blocks
  uint8_t block_bitmap_[NUM_BLOCKS/8];
  block_t blocks_[NUM_BLOCKS];
  block_initialize((char*) blocks_, block_bitmap_, super_block);
  //init inodes
  inode_t inode_head_[NUM_INODES];
  inode_initialize((char*) inode_head_, super_block);
  
  // find the number of bytes that can actually be written
  const int max_size = BLOCKSIZE*(NUM_DIRECT_PTRS 
                            + NUM_S_INDIRECT_PTRS * BLOCKSIZE / 4
                            + NUM_D_INDIRECT_PTRS * BLOCKSIZE * BLOCKSIZE / 16);
  
  // add a file entry to the root
  uint16_t file_index = inode_add_entry(0,"file1",1);
  
  char write_data='d';
  char read_data='a';
  char readbuffer[900];
  uint32_t i;
  
  /* CASE 1: MAX BYTES */
  // write max number of bytes to a file
  for(i=0;i<max_size;i++){
    assert(inode_write_bytes(file_index, &write_data, 1, i)==1);
  }
  
  /* CASE 2: DATA CONSISTENCY */
  // now read back the written data
  for(i=0;i<max_size;i++){
    inode_read_bytes(file_index, &read_data, 1, i);
    assert(read_data==write_data);
  }
  
  /* CASE 3: No empty block */
  
  // Reinitialize the ramdisk partitions
  block_initialize((char*)blocks_, block_bitmap_, super_block);
  inode_initialize((char*) inode_head_, super_block);
  
  // add a file entry to the root
  file_index = inode_add_entry(0,"file1",1);
  
  // alocate all the blocks
  block_t* ptrs[NUM_BLOCKS];
  for(i = 0 ; i < NUM_BLOCKS; i++ ){
    ptrs[i] = block_get_free();
  }
  // try adding a new byte, should 0 as no bytes can be written
  assert(inode_write_bytes(file_index, &write_data,1,0) == 0);
  //deallocate blocks
  for(i = 0 ; i < NUM_BLOCKS; i++ ){
    block_remove(ptrs[i]);
  }
  
  printf("add/write byte test passed.\n");
}

void add_entry_test(){
  //init blocks
  uint8_t block_bitmap_[NUM_BLOCKS/8];
  block_t blocks_[NUM_BLOCKS];
  block_initialize((char*) blocks_, block_bitmap_, super_block);
  //init inodes
  inode_t inode_head_[NUM_INODES];
  inode_initialize((char*) inode_head_, super_block);
   
  //
  const int num_max_inode_blocks = NUM_DIRECT_PTRS
                            + NUM_S_INDIRECT_PTRS * BLOCKSIZE / 4
                            + NUM_D_INDIRECT_PTRS * BLOCKSIZE * BLOCKSIZE / 16;
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
    block_offset = i/(BLOCKSIZE/sizeof(directory_entry_t));
    in_block_offset = i%(BLOCKSIZE/sizeof(directory_entry_t));
    //printf("%s,%s -", buffer,inode_get_block(0,block_offset)->directories[in_block_offset].filename);
    assert(strcmp(buffer,inode_get_block(0,block_offset)->directories[in_block_offset].filename)==0);
  }
  
  /* CASE 4: Find entry test */
  uint16_t eIndex;
  assert(inode_find_entry(0,"583",&eIndex)!=0); // 583 is an existing entry name
  assert(inode_find_entry(0,"dummy",&eIndex)==0); // dummy is not an existing entry name
  
  printf("inode_add_entry & inode_find_entry test passed.\n");
}

void remove_entry_test(){
  //init blocks
  uint8_t block_bitmap_[NUM_BLOCKS/8];
  block_t blocks_[NUM_BLOCKS];
  block_initialize((char*) blocks_, block_bitmap_, super_block);
  //init inodes
  inode_t inode_head_[NUM_INODES];
  inode_initialize((char*) inode_head_, super_block);
  strcpy(inode_head[0].type,"dir");
  
  uint16_t entries[5];
  char buffer[10];
  buffer[1] = '\0';
  int i;
  for(i = 0; i < 5; i++){
    sprintf(buffer,"%d",i);
    entries[i] = inode_add_entry(0, buffer, i%2);
  }
  assert(inode_head[0].size == sizeof(directory_entry_t)*5);
  
  //put one directory and one file in '/2'
  uint16_t inode_2;
  uint16_t child_file;
  strcpy(buffer,"2");
  inode_2 = inode_find_entry(0, buffer, &inode_2);
  strcpy(buffer,"test_dir");
  inode_add_entry(inode_2, buffer, 0);
  strcpy(buffer,"test_file");
  child_file = inode_add_entry(inode_2, buffer, 1);
  //write something to file
  inode_write_bytes(child_file, "dummy_data", 10, 0);

  //attempt to remove parent
  strcpy(buffer,"2");
  assert(inode_remove_entry(0, buffer) == false);
  //remove file
  strcpy(buffer,"test_file");
  block_t* first_block = inode_get_block(child_file, 0);
  assert(inode_remove_entry(inode_2, buffer) == true);
  //check whether the next empty block is the one just removed
  assert(block_get_free() == first_block);
  //attempt to remove directory
  strcpy(buffer,"2");
  assert(inode_remove_entry(0, buffer) == false);
  //remove child directory
  strcpy(buffer,"test_dir");
  assert(inode_remove_entry(inode_2, buffer) == true);
  
  //remove 2
  strcpy(buffer,"2");
  assert(inode_remove_entry(0, buffer) == true);
  assert(inode_head[0].size == sizeof(directory_entry_t)*4);
  assert(inode_get_free_index() == entries[2]);
  assert(inode_remove_entry(0, buffer) == false);
  assert(inode_head[0].size == sizeof(directory_entry_t)*4);
  //remove others
  sprintf(buffer,"%d",0);
  assert(inode_remove_entry(0, buffer) == true);
  assert(inode_head[0].size == sizeof(directory_entry_t)*3);
  sprintf(buffer,"%d",1);
  assert(inode_remove_entry(0, buffer) == true);
  assert(inode_head[0].size == sizeof(directory_entry_t)*2);
  sprintf(buffer,"%d",3);
  assert(inode_remove_entry(0, buffer) == true);
  assert(inode_head[0].size == sizeof(directory_entry_t)*1);
  
  printf("inode_remove_entry test passed.\n");
}

void get_block_test(){

  //init blocks
  uint8_t block_bitmap_[NUM_BLOCKS/8];
  block_t blocks_[NUM_BLOCKS];
  block_initialize((char*) blocks_, block_bitmap_, super_block);
  //init inodes
  inode_t inode_head_[NUM_INODES];
  inode_initialize((char*) inode_head_, super_block);
  
  // test inode_get_block
  const int num_max_inode_blocks = NUM_DIRECT_PTRS 
                            + NUM_S_INDIRECT_PTRS * BLOCKSIZE / 4
                            + NUM_D_INDIRECT_PTRS * BLOCKSIZE * BLOCKSIZE / 16;
                            //a total of 4234 blocks, including pointer blocks
  block_t* ptrs[NUM_BLOCKS];
  //allocate max number of blocks for single inode
  int i, j;
  for(i = 0 ; i < num_max_inode_blocks; i++ ){
    ptrs[i] = inode_get_block(0, i);
    inode_head[0].size += BLOCKSIZE;
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
  block_initialize((char*)blocks_, block_bitmap_, super_block);
  inode_initialize((char*) inode_head_, super_block);
  //allocate all blocks
  for(i = 0 ; i < NUM_BLOCKS/8; i++ ){
    block_bitmap[i] = 255;
  }
  //try to get one more
  assert(inode_get_block(0, i) == NULL);
  
  printf("inode_get_block test passed.\n");
  
}
