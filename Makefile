obj-m += ramdisk_module.o 
ramdisk_module-objs := block.o \
								file_desc_table.o \
								file_operations.o \
								inode.o 

all:
	make -C /usr/src/linux-$(shell uname -r) SUBDIRS=$(PWD) modules
  insmod ramdisk_module.ko

demo:
	$(CC) -Wall -o ramdisk_demo testers/ramdisk_test.c file_operations_wrappers.c
#	./ramdisk_demo

clean:
	make -C /usr/src/linux-$(shell uname -r) SUBDIRS=$(PWD) clean
	rmmod ramdisk_module
	