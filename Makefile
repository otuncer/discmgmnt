obj-m += ramdisk_module.o 
ramdisk-objs := block.o \
								file_desc_table.o \
								file_operations.o \
								inode.o 

all:
	make -C /usr/src/linux-$(shell uname -r) SUBDIRS=$(PWD) modules
#	sudo insmod ramdisk_module.ko

#demo:
#	$(CC) -Wl,-wrap,getchar -o keyboard_demo keyboard_test.c
#	./keyboard_demo

clean:
	rmmod ramdisk_module
	make -C /usr/src/linux-$(shell uname -r) SUBDIRS=$(PWD) clean
	rm ./ramdisk_module