/*
 *  RAMDISK procfs interface
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h> /* error codes */
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/vmalloc.h>

#include "constants.h"
#include "file_operations.h"

MODULE_LICENSE("GPL");

/** PROCFS GLOBAL VARS **/
static struct proc_dir_entry *proc_entry;
static struct file_operations pseudo_dev_proc_operations;

/** PROCFS FUNCTIONS **/
static int pseudo_device_ioctl(struct inode *inode, struct file *file,
			       unsigned int cmd, unsigned long arg);

/***** RAMDISK GLOBAL VARS ******/

char* ramdisk_mem;
inode_t* inode_head;
uint8_t* block_bitmap;
block_t* blocks;
super_block_t* super_block;

/***** INIT MODULE ******/
static int __init initialization_routine(void) {
  printk("<1> Loading RAMDISK\n");

  /* Create RAMDISK /proc entry */
  proc_entry = create_proc_entry("ramdisk", 0444, NULL);
  if(!proc_entry){
    printk("<1> Error creating /proc entry.\n");
    return -ENOMEM;
  }
  pseudo_dev_proc_operations.ioctl = pseudo_device_ioctl;

	/* Allocate RAMDISK memory area */
	if( (ramdisk_mem = vmalloc(PARTITION_SIZE)) == NULL){
    return -ENOMEM;
  }

  proc_entry->proc_fops = &pseudo_dev_proc_operations;
	
  return 0;
}

/***** REMOVE MODULE ******/
static void __exit cleanup_routine(void) {
  
  remove_proc_entry("ramdisk", NULL);

	/* Deallocate RAMDISK memory */
	vfree(ramdisk_mem);
	// Persistency?

  printk("<1> RAMDISK unloded\n");
  return;
}

static int pseudo_device_ioctl(struct inode *inode, struct file *file,
                               unsigned int cmd, unsigned long arg)
{
  switch (cmd){

  //case IOCTL_KEYBOARD:
  
  
  //  break;
  default:
    return -EINVAL;
    break;
  }
  
  return 0;
}


module_init(initialization_routine);
module_exit(cleanup_routine);
