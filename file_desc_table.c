#include "file_desc_table.h"

#ifdef DEBUG
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#else
#include <linux/types.h>
#include <linux/vmalloc.h>
#endif

uint16_t file_add_desc(uint16_t inode)
{
  //create new descriptor
#ifdef DEBUG
  file_desc* new_desc = malloc(sizeof(file_desc));
#else
  file_desc* new_desc = vmalloc(sizeof(file_desc));
#endif
  new_desc->fd = file_find_next_fd();
  new_desc->inode = inode;
  new_desc->f_pos = 0;
  new_desc->next = file_desc_table; //add to head
  file_desc_table = new_desc;
  return new_desc->fd;
}

uint16_t file_remove_fd(uint16_t fd)
{
  file_desc* iter = file_desc_table;
  if(iter == NULL){
    return 0;
  }else if(iter->fd == fd){
    file_desc_table = iter->next;
#ifdef DEBUG
    free(iter);
#else
    vfree(iter);
#endif
    return 1;
  }else{
    while(iter->next != NULL && iter->next->fd != fd){
      iter = iter->next;
    }
    if(iter->next == NULL){
      return 0;
    }else{
      file_desc* temp = iter->next;
      iter->next = iter->next->next;
#ifdef DEBUG
      free(temp);
#else
      vfree(temp);
#endif
      return 1;
    }
  }
}

file_desc* file_get_fd(uint16_t fd)
{
  file_desc* iter;
  for(iter = file_desc_table;
      iter != NULL && iter->fd != fd;
      iter = iter->next);
  return iter;
}

uint16_t file_find_next_fd()
{
  int fd = -1;
  file_desc* iter = file_desc_table;
  do{
    fd++;
    //look for fd
    for(iter = file_desc_table;
        iter != NULL && iter->fd != fd;
        iter = iter->next);
  }while(iter != NULL);
  return fd;
}

bool file_is_open(uint16_t inode){
  file_desc* iter;
  for(iter = file_desc_table; iter != NULL; iter = iter->next){
    if(iter->inode == inode){
      return true;
    }
  }
  return false;
}
