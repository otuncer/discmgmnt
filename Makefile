obj-m += ramdisk_module.o 
ramdisk_module-objs := 	kernel_src/ramdisk_module.o \
												kernel_src/block.o \
												kernel_src/file_desc_table.o \
												kernel_src/file_operations.o \
												kernel_src/inode.o 

all:
	make -C /usr/src/linux-$(shell uname -r) SUBDIRS=$(PWD) modules
	insmod ramdisk_module.ko

demo:
	gcc -Wall -o ramdisk_demo ramdisk_test.c file_operations_wrappers.c

clean:
	make -C /usr/src/linux-$(shell uname -r) SUBDIRS=$(PWD) clean
	rmmod ramdisk_module
	rm ramdisk_demo 