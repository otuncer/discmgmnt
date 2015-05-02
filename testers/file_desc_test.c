#include <assert.h>
#include <stdio.h>
#include "../file_desc_table.h"

int main()
{
  file_desc_table = NULL;
  assert(add_file_desc(0) == 0);
  assert(add_file_desc(1) == 1);
  assert(add_file_desc(2) == 2);
  assert(get_fd(0)->inode == 0);
  assert(get_fd(1)->inode == 1);
  assert(get_fd(2)->inode == 2);
  assert(remove_fd(2) == 1);
  assert(remove_fd(2) == 0);
  assert(remove_fd(0) == 1);
  assert(add_file_desc(3) == 0);
  assert(add_file_desc(4) == 2);
  assert(remove_fd(3) == 0);
  printf("File desctriptor test passed.\n");
  return 0;
}
