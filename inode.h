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

#define TYPE_MEMBER_SIZE 4 //in bytes

typedef struct _inode_t{
	char type[TYPE_MEMBER_SIZE];
	uint32_t size; 
	uint16_t location[NUM_BLOCK_PTRS];
}inode_t;

extern inode_t* inode_head;

/*******************************/
/** INODE Function Prototypes **/
/*******************************/
/*
 * Set type="",
 * size=0
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
/* TODO inode add entry
 * Add an entry to a given inode and initialize it
 * Cannot add entry if the name already exists.
 * Returns:
 *   child inode index
 *   0 if unsuccessful
 * */
uint16_t inode_add_entry(uint16_t parent_inode, char* name, uint16_t isFile);
/* TODO inode find entry
 * finds and returns an entry from the inode of a folder
 * Returns:
 *   found inode index
 *   0 if unsuccessful
 * */
uint16_t inode_find_entry(uint16_t parent_inode, char* name);
/* TODO inode remove entry
 * removes the given entry 
 * Shifts the existing enries to keep the entry continuity
 * Returns:
 *   1 if successful
 *   0 if unsuccessful
 * */
uint16_t inode_remove_entry(uint16_t parent_inode, char* name);

#endif
