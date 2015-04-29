/* Index Node Structure */
#ifndef INODE_H
#define INODE_H

#include "constants.h"
#include "block.h"

#ifdef DEBUG
#include <stdint.h>
#else
#include <linux/types.h>
#endif

/* The following members are in bytes */
#define TYPE_MEMBER_SIZE 4
#define FILENAME_SIZE 14

typedef struct _inode_t{
	
	char type[TYPE_MEMBER_SIZE];
	uint32_t size; 
	uint16_t location[NUM_BLOCK_PTRS];
}inode_t;

inode_t* inode_head;

/*******************************/
/** INODE Function Prototypes **/
/*******************************/
/*
 * Set type="",
 * size=0
 * location=(nullified blocks)
 * */
void inode_initialize(char* inode_partition);
/*
 * Returns:
 *  A pointer to the inode with a given index
 *  NULL if index out of range
 * */
inode_t* inode_get_pointer(uint16_t index);
/*
 * Returns:
 *  The index number of an unused inode
 *  0 if no empty inode exists (0th node is always used by root)
 * */
uint16_t inode_get_free_index(void);
/*
 * Add a directory entry to a given inode
 * Returns:
 *   number of bytes added
 * */
uint16_t inode_add_directory_entry(uint16_t, char*, uint16_t);

#endif
