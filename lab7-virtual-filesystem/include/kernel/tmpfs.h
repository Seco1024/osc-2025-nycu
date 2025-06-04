#ifndef __TMPFS_
#define __TMPFS_

#include "kernel/vfs.h"

struct inode {
    enum node_type type;
    char name[MAX_FILE_NAME_SIZE];
    struct vnode *entry[MAX_DIR_ENTRY];
    char *data;
    size_t datasize;
};

int register_tmpfs(void);
int tmpfs_mount(struct filesystem *fs, struct mount *mount);
int tmpfs_write(struct file *file, const void *buf, size_t len);
int tmpfs_read(struct file *file, void *buf, size_t len);
int tmpfs_open(struct vnode *file_node, struct file **target);
int tmpfs_close(struct file *file);
long tmpfs_lseek64(struct file *file, long offset, int whence);
long tmpfs_getsize(struct vnode *v);
int tmpfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int tmpfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
int tmpfs_mkdir(struct vnode *dir_node, struct vnode **target, const char *component_name);
struct vnode *tmpfs_create_vnode(struct mount *mount, enum node_type type);

#endif