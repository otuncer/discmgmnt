#include "file_operations_wrappers.h"
#include "constants.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <fcntl.h>

int rd_creat(char* pathname){

  discos_arguments_t arguments;
  arguments.arg_int_first = 10;
  arguments.arg_int_second = 11;
  arguments.arg_char = "dir1";
  printf("%p\n", &arguments);
  printf("UL: %d %s %d \n", arguments.arg_int_first, arguments.arg_char, arguments.arg_int_second);
  printf("FP2 %d\n", ramdisk);
  int response;
  response = ioctl (ramdisk, IOCTL_CREAT, &arguments);
  if (response == -1 ) {
    perror("OOPS! Something went wrong... Did you enable the module?\n");
    exit(1);
  }
  printf("%p\n", &arguments);
  printf("UL: %d %s %d \n", arguments.arg_int_first, arguments.arg_char,arguments.arg_int_second);

  close(ramdisk);
}
/*
int rd_mkdir(char* pathname);
int rd_open(char* pathname);

int rd_close(int fd);

int rd_read(int d, char* buffer, int num_bytes);

int rd_write(int d, char* buffer, int num_bytes);

int rd_lseek(int fd, int offset);

int rd_unlink(char* pathname);

int rd_readdir(int fd, char* buffer);
*/
