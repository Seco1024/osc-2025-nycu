#ifndef __CPIO_H
#define __CPIO_H

#include "lib/io.h"
#include "lib/string.h"
#include "lib/stdlib.h"
#include "kernel/mm.h"
#include "kernel/interrupt_handler.h"
#include "drivers/uart.h"

#define CPIO_FOOTER_MAGIC "TRAILER!!!"
#define CPIO_BASE 0x20000000
#define MAX_FILES 100       
#define MAX_FILENAME_LENGTH 256 

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

typedef struct {
    char file_names[MAX_FILES][MAX_FILENAME_LENGTH];
    unsigned long file_sizes[MAX_FILES];
    int file_count;
    char* file_data[MAX_FILES];
} file_list_t;

void ramfs_init(void *);
void ramfs_list(int, char **);
void ramfs_cat(int, char **);
void ramfs_exec(int, char **);
unsigned long ramfs_get_file_size(const char *);
void ramfs_get_file_contents(const char *, char *);
file_list_t *ramfs_get_filelist(void);

#endif