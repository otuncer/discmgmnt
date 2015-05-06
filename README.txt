CS552 - Operating Systems
DISCOS Assignment

/** TEAM MEMBERS **/
Ozan Tuncer, otuncer@bu.edu
Onur Sahin, sahin@bu.edu

/** Overview **/
In our ramdisk file system, we chose to follow the Linux kernel implmentation apprach. The ramdisk file system is implemented as a kernel module which is interfaced by a set of ioctl commands through /proc/ramdisk entry. Details of the ioctl calls are hidden to the user using the wrapper functions listed under file_operations_wrappers.h. The file system can be utilized by including this header file (and compiling its souce file counterpart) a the user-level program and issuing the rd_creat, rd_open etc. functions as listed in the assignment.

/** HOW TO BUILD **/
The kernel level source files are populated under the kernel_src folder. The ramdisk kernel module can be build by using the provided Makefile, issuing a "make" call. The Makefile automatically inserts the kernel module into the ssystem and removes it back after issuing a "make clean" command.  

/** TESTING **/
We have tested our ramdisk implementation using both the provided and our own tester codes. We provide a script (mytest.sh) that handles all the compilation and the testing process. It initially runs our own custom test code (mytest.c) and then the provided ramdisk_test.c tst code. In order to run the tests, simply use:

./mytest.sh

The custom test code makes use of the "assert" statements to verify the correct functionality of all the file operations. We provide our comments within this file to explain details of each test case. The program successfully completes once all the operations generate the correct results as checked by the assert statements within the code. We also used a set of unit tests to stress different parts of the project (inode, blocks, file descriptor) individually. Those tests can be found under the kernel_src/testers/ folder and could be run by simply issuing:

./kernel_src/testers/runtests.sh 

