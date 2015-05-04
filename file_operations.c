#include "file_operations.h"
#include "file_desc_table.h"

#ifdef DEBUG
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#else
#include <linux/string.h>
#include <linux/types.h>
#endif

//TODO implement rd_creat
int rd_creat(char* pathname){
  return -1;
}

//TODO implement rd_mkdir
int rd_mkdir(char* pathname){
  return -1;
}

//TODO implement rd_open
int rd_open(char* pathname){
  return -1;
}

//TODO implement rd_close
int rd_close(int fd){
  return -1;
}

int rd_read(int fd, char* buffer, int num_bytes){
  
  // Major variables
  int num_bytes_read;           // Number of bytes that are successfully read
  file_desc* target_file_desc;  // File descriptor of the target file being read
  uint16_t target_inode_index;  // Inode index 
  inode_t* target_inode_ptr;    // Pointer to the corresponding inode
  
  // Check if the target file exists
  target_file_desc = file_get_fd(fd);
  if(target_file_desc==NULL) 
    goto fail;
  
  // Verify that target is not a directory file
  target_inode_index = target_file_desc->inode;
  target_inode_ptr = inode_get_pointer(target_inode_index);
  if(strcmp(target_inode_ptr->type,"reg")) 
    goto fail;
  
  // Check if the f_pos is in a feasible range
  if((target_file_desc->f_pos)>=(target_inode_ptr->size))
    goto fail;
  
  // Read bytes from the file
  num_bytes_read = inode_read_bytes(target_inode_index, buffer, num_bytes, target_file_desc->f_pos);
  if(num_bytes_read == -1)
    goto fail;
  
  // Update the file position
  target_file_desc->f_pos += num_bytes_read;
  
  success:
    return num_bytes_read;
  fail:
    return -1;
}

//TODO implement rd_write
int rd_write(int fd, char* buffer, int num_bytes){
  return -1;
}

//TODO implement rd_seek
int rd_seek(int fd, int offset){
  return -1;
}

//TODO implement rd_unlink
int rd_unlink(char* pathname){
  return -1;
}

//TODO implement rd_readdir
int rd_readdir(int fd, char* buffer){
  return -1;
}

//TODO implement rd_check_path
static inode_t* rd_check_path(char* pathname, inode_t* parent){
  
  
  
  
  return NULL;
}
