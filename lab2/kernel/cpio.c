#include "kernel/cpio.h"
#include "lib/stdlib.h"

#define CPIO_BASE 0x20000000
static void *initramfs_address = (void *)CPIO_BASE;
// static void *initramfs_address;

void ramfs_init(void *ptr) {
    initramfs_address = ptr;
    return ;
}


void ramfs_list(void) {
    char *ptr = (char *)initramfs_address;
    while(strcmp((char *)(ptr + sizeof(cpio_t)), CPIO_FOOTER_MAGIC) != 0) {
        cpio_t *header = (cpio_t *)ptr;
        unsigned long namesize = htoi(header->c_namesize, 8);
        unsigned long filesize = htoi(header->c_filesize, 8);
        unsigned long headerPathname_size = sizeof(cpio_t) + namesize;

        align(&headerPathname_size, 4);
        align(&filesize, 4);

        puts(ptr + sizeof(cpio_t));
        puts("\n");
        ptr += headerPathname_size + filesize;
    }
} 

void ramfs_cat(void) {
    char *ptr = (char *)initramfs_address;
    while(strcmp((char *)(ptr + sizeof(cpio_t)), CPIO_FOOTER_MAGIC) != 0) {
        cpio_t *header = (cpio_t *)ptr;
        unsigned long namesize = htoi(header->c_namesize, 8);
        unsigned long filesize = htoi(header->c_filesize, 8);
        unsigned long headerPathname_size = sizeof(cpio_t) + namesize;

        align(&headerPathname_size, 4);
        align(&filesize, 4);

        puts("Filename: ");
        puts(ptr + sizeof(cpio_t));
        puts("\n");

        char *file_content = ptr + headerPathname_size;
        for (unsigned long i = 0; i < filesize; ++i) {
            putchar(file_content[i]); 
        }
        puts("\n");
        ptr += headerPathname_size + filesize;
    }
}