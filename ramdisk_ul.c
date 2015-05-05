/* User level test program to test the ramdisk module */

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "constants.h"
#include "file_operations_wrappers.h"

int ramdisk;

int main(){
  
  int ramdisk = open("/proc/ramdisk", O_RDWR);
  
  if(ramdisk == -1){
    perror("/proc/ramdisk");
    exit(1);
  }
  
  
  close(ramdisk);
}
