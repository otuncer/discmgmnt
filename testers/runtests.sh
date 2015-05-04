#!/bin/bash

#get base folder
cur_folder=${PWD##*/}
if [ $cur_folder == testers ] ; then
  base="../"
else
  base=""
fi

gcc -DDEBUG ${base}testers/file_desc_test.c ${base}file_desc_table.c
./a.out
rm a.out

gcc -DDEBUG ${base}testers/block_test.c ${base}block.c
./a.out
rm a.out

# TODO extend remove entry test for ...
#  ...non-empty directory, file blocks, root directory
gcc -DDEBUG ${base}testers/inode_test.c ${base}inode.c ${base}block.c
./a.out
rm a.out

