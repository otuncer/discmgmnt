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
  return rd_create(pathname, 1);
}

int rd_mkdir(char* pathname){
  return rd_create(pathname, 0);
}

int rd_create(char* pathname, int isFile){
  int parent;
  char child_name[FILENAME_SIZE];
  int child = rd_check_path(pathname, &parent, child_name);
  if(   child != -1
     || parent == -1
     || strcmp(inode_get_pointer(parent)->type,"dir") != 0
     || inode_add_entry(parent, child_name, isFile) == 0){
    return -1;
  }
  return 0;
}

int rd_open(char* pathname){
  int parent;
  char child_name[FILENAME_SIZE];
  int child = rd_check_path(pathname, &parent, child_name);
  if(child == -1){
    return -1;
  }
  return file_add_desc(child);
}

int rd_close(int fd){
  if(file_remove_fd(fd) == 0){
    return -1;
  }
  return 0;
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
  
  // Read bytes from the file
  num_bytes_read = inode_read_bytes(target_inode_index, buffer, num_bytes, target_file_desc->f_pos);
  if(num_bytes_read == -1)
    goto fail;
  
  // Update the file position
  target_file_desc->f_pos += num_bytes_read;
  
  return num_bytes_read;
  fail:
    return -1;
}

int rd_write(int fd, char* buffer, int num_bytes){
  
  // Major variables
  int num_bytes_written;        // Number of bytes that are successfully written
  file_desc* target_file_desc;  // File descriptor of the target file being written to
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
  
  // Write bytes to the file
  num_bytes_written = inode_write_bytes(target_inode_index, buffer, num_bytes, target_file_desc->f_pos);
  if(num_bytes_written == -1)
    goto fail;
  
  // Update the file position
  target_file_desc->f_pos += num_bytes_written;
  
  return num_bytes_written;
  fail:
    return -1;
}

int rd_lseek(int fd, int offset){

  // Major variables
  file_desc* target_file_desc;  // File descriptor of the target file
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
  
  // Assign new file position 
  target_file_desc->f_pos = (offset > (target_inode_ptr->size)) ? (target_inode_ptr->size) : offset;
  
  return 0;
  fail:
    return -1;
}

int rd_unlink(char* pathname){
  int parent;
  char child_name[FILENAME_SIZE];
  int child = rd_check_path(pathname, &parent, child_name);
  if(   child == -1
     || file_is_open(child)
     || inode_remove_entry(parent, child_name) == 0){
    return -1;
  }
  return 0;
}

int rd_readdir(int fd, char* buffer){
  file_desc* file = file_get_fd(fd);
  if(   file == NULL
     || strcmp(inode_get_pointer(file->inode)->type, "dir") != 0){
       return -1;
  }
  if(file->f_pos == inode_get_pointer(file->inode)->size){
    return 0;
  }
  //copy directory entry
  inode_read_bytes(file->inode, buffer, FILENAME_SIZE+2, file->f_pos);
  
  // move f_pos to the next directory entry
  file->f_pos += sizeof(directory_entry_t);
  
  return 1;
}

int rd_check_path(char* pathname, int* parent, char* leaf_name){
  int child;
  char next[FILENAME_SIZE];
  char* iter;
  uint16_t dummy;
  
  *parent = -1;
  child = 0;
  next[0] = '\0';
  
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
