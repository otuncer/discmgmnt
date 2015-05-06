#ifndef FILE_OPERATIONS_WRAPPERS_H
#define FILE_OPERATIONS_WRAPPERS_H

//Issues  IOCTL_CREAT command
int rd_creat(char* pathname);

//Issues  IOCTL_MKDIR command
int rd_mkdir(char* pathname);

//Issues  IOCTL_OPEN command
int rd_open(char* pathname);

//Issues  IOCTL_CLOSE command
int rd_close(int fd);

//Issues  IOCTL_READ command
int rd_read(int d, char* buffer, int num_bytes);

//Issues  IOCTL_WRITE commandcommand
int rd_write(int d, char* buffer, int num_bytes);

//Issues  IOCTL_LSEEK command
int rd_lseek(int fd, int offset);

//Issues  IOCTL_UNLINK command
int rd_unlink(char* pathname);

//Issues  IOCTL_READDIR command
int rd_readdir(int fd, char* buffer);

#endif
