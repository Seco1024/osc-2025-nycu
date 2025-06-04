#include "kernel/vfs.h"
#include "kernel/tmpfs.h"
#include "kernel/fdt.h"
#include "kernel/kmalloc.h"
#include "lib/string.h"
#include "lib/io.h"

struct file_operations tmpfs_file_operations = {tmpfs_write,   tmpfs_read, tmpfs_open, tmpfs_close, tmpfs_lseek64, tmpfs_getsize};
struct vnode_operations tmpfs_vnode_operations = {tmpfs_lookup, tmpfs_create, tmpfs_mkdir};

int register_tmpfs(void) {
    struct filesystem fs;
    fs.name = "tmpfs";
    fs.setup_mount = tmpfs_mount;
    return register_filesystem(&fs);
}

int tmpfs_mount(struct filesystem *fs, struct mount *mount) {
    mount->fs = fs;
    mount->root = tmpfs_create_vnode(0, DIR);
    return 0;
}

struct vnode *tmpfs_create_vnode(struct mount *mount, enum node_type type) {
    struct vnode *v = allocate(sizeof(struct vnode));
    struct inode *i = allocate(sizeof(struct inode));
    memset(v, 0, sizeof(struct vnode));
    memset(i, 0, sizeof(struct inode));

    v->f_ops = &tmpfs_file_operations;
    v->v_ops = &tmpfs_vnode_operations;
    v->mount = 0;

    i->type = type;
    i->data = allocate(MAX_FILE_SIZE);

    v->internal = i;
    return v;
}

int tmpfs_write(struct file *file, const void *buf, size_t len) {
    struct inode *i = file->vnode->internal;
    memncpy(i->data + file->f_pos, buf, len);
    file->f_pos += len;
    if (i->datasize < file->f_pos) {
        i->datasize = file->f_pos;
    }

    return len;
}

int tmpfs_read(struct file *file, void *buf, size_t len) {
    struct inode *i = file->vnode->internal;

    if (len + file->f_pos > i->datasize) {
        len = i->datasize - file->f_pos;
        memncpy(buf, i->data + file->f_pos, len);
        file->f_pos += i->datasize - file->f_pos;
        return len;
    } else {
        memncpy(buf, i->data + file->f_pos, len);
        file->f_pos += len;
        return len;
    }
    return -1;
}

int tmpfs_open(struct vnode *file_node, struct file **target) {
    (*target)->vnode = file_node;
    (*target)->f_ops = file_node->f_ops;
    (*target)->f_pos = 0;
    return 0;
}

int tmpfs_close(struct file *file) {
    free(file);
    return 0;
}

long tmpfs_lseek64(struct file *file, long offset, int whence) {
    if (whence == SEEK_SET) {
        file->f_pos = offset;
        return file->f_pos;
    }
    return -1;
}

long tmpfs_getsize(struct vnode *v) {
    struct inode *i = v->internal;
    return i->datasize;
}

int tmpfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name) {
    struct inode *dir_inode = dir_node->internal;
    int idx = 0;

    for (; idx <= MAX_DIR_ENTRY; ++idx) {
        struct vnode *v = dir_inode->entry[idx];
        if (!v) break;
        struct inode *i = v->internal;
        if (strcmp(component_name, i->name) == 0) {
            *target = v;
            return 0;
        }
    }
    return -1;
}

int tmpfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name) {
    struct inode *i = dir_node->internal;
    if (i->type != DIR) {
        return -1;
    }

    int idx = 0;
    for (; idx <= MAX_DIR_ENTRY; ++idx) {
        if (!i->entry[idx]) break;
        struct inode *child_inode = i->entry[idx]->internal;
        if (strcmp(child_inode->name, component_name) == 0) {
            puts("tmpfs create file exists\r\n");
            return -1;
        }
    }

    if (idx > MAX_DIR_ENTRY) {
        puts("directory entry full\r\n");
        return -1;
    }

    if (strlen(component_name) > MAX_FILE_NAME_SIZE) {
        puts("file name is too long\r\n");
        return -1;
    }

    struct vnode *v = tmpfs_create_vnode(0, FILE);
    i->entry[idx] = v;

    struct inode *new_i = v->internal;
    strcpy(new_i->name, component_name);

    *target = v;
    return 0;
}

int tmpfs_mkdir(struct vnode *dir_node, struct vnode **target, const char *component_name) {
    struct inode *i = dir_node->internal;

    if (i->type != DIR) {
        return -1;
    }

    int idx = 0;
    for (; idx <= MAX_DIR_ENTRY; ++idx) {
        if (!i->entry[idx]) {
            break;
        }
    }

    if (idx > MAX_DIR_ENTRY) {
        puts("directory entry full\r\n");
        return -1;
    }

    if (strlen(component_name) > MAX_FILE_NAME_SIZE) {
        puts("file name is too long\r\n");
        return -1;
    }

    struct vnode *v = tmpfs_create_vnode(0, DIR);
    i->entry[idx] = v;

    struct inode *new_i = v->internal;
    strcpy(new_i->name, component_name);

    *target = v;
    return 0;
}