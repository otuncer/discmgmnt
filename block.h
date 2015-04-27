#ifndef BLOCH_H
#define BLOCK_H

#include "constants.h"
#include "directory_entry.h"

#ifdef DEBUG
#include <stdint.h>
#else
#include <linux/types.h>
#endif

/*********************/
/** DATA STRUCTURES **/
/*********************/

/* RAMDISK Block */
typedef union _block_t{
  char data[BLOCK_SIZE];
  directory_entry_t directories[BLOCK_SIZE/(FILENAME_SIZE+2)];
}block_t;

/* Block Bitmap Array 
 * A single bit indicates whether the block is occupied 
 * */
extern uint8_t* block_bitmap;

extern block_t* blocks;

/*************************/
/** FUNCTION PROTOTYPES **/
/*************************/
/*
 * Initialize ramdisk block partition and the bitmap array
 * */
void block_initialize(char* blocks_ptr, uint8_t* bitmap_ptr);

/*
 * Returns:
 *  A pointer to a ramdisk block
 * */ 
block_t* block_get_pointer(uint16_t);

/*
 * Returns:
 *  Index of a free ramdisk block
 *  -1 if no free blocks exist
 * */
int block_get_free_index(void);

/*  TODO : remove block
 *  de-allocates the given block
 * */
void block_remove(uint16_t);
 

#endif