/* Index Node Structure */
#ifndef INODE_H
#define INODE_H

#include <linux/types.h>
#include "constants.h"

/* The following members are in bytes */
#define TYPE_MEMBER_SIZE 4 

struct inode{
	
	/*
	 * "dir" or "reg"
	 * */
	char type[TYPE_MEMBER_SIZE];
	
	/* The current sie of the corresponding file in bytes. For directories, this will 
	be the size of all entries in the corresponding directory file, where each entry's size
	is the number f bytes needed to record a (filename, #inode) pair */
	uint32_t size; 

	/*
	 * Needed to identify the blocks
	 * */
	uint16_t location[NUM_BLOCK_PTRS];
};

#endif
