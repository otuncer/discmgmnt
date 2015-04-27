#ifndef CONSTANTS_H
#define CONSTANTS_H

//It's the user responsibility to use matching constants

#define PARTITION_SIZE 2*1024*1024//must be <=4GB
#define NUM_BLOCKS     8192       //must be a multiple of 8 and <= 2^16
#define BLOCK_SIZE     256        //in bytes

#define NUM_INODES     1024       //must be multiple of 4 and <= 2^16
#define FILENAME_SIZE  14         //including the null terminator
#define NUM_DIRECT_PTRS 8
#define NUM_S_INDIRECT_PTRS 1
#define NUM_D_INDIRECT_PTRS 1
#define NUM_BLOCK_PTRS (NUM_DIRECT_PTRS+NUM_S_INDIRECT_PTRS+NUM_D_INDIRECT_PTRS)

#endif
