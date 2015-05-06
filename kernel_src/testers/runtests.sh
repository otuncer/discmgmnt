#!/bin/bash

#get base folder
cur_folder=${PWD##*/}
if [ $cur_folder == kernel_src ] ; then
	base="../../"
elif [ $cur_folder == testers ] ; then
  base="../"
else
  base=""
fi

gcc -DDEBUG -g ${base}kernel_src/testers/file_desc_test.c ${base}kernel_src/file_desc_table.c
./a.out
rm a.out

gcc -DDEBUG -g ${base}kernel_src/testers/block_test.c ${base}kernel_src/block.c
./a.out
rm a.out

gcc -DDEBUG -g ${base}kernel_src/testers/inode_test.c ${base}kernel_src/inode.c ${base}kernel_src/block.c
./a.out
rm a.out

gcc -DDEBUG -g \
    ${base}kernel_src/testers/file_operations_test.c \
    ${base}kernel_src/file_operations.c \
    ${base}kernel_src/inode.c \
    ${base}kernel_src/block.c \
    ${base}kernel_src/file_desc_table.c
./a.out
rm a.out



