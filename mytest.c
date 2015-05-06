#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <execinfo.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "file_operations_wrappers.h"

#define PARTITION_SIZE 2*1024*1024//must be <=4GB
#define NUM_BLOCKS     8192       //must be a multiple of 8 and <= 2^16
#define BLOCKSIZE      256        //in bytes

#define NUM_INODES     1024       //must be multiple of 4 and <= 2^16
#define FILENAME_SIZE  14         //including the null terminator
#define NUM_DIRECT_PTRS 8
#define NUM_S_INDIRECT_PTRS 1
#define NUM_D_INDIRECT_PTRS 1
#define NUM_BLOCK_PTRS (NUM_DIRECT_PTRS+NUM_S_INDIRECT_PTRS+NUM_D_INDIRECT_PTRS)

void test_file_operations(){
  
  // Create a sample tree of directories and files
  assert(0==rd_mkdir("/sys"));
  assert(0==rd_mkdir("/etc"));
  assert(0==rd_mkdir("/sys/class"));
  assert(0==rd_mkdir("/sys/class/devices"));
  assert(0==rd_creat("/sys/class/devices/f1"));
  assert(0==rd_creat("/sys/class/devices/f2"));
  assert(0==rd_creat("/sys/class/devices/f3"));
  assert(0==rd_creat("/sys/class/devices/f4"));
  assert(0==rd_creat("/sys/class/devices/f5"));
  
  /*** TEST 1: Open a file ***/
  // Tests: rd_open
  int fp1 = rd_open("/sys/class/devices/f1");
  assert(fp1!=-1);
  
  /*** TEST 2: Write to a file till its size reaches to maximum ***/
  // Tests: rd_read, rd_write, rd_lseek
  const int max_size = BLOCKSIZE*(NUM_DIRECT_PTRS 
                            + NUM_S_INDIRECT_PTRS * BLOCKSIZE / 4
                            + NUM_D_INDIRECT_PTRS * BLOCKSIZE * BLOCKSIZE / 16);
  char write_data='d';
  char read_data='a';
  uint32_t i;
  
  // fill in the file with characters
  for(i=0;i<max_size;i++){
    assert(rd_write(fp1, &write_data, 1)==1);
  }
  
  // Set file pointer to point to the beginning of the file
  assert(0==rd_lseek(fp1,0));
  
  // now read back the written data
  for(i=0;i<max_size;i++){
    assert(1==rd_read(fp1, &read_data, 1));
    assert(read_data==write_data);
  }
  
  /*** TEST 3: Creation of existing files and directories ***/
  // Tests: rd_mkdir, rd_creat
  assert(-1==rd_creat("/sys/class/devices/f3"));
  assert(-1==rd_mkdir("/sys/class/devices"));
  assert(-1==rd_mkdir("/etc"));
  
  /*** TEST 4: Accessing a closed and re-opened file ***/
  // Tests: rd_close, rd_read, rd_write, rd_lseek
  assert(0==rd_lseek(fp1,0));
  assert(rd_close(fp1)==0);
  assert(rd_read(fp1,&read_data,1)==-1);  // read/write should fail as the file was closed
  assert(rd_write(fp1,&read_data,1)==-1);
  
  fp1 = rd_open("/sys/class/devices/f1"); // re-open the file
  assert(rd_read(fp1,&read_data,1)==1);   // read/write should now go through without error
  assert(rd_write(fp1,&read_data,1)==1);
  assert(rd_close(fp1)==0);
  
  /*** TEST 5: Removing a file from the file system ***/
  // Tests: rd_unlink, rd_open 
  assert(-1==rd_unlink("/sys/class")); // unlink should fail as /sys/class has subdirectories
  assert(-1==rd_unlink("/nosuchfile"));
  assert(0==rd_unlink("/sys/class/devices/f5"));
  int fp4 = rd_open("/sys/class/devices/f5");
  assert(fp4==-1);                              // open should fail as file is removed
  assert(0==rd_creat("/sys/class/devices/f5")); // create the file again
  fp4 = rd_open("/sys/class/devices/f5");
  assert(fp4!=-1);                              // open should work now
  assert(0==rd_close(fp4));                     // close the file
  
  /*** TEST 6: Getting a list of subdirectories ***/
  // Tests: rd_readdir, rd_open, rd_close
  int f_root = rd_open("/");
  assert(f_root!=-1);

  char buffer[16]; 
  uint16_t a;
  // Testing wuth the subdirectories of the root folder
  assert(rd_readdir(f_root, buffer)==1); // reading sys folder (name null terminated)
  assert(strcmp("sys", buffer)==0);     // compare filename

  assert(rd_readdir(f_root, buffer)==1); // reading etc folder (name null terminated)
  assert(strcmp("etc", buffer)==0);      // compare filename
  
  assert(rd_readdir(f_root, buffer)==0); // should return 0 since reached end of entries
  assert(rd_close(f_root)==0);
  
  // testing with the subdirectories of the devices folder
  int dev_fp = rd_open("/sys/class/devices/");
  assert(dev_fp!=-1);
  
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f1", buffer)==0);        // compare filename
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f2", buffer)==0);        // compare filename
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f3", buffer)==0);        // compare filename
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f4", buffer)==0);        // compare filename
  assert(rd_readdir(dev_fp, buffer)==1);  // reading sys folder (name null terminated)
  assert(strcmp("f5", buffer)==0);        // compare filename
  assert(rd_readdir(dev_fp, buffer)==0); // should return 0 since reached end of entries
  assert(rd_close(dev_fp)==0);
  
  printf("File operations test passed.\n");
}

int main(){
  
  test_file_operations();

  return 0;
}
