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

//#include "block.h"
//#include "file_operations.h"
//#include "inode.h"

MODULE_LICENSE("GPL");

/** PROCFS GLOBAL VARS **/
static struct proc_dir_entry *proc_entry;
static struct file_operations pseudo_dev_proc_operations;

/** PROCFS FUNCTIONS **/
static int pseudo_device_ioctl(struct inode *inode, struct file *file,
			       unsigned int cmd, unsigned long arg);

/***** RAMDISK GLOBAL VARS ******/

char* ramdisk_mem;
//inode_t* inode_head;
//uint8_t* block_bitmap;
//block_t* blocks;
//super_block_t* super_block;

/***** INIT MODULE ******/
static int __init initialization_routine(void) {
  printk("<1> Loading RAMDISK\n");

  /* Create RAMDISK /proc entry */
  proc_entry = create_proc_entry("ramdisk", 0444, NULL);
  if(!proc_entry){
    printk("<1> Error creating /proc entry.\n");
    return -ENOMEM;
  }
  proc_entry->proc_fops = &pseudo_dev_proc_operations;
  pseudo_dev_proc_operations.ioctl = pseudo_device_ioctl;
  
	// Allocate RAMDISK memory area
	if( (ramdisk_mem = vmalloc(1)) == NULL){
    return -ENOMEM;
  }
  #if 0
  //initialize global pointers
  super_block = (super_block_t*) &ramdisk_mem[0];
  inode_initialize(&ramdisk_mem[BLOCKSIZE], super_block);
  block_initialize(&ramdisk_mem[BLOCKSIZE*(1+NUM_INODES/4)+NUM_BLOCKS/8],//blocks
                   &ramdisk_mem[BLOCKSIZE*(1+NUM_INODES/4)], //bit-map ptr
                   super_block);
  #endif
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
  discos_arguments_t user_args;
  
  switch (cmd){

  case IOCTL_CLOSE:
    ;
    if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
      return -EFAULT;
    }
    // Call corresponding function
    if(copy_to_user((discos_arguments_t*)arg, &user_args, sizeof(discos_arguments_t)))
      return -EFAULT;
    break;
  case IOCTL_CREAT:
    ;
    //if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
    //  return -EFAULT;
    //}
    // Call corresponding function
    //if(copy_to_user((discos_arguments_t*)arg, &user_args, sizeof(discos_arguments_t)))
    //  return -EFAULT;
    printk(KERN_ALERT "In creat\n");
    break;
  case IOCTL_LSEEK:
    ;
    if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
      return -EFAULT;
    }
    // Call corresponding function
    if(copy_to_user((discos_arguments_t*)arg, &user_args, sizeof(discos_arguments_t)))
      return -EFAULT;
    break;
  case IOCTL_MKDIR:
    ;
    if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
      return -EFAULT;
    }
    // Call corresponding function
    if(copy_to_user((discos_arguments_t*)arg, &user_args, sizeof(discos_arguments_t)))
      return -EFAULT;
    break;
  case IOCTL_OPEN:
    ;
    if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
      return -EFAULT;
    }
    // Call corresponding function
    if(copy_to_user((discos_arguments_t*)arg, &user_args, sizeof(discos_arguments_t)))
      return -EFAULT;
    break;
  case IOCTL_READ:
    ;
    if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
      return -EFAULT;
    }
    // Call corresponding function
    if(copy_to_user((discos_arguments_t*)arg, &user_args, sizeof(discos_arguments_t)))
      return -EFAULT;
    break;
  case IOCTL_READDIR:
    ;
    if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
      return -EFAULT;
    }
    break;
  case IOCTL_UNLINK:
    ;
    if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
      return -EFAULT;
    }
    // Call corresponding function
    if(copy_to_user((discos_arguments_t*)arg, &user_args, sizeof(discos_arguments_t)))
      return -EFAULT;
    break;
  case IOCTL_WRITE:
    ;
    if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
      return -EFAULT;
    }
    // Call corresponding function
    if(copy_to_user((discos_arguments_t*)arg, &user_args, sizeof(discos_arguments_t)))
      return -EFAULT;
    break;
  default:
    return -EINVAL;
    break;
  }
  
  return 0;
}


module_init(initialization_routine);
module_exit(cleanup_routine);
