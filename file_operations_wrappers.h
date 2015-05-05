#ifndef FILE_OPERATIONS_WRAPPERS_H
#define FILE_OPERATIONS_WRAPPERS_H

// TODO: rd_creat wrapper 
//Issues  IOCTL_CREAT command
int rd_creat(char* pathname);

// TODO: rd_mkdir wrapper 
//Issues  IOCTL_MKDIR command
int rd_mkdir(char* pathname);

// TODO: rd_open wrapper 
//Issues  IOCTL_OPEN command
int rd_open(char* pathname);

// TODO: rd_close wrapper 
//Issues  IOCTL_CLOSE command
int rd_close(int fd);

// TODO: rd_read wrapper 
//Issues  IOCTL_READ command
int rd_read(int d, char* buffer, int num_bytes);

// TODO: rd_write wrapper 
//Issues  IOCTL_WRITE commandcommand
int rd_write(int d, char* buffer, int num_bytes);

// TODO: rd_lseek wrapper 
//Issues  IOCTL_LSEEK command
int rd_lseek(int fd, int offset);

// TODO: rd_unlink wrapper 
//Issues  IOCTL_UNLINK command
int rd_unlink(char* pathname);

// TODO: rd_readdir wrapper 
//Issues  IOCTL_READDIR command
int rd_readdir(int fd, char* buffer);

#endif
