/** Overview **/
In our ramdisk file system, the details of the ioctl calls are hidden from the user by the wrapper functions listed under file_operations_wrappers.h. The file system can be utilized by including this header file and kernel_src/constants.h, which defines the ramdisk constants. The user-level program can than directly issue the rd_creat, rd_open etc. functions as listed in the assignment.

/** HOW TO BUILD **/
The ramdisk kernel module can be built by issuing "make". The Makefile automatically inserts the kernel module into the system and removes it back after issuing a "make clean" command. The kernel level source files are located under the kernel_src directory.

/** TESTING **/
We have tested our ramdisk using both the provided and our own tester codes. We provide a script (mytest.sh) that handles all the compilation and the testing process. It initially runs our own custom test code (mytest.c) and then the provided ramdisk_test.c. In order to run the tests, simply use:

./mytest.sh

The custom test code verifies the correct functionality of all the file operations using "assert" statements (details provided as comments in mytest.c). We also used a set of unit tests to stress different parts of the project (inode, blocks, file descriptor) individually. Those tests are located under the kernel_src/testers/ directory and can be run by simply issuing:

./kernel_src/testers/runtests.sh 

