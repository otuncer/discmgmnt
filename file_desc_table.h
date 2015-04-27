#ifndef FILE_DESC_TABLE_H
#define FILE_DESC_TABLE_H

#ifdef DEBUG
#include <stdint.h>
#else
#include <linux/types.h>
#endif

typedef struct _file_desc
{
  uint16_t fd;
  uint16_t inode;
  uint32_t f_pos;
  struct _file_desc *next;
} file_desc;

file_desc* file_desc_table;

//returns fd
uint16_t add_file_desc(uint16_t inode);

//returns 0 if fd does not exist
//returns 1 if successful
//---bool or int did not compile for some reason
uint16_t remove_fd(uint16_t fd);

//returns NULL if fd is not found
file_desc* get_fd(uint16_t fd);

#endif
