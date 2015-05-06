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
#include <linux/mutex.h>

#include "file_operations.h"

MODULE_LICENSE("GPL");

static DEFINE_MUTEX(lock);

/** PROCFS GLOBAL VARS **/
static struct proc_dir_entry *proc_entry;
static struct file_operations pseudo_dev_proc_operations;

/** PROCFS FUNCTIONS **/
static int pseudo_device_ioctl(struct inode *inode, struct file *file,
			       unsigned int cmd, unsigned long arg);

/***** RAMDISK GLOBAL VARS ******/

char* ramdisk_mem;

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
  proc_entry->proc_fops = &pseudo_dev_proc_operations;
	// Allocate RAMDISK memory area
	if( (ramdisk_mem = vmalloc(PARTITION_SIZE)) == NULL){
    return -ENOMEM;
  }

  //initialize global pointers
  super_block = (super_block_t*) &ramdisk_mem[0];
  inode_initialize(&ramdisk_mem[BLOCKSIZE], super_block);
  block_initialize(&ramdisk_mem[BLOCKSIZE*(1+NUM_INODES/4)+NUM_BLOCKS/8],//blocks
                   &ramdisk_mem[BLOCKSIZE*(1+NUM_INODES/4)], //bit-map ptr
                   super_block);
  
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
  
  if(copy_from_user(&user_args, (discos_arguments_t*)arg, sizeof(discos_arguments_t))){
    return -EFAULT;
  }
  
  mutex_lock(&lock);
  
  switch (cmd){
  case IOCTL_CLOSE:
    user_args.arg_return = rd_close(user_args.arg_int_first);
    break;
  case IOCTL_CREAT:
    user_args.arg_return = rd_creat(user_args.arg_char);
    break;
  case IOCTL_LSEEK:
    user_args.arg_return = rd_lseek(user_args.arg_int_first, user_args.arg_int_second);
    break;
  case IOCTL_MKDIR:
    user_args.arg_return = rd_mkdir(user_args.arg_char);
    break;
  case IOCTL_OPEN:
    user_args.arg_return = rd_open(user_args.arg_char);
    break;
  case IOCTL_READ:
    user_args.arg_return = rd_read(user_args.arg_int_first, user_args.arg_char, user_args.arg_int_second);
    break;
  case IOCTL_READDIR:
    user_args.arg_return = rd_readdir(user_args.arg_int_first, user_args.arg_char);
    break;
  case IOCTL_UNLINK: 
    user_args.arg_return = rd_unlink(user_args.arg_char);
    break;
  case IOCTL_WRITE:
    user_args.arg_return = rd_write(user_args.arg_int_first, user_args.arg_char, user_args.arg_int_second);
    break;
  default:
    mutex_unlock(&lock);
    return -EINVAL;
    break;
  }
  
  if(copy_to_user((discos_arguments_t*)arg, &user_args, sizeof(discos_arguments_t))){
    mutex_unlock(&lock);
    return -EFAULT;
	}
  
  mutex_unlock(&lock);
  return 0;
}

module_init(initialization_routine);
module_exit(cleanup_routine);
