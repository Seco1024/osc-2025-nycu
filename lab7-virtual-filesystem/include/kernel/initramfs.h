#ifndef __RAMFS_
#define __RAMFS_
#include "kernel/tmpfs.h"
#include "kernel/cpio.h"
#include "lib/io.h"
#include "kernel/vfs.h"
#include "lib/stdlib.h"
#include "kernel/kmalloc.h"
#include "lib/string.h"
#include "kernel/fdt.h"

int register_initramfs(void);
int initramfs_mount(struct filesystem *fs, struct mount *mount);
struct vnode *initramfs_create_vnode(struct mount *mount, enum node_type type);
int initramfs_write(struct file *file, const void *buf, size_t len);
int initramfs_read(struct file *file, void *buf, size_t len);
int initramfs_open(struct vnode *file_node, struct file **target);
int initramfs_close(struct file *file);
long initramfs_lseek64(struct file *file, long offset, int whence);
int initramfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int initramfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
int initramfs_mkdir(struct vnode *dir_node, struct vnode **target, const char *component_name);
long initramfs_getsize(struct vnode *vd);

#endif