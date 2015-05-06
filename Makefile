obj-m += ramdisk_module.o 
ramdisk_module-objs := block.o \
								file_desc_table.o \
								file_operations.o \
								inode.o 

all:
	make -C /usr/src/linux-$(shell uname -r) SUBDIRS=$(PWD) modules
#	insmod ramdisk_module.ko

#demo:
#	$(CC) -Wl,-wrap,getchar -o keyboard_demo keyboard_test.c
#	./keyboard_demo

clean:
	make -C /usr/src/linux-$(shell uname -r) SUBDIRS=$(PWD) clean
	rmmod ramdisk_module
	#rm ./ramdisk_module