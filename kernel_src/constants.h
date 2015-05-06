#ifndef CONSTANTS_H
#define CONSTANTS_H

//It's the user responsibility to use matching constants

#define PARTITION_SIZE 2*1024*1024//must be <=4GB
#define NUM_BLOCKS     8192       //must be a multiple of 8 and <= 2^16
#define BLOCKSIZE      256        //in bytes

#define NUM_INODES     1024       //must be multiple of 4 and <= 2^16
#define FILENAME_SIZE  14         //including the null terminator
#define NUM_DIRECT_PTRS 8
#define NUM_S_INDIRECT_PTRS 1
#define NUM_D_INDIRECT_PTRS 1
#define NUM_BLOCK_PTRS (NUM_DIRECT_PTRS+NUM_S_INDIRECT_PTRS+NUM_D_INDIRECT_PTRS)

/** UNIT OF TRANSFER BETWEEN THE USER AND KERNEL LEVEL: ARGUMENTS **/
typedef struct _discos_arguments_t{
  int arg_int_first;  // file descriptor
  char* arg_char;     // address or pathname
  int arg_int_second; // num_bytes or offset
  int arg_return;     // return value
}discos_arguments_t;

/** IOCTL COMMANDS **/
#define DISCOS_MAGIC 'k'
#define IOCTL_CREAT   _IOWR(DISCOS_MAGIC,1, discos_arguments_t*)
#define IOCTL_MKDIR   _IOWR(DISCOS_MAGIC,2, discos_arguments_t*)
#define IOCTL_OPEN    _IOWR(DISCOS_MAGIC,3, discos_arguments_t*)
#define IOCTL_CLOSE   _IOWR(DISCOS_MAGIC,4, discos_arguments_t*)
#define IOCTL_READ    _IOWR(DISCOS_MAGIC,5, discos_arguments_t*)
#define IOCTL_WRITE   _IOWR(DISCOS_MAGIC,6, discos_arguments_t*)
#define IOCTL_LSEEK   _IOWR(DISCOS_MAGIC,7, discos_arguments_t*)
#define IOCTL_UNLINK  _IOWR(DISCOS_MAGIC,8, discos_arguments_t*)
#define IOCTL_READDIR _IOWR(DISCOS_MAGIC,9, discos_arguments_t*)

#endif
