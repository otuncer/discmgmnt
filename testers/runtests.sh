#!/bin/bash

#get base folder
cur_folder=${PWD##*/}
if [ $cur_folder == testers ] ; then
  base="../"
else
  base=""
fi

gcc ${base}testers/file_desc_test.c ${base}file_desc_table.c
./a.out
rm a.out
