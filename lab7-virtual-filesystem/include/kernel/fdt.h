#ifndef __FDT_
#define __FDT_

#include "kernel/vfs.h"
#include "lib/stdlib.h"

#define MAX_FD 15

struct file {
  struct vnode* vnode;
  size_t f_pos;  // RW position of this file handle
  struct file_operations* f_ops;
  int flags;
};

struct fdtable {
  int count;
  struct file* fds[MAX_FD];
};

#endif