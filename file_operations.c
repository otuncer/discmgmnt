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

int rd_creat(char* pathname){
  int parent;
  char child_name[FILENAME_SIZE];
  int child = rd_check_path(pathname, &parent, child_name);
  if(child != -1 || parent == -1){
    return -1;
  }
  if(inode_add_entry(parent, child_name, 1) == 0){
    return -1;
  }
  return 0;
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

int rd_check_path(char* pathname, int* parent, char* leaf_name){
  *parent = -1;
  int child = 0;
  char next[FILENAME_SIZE];
  char* iter;
  uint16_t dummy;
  
  //check root
  if(pathname[0] != '/'){
    return -1;
  }else{
    pathname++;
  }
    
  //get next name
  while(*pathname != '\0'){
    iter = next;
    while(*pathname != '/' && *pathname != '\0'){
      *iter = *pathname;
      iter++;
      pathname++;
    }
    *iter = '\0';
    if(*pathname == '/'){
      pathname++;
    }
    *parent = child;
    if(*parent != -1){
      child = inode_find_entry(*parent, next, &dummy);
      if(child == 0){
        child = -1;
      }
    } else {
      child = -1;
    }
  }
  strcpy(leaf_name, next);
  
  return child;
}
