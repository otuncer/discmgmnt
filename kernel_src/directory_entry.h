#ifndef DIRENTRY_H
#define DIRENTRY_H

#include "constants.h"

#ifdef DEBUG
#include <stdint.h>
#else
#include <linux/types.h>
#endif

/** Directory Entry **/
typedef struct _directory_entry_t{ 
  char filename[FILENAME_SIZE];
  uint16_t index_node_number;
}directory_entry_t;

#endif
