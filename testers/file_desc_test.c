#include <assert.h>
#include <stdio.h>
#include "../file_desc_table.h"

int main()
{
  file_desc_table = NULL;
  assert(file_add_desc(0) == 0);
  assert(file_add_desc(1) == 1);
  assert(file_add_desc(2) == 2);
  assert(file_get_fd(0)->inode == 0);
  assert(file_get_fd(1)->inode == 1);
  assert(file_get_fd(2)->inode == 2);
  assert(file_remove_fd(2) == 1);
  assert(file_remove_fd(2) == 0);
  assert(file_remove_fd(0) == 1);
  assert(file_add_desc(3) == 0);
  assert(file_add_desc(4) == 2);
  assert(file_remove_fd(3) == 0);
  printf("File desctriptor test passed.\n");
  return 0;
}
