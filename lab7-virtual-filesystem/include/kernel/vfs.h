#ifndef __VFS_
#define __VFS_

#include "kernel/fdt.h"
#include "lib/stdlib.h"

struct file;        

#define O_CREAT 100
#define MAX_PATH_NAME 255
#define MAX_FILE_NAME_SIZE 15
#define MAX_DIR_ENTRY 16
#define MAX_FILE_SIZE 4096
#define SEEK_SET 0
#define MAX_FS_REG 0x50
#define MAX_DEV_REG 0x10
#define O_RDWR  000

enum node_type
{
  DIR,
  FILE
};

struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
};

struct mount {
  struct vnode* root;
  struct filesystem* fs;
};

struct filesystem {
  const char* name;
  int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
  int (*write)(struct file* file, const void* buf, size_t len);
  int (*read)(struct file* file, void* buf, size_t len);
  int (*open)(struct vnode* file_node, struct file** target);
  int (*close)(struct file* file);
  long (*lseek64)(struct file* file, long offset, int whence);
  long (*getsize)(struct vnode *v);
};

struct vnode_operations {
  int (*lookup)(struct vnode* dir_node, struct vnode** target,
                const char* component_name);
  int (*create)(struct vnode* dir_node, struct vnode** target,
                const char* component_name);
  int (*mkdir)(struct vnode* dir_node, struct vnode** target,
              const char* component_name);
};

void init_rootfs(void);
int register_filesystem(struct filesystem *fs);
int vfs_open(const char *pathname, int flags, struct file **target);
int vfs_close(struct file *file);
int vfs_write(struct file *file, const void *buf, size_t len);
int vfs_read(struct file *file, void *buf, size_t len);
int vfs_lookup(const char *pathname, struct vnode **target);
int vfs_mkdir(const char *pathname);
int vfs_mount(const char *target, const char *filesystem);
struct filesystem *find_filesystem(const char *fs_name);
char *get_absolute_path(char *path, char *cwd);

#endif