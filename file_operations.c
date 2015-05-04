#include "file_operations.h"

#ifdef DEBUG
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#else
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

//TODO implement rd_read
int rd_read(int d, char* buffer, int num_bytes){
  return -1;
}

//TODO implement rd_write
int rd_write(int d, char* buffer, int num_bytes){
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
