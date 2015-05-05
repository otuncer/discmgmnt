/* User level test program to test the ramdisk module */

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "constants.h"
#include "file_operations_wrappers.h"

int main(){
  rd_creat("asd");
  
  return 0;
}
