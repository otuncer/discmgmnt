#include "file_operations_wrappers.h"
#include "kernel_src/constants.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <fcntl.h>
#include <unistd.h>

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

int rd_open(char* pathname){
  
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
  response = ioctl (ramdisk, IOCTL_OPEN, &test_arg);
  if (response == -1 ) {
    perror("Error in IOCTL call");
    exit(1);
  }
  
  close(ramdisk);
  
  // Extract and return the result of the file operation
  return test_arg.arg_return;
}

int rd_close(int fd){
  
  // Open ramdisk 
  int ramdisk = open("/proc/ramdisk", O_RDWR);
  if(ramdisk == -1){
    perror("/proc/ramdisk");
    exit(1);
  }
  
  // Arguments for the file operation
  discos_arguments_t test_arg;
  test_arg.arg_int_first = fd;
  
  // Issue the respective ioctl call
  int response;
  response = ioctl (ramdisk, IOCTL_CLOSE, &test_arg);
  if (response == -1 ) {
    perror("Error in IOCTL call");
    exit(1);
  }
  
  close(ramdisk);
  
  // Extract and return the result of the file operation
  return test_arg.arg_return;
}

int rd_read(int fd, char* buffer, int num_bytes){
  
  // Open ramdisk 
  int ramdisk = open("/proc/ramdisk", O_RDWR);
  if(ramdisk == -1){
    perror("/proc/ramdisk");
    exit(1);
  }
  
  // Arguments for the file operation
  discos_arguments_t test_arg;
  test_arg.arg_int_first = fd;
  test_arg.arg_char = buffer;
  test_arg.arg_int_second = num_bytes;
  
  // Issue the respective ioctl call_arg
  int response;
  response = ioctl (ramdisk, IOCTL_READ, &test_arg);
  if (response == -1 ) {
    perror("Error in IOCTL call");
    exit(1);
  }
  
  close(ramdisk);
  
  // Extract and return the result of the file operation
  return test_arg.arg_return;
}

int rd_write(int fd, char* buffer, int num_bytes){
  
  // Open ramdisk 
  int ramdisk = open("/proc/ramdisk", O_RDWR);
  if(ramdisk == -1){
    perror("/proc/ramdisk");
    exit(1);
  }
  
  // Arguments for the file operation
  discos_arguments_t test_arg;
  test_arg.arg_int_first = fd;
  test_arg.arg_char = buffer;
  test_arg.arg_int_second = num_bytes;
  
  // Issue the respective ioctl call_arg
  int response;
  response = ioctl (ramdisk, IOCTL_WRITE, &test_arg);
  if (response == -1 ) {
    perror("Error in IOCTL call");
    exit(1);
  }
  
  close(ramdisk);
  
  // Extract and return the result of the file operation
  return test_arg.arg_return;
}

int rd_lseek(int fd, int offset){
  
  // Open ramdisk 
  int ramdisk = open("/proc/ramdisk", O_RDWR);
  if(ramdisk == -1){
    perror("/proc/ramdisk");
    exit(1);
  }
  
  // Arguments for the file operation
  discos_arguments_t test_arg;
  test_arg.arg_int_first = fd;
  test_arg.arg_int_second = offset;
  
  // Issue the respective ioctl call_arg
  int response;
  response = ioctl (ramdisk, IOCTL_LSEEK, &test_arg);
  if (response == -1 ) {
    perror("Error in IOCTL call");
    exit(1);
  }
  
  close(ramdisk);
  
  // Extract and return the result of the file operation
  return test_arg.arg_return;
}

int rd_unlink(char* pathname){
  
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
  response = ioctl (ramdisk, IOCTL_UNLINK, &test_arg);
  if (response == -1 ) {
    perror("Error in IOCTL call");
    exit(1);
  }
  
  close(ramdisk);
  
  // Extract and return the result of the file operation
  return test_arg.arg_return;
}

int rd_readdir(int fd, char* buffer){
  
  // Open ramdisk 
  int ramdisk = open("/proc/ramdisk", O_RDWR);
  if(ramdisk == -1){
    perror("/proc/ramdisk");
    exit(1);
  }
  
  // Arguments for the file operation
  discos_arguments_t test_arg;
  test_arg.arg_int_first = fd;
  test_arg.arg_char = buffer;
  
  // Issue the respective ioctl call
  int response;
  response = ioctl (ramdisk, IOCTL_READDIR, &test_arg);
  if (response == -1 ) {
    perror("Error in IOCTL call");
    exit(1);
  }
  
  close(ramdisk);
  
  // Extract and return the result of the file operation
  return test_arg.arg_return;
}
