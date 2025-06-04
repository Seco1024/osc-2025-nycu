#include "kernel/initramfs.h"

struct file_operations initramfs_file_operations = {initramfs_write, initramfs_read, initramfs_open, initramfs_close, initramfs_lseek64, initramfs_getsize};
struct vnode_operations initramfs_vnode_operations = {initramfs_lookup, initramfs_create, initramfs_mkdir};

int register_initramfs(void) {
    struct filesystem fs;
    fs.name = "initramfs";
    fs.setup_mount = initramfs_mount;
    return register_filesystem(&fs);
}

int initramfs_mount(struct filesystem *fs, struct mount *mount) {
    mount->fs = fs;
    mount->root = initramfs_create_vnode(0, DIR);
    struct inode *ramdir_inode = mount->root->internal;

    int idx = 0;
    file_list_t *files = ramfs_get_filelist();

    for (int i = 0; i < files->file_count; i++) {
        struct vnode *file_vnode = initramfs_create_vnode(0, FILE);
        struct inode *file_inode = file_vnode->internal;
        strcpy(file_inode->name, files->file_names[i]);
        file_inode->data = files->file_data[i];
        file_inode->datasize = files->file_sizes[i];
        ramdir_inode->entry[idx++] = file_vnode;
    }

    return 0;
}

struct vnode *initramfs_create_vnode(struct mount *mount, enum node_type type) {
    struct vnode *v = allocate(sizeof(struct vnode));
    v->f_ops = &initramfs_file_operations;
    v->v_ops = &initramfs_vnode_operations;
    v->mount = mount;
    struct inode *i = allocate(sizeof(struct inode));
    memset(i, 0, sizeof(struct inode));
    i->type = type;
    i->data = NULL;
    v->internal = i;
    return v;
}

int initramfs_write(struct file *file, const void *buf, size_t len) {
    return -1;
}

int initramfs_read(struct file *file, void *buf, size_t len) {
    struct inode *i = file->vnode->internal;
    if (len + file->f_pos > i->datasize) {
        memncpy(buf, i->data + file->f_pos, i->datasize - file->f_pos);
        file->f_pos += i->datasize - file->f_pos;
        return i->datasize - file->f_pos;
    } else {
        memncpy(buf, i->data + file->f_pos, len);
        file->f_pos += len;
        return len;
    }
    return -1;
}

int initramfs_open(struct vnode *file_node, struct file **target) {
    (*target)->vnode = file_node;
    (*target)->f_ops = file_node->f_ops;
    (*target)->f_pos = 0;
    return 0;
}

int initramfs_close(struct file *file) {
    free(file);
    return 0;
}

long initramfs_lseek64(struct file *file, long offset, int whence) {
    if (whence == SEEK_SET) {
        file->f_pos = offset;
        return file->f_pos;
    }
    return -1;
}

int initramfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name) {
    struct inode *dir_inode = dir_node->internal;
    int child_idx = 0;
    for (; child_idx < MAX_DIR_ENTRY; child_idx++) {
        struct vnode *v = dir_inode->entry[child_idx];
        if (!v) break;
        struct inode *i = v->internal;
        if (strcmp(component_name, i->name) == 0) {
            *target = v;
            return 0;
        }
    }
    return -1;
}

int initramfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name) {
    return -1;
}

int initramfs_mkdir(struct vnode *dir_node, struct vnode **target, const char *component_name) {
    return -1;
}

long initramfs_getsize(struct vnode *vd) {
    struct inode *i = vd->internal;
    return i->datasize;
}
