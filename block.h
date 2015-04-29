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
uint8_t block_bitmap[NUM_BLOCKS/8]; // Divide by X for an uintX_t type

block_t* blocks;

/*************************/
/** FUNCTION PROTOTYPES **/
/*************************/
/*
 * Initialize ramdisk block partition and the bitmap array
 * */
void block_initialize(char*);

/*
 * Returns:
 *  0: (blocks[index] is occupied)
 *  1: (blocks[index] is free )
 * */ 
uint8_t block_check_bitmap(uint16_t);

/*
 * Returns:
 *  A pointer to a newly allocated ramdisk block
 * */ 
block_t* block_get_pointer(uint16_t);

/*
 * Set/Reset the indicator bit for a newly allocated block
 * */
void block_set_bitmap(uint16_t);
void block_clear_bitmap(uint16_t);

/*
 * Returns:
 *  Index of a free ramdisk block
 *  -1 if no free blocks exist
 * */
uint16_t block_get_free_index(void);

#endif