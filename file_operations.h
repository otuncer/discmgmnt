//TODO implement rd_creat
//create a regular file. Returns 0 on success, -1 on error.
int rd_creat(char* pathname);

//TODO implement rd_mkdir
//creates a directory. Returns 0 on success, -1 on error.
int rd_mkdir(char* pathname);

//TODO implement rd_open
//opens an existing file or directory.
//Returns file descriptor, or -1 if the file/folder does not exist.
int rd_open(char* pathname);

//TODO implement rd_close
//close give file descriptor. 
//Returns 0 if successful, or -1 if fd does not exist
int rd_close(int fd);

//TODO implement rd_read
//read up to num_bytes from regular file identified by fd
//returns number of bytes actually read, 
//  or -1 if fd does not exist or refers to a directory
int rd_read(int d, char* buffer, int num_bytes);

//TODO implement rd_write
//write up to num_bytes to regular file identified by fd
//returns number of bytes actually written, 
//  or -1 if fd does not exist or refers to a directory
int rd_write(int d, char* buffer, int num_bytes);

//TODO implement rd_seek
//set file's position identified by file descriptor.
//returns new position, or the end of the file position if offset > size
// returns -1 if applied to directory files
int rd_seek(int fd, int offset);

//TODO implement rd_unlink
//remove given file or directory, freeing its memory
//returns 0 on success or -1 if (1) pathname does not exist, (2) attempt to
// unlink non-empty directory, (3) unlink open file, (4) attempt to remove root
int rd_unlink(char* pathname);

//TODO implement rd_readdir
//read one entry from directory file into buffer
//returns 1 on success, or -1 if fd is not a directory, or 0 at end-of-file
int rd_readdir(int fd, char* buffer);
