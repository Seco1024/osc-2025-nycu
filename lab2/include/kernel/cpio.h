#ifndef __CPIO_H
#define __CPIO_H

#include "lib/io.h"
#include "lib/string.h"
#include "lib/stdlib.h"

#define CPIO_FOOTER_MAGIC "TRAILER!!!"


typedef struct cpio_newc_header {
    char    c_magic[6];
    char    c_ino[8];
    char    c_mode[8];
    char    c_uid[8];
    char    c_gid[8];
    char    c_nlink[8];
    char    c_mtime[8];
    char    c_filesize[8];
    char    c_devmajor[8];
    char    c_devminor[8];
    char    c_rdevmajor[8];
    char    c_rdevminor[8];
    char    c_namesize[8];
    char    c_check[8];
} cpio_t;

void ramfs_init(void *);
void ramfs_list(void);
void ramfs_cat(void);


#endif