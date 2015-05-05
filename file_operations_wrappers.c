#include "file_operations_wrappers.h"
#include "constants.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <fcntl.h>

int rd_creat(char* pathname){
  
  // Open ramdisk 
  int ramdisk = open("/proc/ramdisk", O_RDWR);
  if(ramdisk == -1){
    perror("/proc/ramdisk");
    exit(1);
  }
  
  // Arguments for the file operation
  discos_arguments_t test_arg;
  test_arg.arg_char = pathname;
  
  // Issue the respective ioctl call
  int response;
  response = ioctl (ramdisk, IOCTL_CREAT, &test_arg);
  if (response == -1 ) {
    perror("Error in IOCTL call");
    exit(1);
  }
  
  close(ramdisk);
  
  // Extract and return the result of the file operation
  return test_arg.arg_return;
}

int rd_mkdir(char* pathname){
  
  // Open ramdisk 
  int ramdisk = open("/proc/ramdisk", O_RDWR);
  if(ramdisk == -1){
    perror("/proc/ramdisk");
    exit(1);
  }
  
  // Arguments for the file operation
  discos_arguments_t test_arg;
  test_arg.arg_char = pathname;
  
  // Issue the respective ioctl call
  int response;
  response = ioctl (ramdisk, IOCTL_MKDIR, &test_arg);
  if (response == -1 ) {
    perror("Error in IOCTL call");
    exit(1);
  }
  
  close(ramdisk);
  
  // Extract and return the result of the file operation
  return test_arg.arg_return;
}

int rd_open(char* pathname);

int rd_close(int fd);

int rd_read(int d, char* buffer, int num_bytes);

int rd_write(int d, char* buffer, int num_bytes);

int rd_lseek(int fd, int offset);

int rd_unlink(char* pathname);

int rd_readdir(int fd, char* buffer);
