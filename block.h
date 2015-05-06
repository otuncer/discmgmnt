#ifndef BLOCH_H
#define BLOCK_H

#include "constants.h"
#include "directory_entry.h"

#ifdef DEBUG
#include <stdbool.h>
#include <stdint.h>
#else
#include <linux/types.h>
#endif

/*********************/
/** DATA STRUCTURES **/
/*********************/

/* RAMDISK Block */
typedef union _block_t{
  char data[BLOCKSIZE];
  directory_entry_t directories[BLOCKSIZE/(FILENAME_SIZE+2)];
  union _block_t* block_ptr[BLOCKSIZE/4];
}block_t;

typedef struct _super_block_t{
  uint16_t num_free_blocks;
  uint16_t num_inodes;
} super_block_t;

extern super_block_t* super_block;

/*************************/
/** FUNCTION PROTOTYPES **/
/*************************/
/*
 * Initialize ramdisk block partition and the bitmap array
 * */
void block_initialize(char* blocks_ptr,
                      uint8_t* bitmap_ptr,
                      super_block_t* super_block);

/*
 * Allocates a new block
 * Returns:
 *  Pointer to a free ramdisk block
 *  NULL if no free blocks exist
 * */
block_t* block_get_free(void);

/*  
 * Set the block with a given index to be free
 * */
void block_remove(block_t*);

#endif