#include "kernel/vfs.h"
#include "kernel/tmpfs.h"
#include "kernel/kmalloc.h"
#include "kernel/initramfs.h"
#include "lib/string.h"
#include "lib/io.h"

struct mount *rootfs;
struct filesystem registered_fs[MAX_FS_REG];
struct file_operations registered_dev[MAX_DEV_REG];

void init_rootfs(void) {
    int idx = register_tmpfs();
    rootfs = allocate(sizeof(struct mount));
    registered_fs[idx].setup_mount(&registered_fs[idx], rootfs);

    vfs_mkdir("/initramfs");
    register_initramfs();
    vfs_mount("/initramfs", "initramfs");
}

int register_filesystem(struct filesystem *fs) {
    for (int i = 0; i < MAX_FS_REG; i++) {
        if (!registered_fs[i].name) {
            registered_fs[i].name = fs->name;
            registered_fs[i].setup_mount = fs->setup_mount;
            return i;
        }
    }
    return -1;
}

int vfs_open(const char *pathname, int flags, struct file **target) {
    struct vnode *node;
    if (vfs_lookup(pathname, &node) != 0 && (flags & O_CREAT)) {
        int last_slash_idx = 0;
        for (int i = 0; i < strlen(pathname); i++) {
            if (pathname[i] == '/') {
                last_slash_idx = i;
            }
        }

        char dirname[MAX_PATH_NAME + 1];
        strcpy(dirname, pathname);
        dirname[last_slash_idx] = 0;

        if (vfs_lookup(dirname, &node) != 0) {
            puts("cannot create no dirname\r\n");
            return -1;
        }

        node->v_ops->create(node, &node, pathname + last_slash_idx + 1);
        *target = allocate(sizeof(struct file));
        node->f_ops->open(node, target);
        (*target)->flags = flags;
        return 0;
    }

    *target = allocate(sizeof(struct file));
    node->f_ops->open(node, target);
    (*target)->flags = flags;
    return 0;
}

int vfs_close(struct file *file) {
    file->f_ops->close(file);
    return 0;
}

int vfs_write(struct file *file, const void *buf, size_t len) {
    return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file *file, void *buf, size_t len) {
    return file->f_ops->read(file, buf, len);
}

int vfs_lookup(const char *pathname, struct vnode **target) {
    if (strlen(pathname) == 0) {
        *target = rootfs->root;
        return 0;
    }

    struct vnode *dirnode = rootfs->root;
    char component_name[MAX_FILE_NAME_SIZE + 1] = {};
    int c_idx = 0;
    
    for (int i = 1; i < strlen(pathname); i++) {
        if (pathname[i] == '/') {
            component_name[c_idx++] = 0;
            if (dirnode->v_ops->lookup(dirnode, &dirnode, component_name) != 0)
                return -1;
                
            while (dirnode->mount) {
                dirnode = dirnode->mount->root;
            }
            c_idx = 0;
        } else {
            component_name[c_idx++] = pathname[i];
        }
    }

    component_name[c_idx++] = 0;
    if (dirnode->v_ops->lookup(dirnode, &dirnode, component_name) != 0)
        return -1;
    while (dirnode->mount) {
        dirnode = dirnode->mount->root;
    }

    *target = dirnode;
    return 0;
}

int vfs_mkdir(const char *pathname) {
    char dirname[MAX_PATH_NAME] = {}; 
    char newdirname[MAX_PATH_NAME] = {};  

    int last_slash_idx = 0;
    for (int i = 0; i < strlen(pathname); i++) {
        if (pathname[i] == '/') {
            last_slash_idx = i;
        }
    }

    memncpy(dirname, pathname, last_slash_idx);
    strcpy(newdirname, pathname + last_slash_idx + 1);

    struct vnode *node;
    if (vfs_lookup(dirname, &node) == 0) {
        node->v_ops->mkdir(node, &node, newdirname);
        return 0;
    }

    puts("mkdir cannot find pathname");
    return -1;
}

int vfs_mount(const char *target, const char *filesystem) {
    struct filesystem *fs = find_filesystem(filesystem);
    if (!fs) {
        puts("cannot find filesystem\r\n");
        return -1;
    }

    struct vnode *node;
    if (vfs_lookup(target, &node) == -1) {
        puts("cannot find dir\r\n");
        return -1;
    } else {
        node->mount = allocate(sizeof(struct mount));
        fs->setup_mount(fs, node->mount);
    }
    return 0;
}

struct filesystem *find_filesystem(const char *fs_name) {
    for (int i = 0; i < MAX_FS_REG; i++) {
        if (strcmp(registered_fs[i].name, fs_name) == 0) {
            return &registered_fs[i];
        }
    }
    return 0;
}


char *get_absolute_path(char *path, char *cwd) {
    if (path[0] != '/') {
        char tmp[MAX_PATH_NAME];
        strcpy(tmp, cwd);
        if (strcmp(cwd, "/") != 0) {
            strcat(tmp, "/");
        }
            
        strcat(tmp, path);
        strcpy(path, tmp);
    }

    char absolute_path[MAX_PATH_NAME + 1] = {};
    int idx = 0;
    for (int i = 0; i < strlen(path); i++) {
        if (path[i] == '/' && path[i + 1] == '.' && path[i + 2] == '.') {
            for (int j = idx; j >= 0; j--) {
                if (absolute_path[j] == '/') {
                    absolute_path[j] = 0;
                    idx = j;
                }
            }
            i += 2;
            continue;
        }

        if (path[i] == '/' && path[i + 1] == '.') {
            i++;
            continue;
        }

        absolute_path[idx++] = path[i];
    }
    absolute_path[idx] = 0;
    return strcpy(path, absolute_path);
}
