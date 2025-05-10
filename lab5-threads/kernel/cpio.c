#include "kernel/cpio.h"

static void *initramfs_address = (void *)CPIO_BASE;
// static void *initramfs_address;

void ramfs_init(void *ptr) {
    initramfs_address = ptr;
    return ;
}


void ramfs_list(int argc, char **argv) {
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

void ramfs_cat(int argc, char **argv) {
    if (argc < 2) {
        puts("\nUsage: cat <filename>\n");
        return ;
    }
    char *ptr = (char *)initramfs_address;
    while(strcmp((char *)(ptr + sizeof(cpio_t)), CPIO_FOOTER_MAGIC) != 0) {
        cpio_t *header = (cpio_t *)ptr;
        unsigned long namesize = htoi(header->c_namesize, 8);
        unsigned long filesize = htoi(header->c_filesize, 8);
        unsigned long headerPathname_size = sizeof(cpio_t) + namesize;

        align(&headerPathname_size, 4);
        align(&filesize, 4);

        if (!strcmp(argv[1], (char *)(ptr + sizeof(cpio_t)))) {
            char *file_content = ptr + headerPathname_size;
            for (unsigned long i = 0; i < filesize; ++i) {
                putchar(file_content[i]); 
            }
            puts("\n");
            return ;
        } else {
            ptr += headerPathname_size + filesize;
        }
    }
}

void ramfs_exec(int argc, char **argv) {
    if (argc < 2) {
        puts("\nUsage: exec <filename>\n");
        return ;
    }
    char *ptr = (char *)initramfs_address;
    while(strcmp((char *)(ptr + sizeof(cpio_t)), CPIO_FOOTER_MAGIC) != 0) {
        cpio_t *header = (cpio_t *)ptr;
        unsigned long namesize = htoi(header->c_namesize, 8);
        unsigned long filesize = htoi(header->c_filesize, 8);
        unsigned long headerPathname_size = sizeof(cpio_t) + namesize;

        align(&headerPathname_size, 4);
        align(&filesize, 4);

        if (!strcmp(argv[1], (char *)(ptr + sizeof(cpio_t)))) {
            char *file_content = ptr + headerPathname_size;
            char *user_program = USER_PROGRAM_BASE;
            memncpy((void *)user_program, (void *)file_content, (unsigned int)filesize);
            from_el1_to_el0((uint64_t)USER_PROGRAM_BASE, (uint64_t)USER_STACK_POINTER_BASE);
            break;
        } else {
            ptr += headerPathname_size + filesize;
        }
    }
}

void ramfs_get_file_contents(const char *file_name, char *buf) {
    char *ptr = (char *)initramfs_address;
    while(strcmp((char *)(ptr + sizeof(cpio_t)), CPIO_FOOTER_MAGIC) != 0) {
        cpio_t *header = (cpio_t *)ptr;
        unsigned long namesize = htoi(header->c_namesize, 8);
        unsigned long filesize = htoi(header->c_filesize, 8);
        unsigned long headerPathname_size = sizeof(cpio_t) + namesize;

        align(&headerPathname_size, 4);
        align(&filesize, 4);

        if (!strcmp(file_name, (char *)(ptr + sizeof(cpio_t)))) {
            char *file_content = ptr + headerPathname_size;
            memncpy((void *)buf, (void *)file_content, (unsigned int)filesize);
            return ;
        } else {
            ptr += headerPathname_size + filesize;
        }
    }
    return ;
}

unsigned long ramfs_get_file_size(const char *file_name) {
    char *ptr = (char *)initramfs_address;
    while(strcmp((char *)(ptr + sizeof(cpio_t)), CPIO_FOOTER_MAGIC) != 0) {
        cpio_t *header = (cpio_t *)ptr;
        unsigned long namesize = htoi(header->c_namesize, 8);
        unsigned long filesize = htoi(header->c_filesize, 8);
        unsigned long headerPathname_size = sizeof(cpio_t) + namesize;

        align(&headerPathname_size, 4);
        align(&filesize, 4);

        if (!strcmp(file_name, (char *)(ptr + sizeof(cpio_t)))) {
            return filesize;
        } else {
            ptr += headerPathname_size + filesize;
        }
    }
    return 0;
}