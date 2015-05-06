#!/bin/bash

# Prepare ramdisk module
echo "Removing module"
make clean > /dev/null 2>&1
make
make demo

# Compile the test code
gcc -DDEBUG -g file_operations_wrappers.c mytest.c

# Run the tests
echo ""
echo "Running the custom test file"
./a.out
rmmod ramdisk_module

insmod ramdisk_module.ko
echo ""
echo "Running the given test file"
./ramdisk_demo 
make clean > /dev/null 2>&1
# Clear workspace
rm a.out
