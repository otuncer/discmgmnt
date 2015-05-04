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

#include "file_operations.h"

#define KILOBYTES 1024
#define MEGABYTES (KILOBYTES*1024)

MODULE_LICENSE("GPL");

/************************/
/** PROCFS GLOBAL VARS **/
/************************/
static struct file_operations pseudo_dev_proc_operations;
static struct proc_dir_entry *proc_entry;

inode_t* inode_head;
uint8_t* block_bitmap;
block_t* blocks;

/************************/
/***** INIT MODULE ******/
/************************/
static int __init initialization_routine(void) {
  printk("<1> Loading RAMDISK\n");

  /* Create RAMDISK /proc entry */
  proc_entry = create_proc_entry("ramdisk", 0444, NULL);
  if(!proc_entry){
      printk("<1> Error creating /proc entry.\n");
      return -ENOMEM;
  }

	/* Allocate RAMDISK memory area */
	char* ramdisk_mem = vmalloc(2*MEGABYTES);

  proc_entry->proc_fops = &pseudo_dev_proc_operations;
	
  return 0;
}

static void __exit cleanup_routine(void) {
  
  remove_proc_entry("ramdisk", NULL);

	/* Deallocate RAMDISK memory */
	vfree(ramdisk_mem);
	// Persistency?

  printk("<1> RAMDISK unloded\n");
  return;
}

module_init(initialization_routine);
module_exit(cleanup_routine);
