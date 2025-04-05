#include "kernel/dtb.h"
#include "lib/io.h"
#include "lib/string.h"

extern uint32_t _dtb_base;  

uint32_t betole(const void *p) {
    const unsigned char *bytes = p;
    uint32_t result = *(bytes + 3);
    result |= *(bytes + 2) << 8;
    result |= *(bytes + 1) << 16;
    result |= *bytes << 24;
    return result;
}

// void fdt_traverse(void (*function)(void*)) {
//     int flag = 0;
//     uintptr_t dtb_base = (uintptr_t)_dtb_base;
//     puts(".dtb loaded at: ");
//     puthex(_dtb_base);
//     fdt_t *header = (fdt_t *)dtb_base;
//     if (betole(&(header->magic)) != 0xd00dfeed) {
//         puts("DTB not loaded\n");
//         return;
//     }
//     uint8_t *structure = (uint8_t *)(dtb_base + betole(&(header->off_dt_struct)));
//     uint8_t *strings = (uint8_t *)(dtb_base + betole(&(header->off_dt_strings)));
//     uint32_t totalsize = betole(&(header->totalsize));
//     uint32_t *ptr = (uint32_t *)structure;

//     while((uint8_t *)ptr < structure + header->size_dt_strings) {
//         uint32_t token = betole(ptr++);
//         switch (token) {
//             case FDT_BEGIN_NODE:
//                 ptr += align_int(strlen((char *)ptr) + 1, 4);
//                 break;
//             case FDT_END_NODE:
//                 break;
//             case FDT_PROP:
//                 uint32_t property_length = betole(ptr++);
//                 uint32_t offset = betole(ptr++);
//                 char *name = (char *)(strings + offset);
//                 puts(name);
//                 puts("\n");
//                 if(strcmp(name, "linux,initrd-start") == 0) {
//                     puts("Found initrd");
//                     flag = 1;
//                     function((void *)(uintptr_t)(betole(ptr)));
//                 }
//                 ptr += align_int(property_length, 4);
//                 break;
//             case FDT_NOP:
//                 break;
//             case FDT_END:
//                 break;
//         }
//     }
//     if (flag == 0) {
//         puts("Not found");
//     }
// }


void fdt_traverse(void (*function)(void*)) {
    uintptr_t dtb_base = (uintptr_t)_dtb_base;
    puts(".dtb loaded at: ");
    puthex(_dtb_base);

    fdt_t *header = (fdt_t *)dtb_base;
    if (betole(&(header->magic)) != 0xd00dfeed) {
        puts("DTB not loaded\n");
        return;
    }

    uint8_t *structure = (uint8_t *)(dtb_base + betole(&(header->off_dt_struct)));
    uint8_t *strings   = (uint8_t *)(dtb_base + betole(&(header->off_dt_strings)));
    uint32_t struct_size = betole(&(header->size_dt_struct));
    uint8_t *struct_end  = structure + struct_size;
    uint32_t *ptr = (uint32_t *)structure;

    while ((uint8_t *)ptr < struct_end) {
        uint32_t token = betole(ptr++);
        switch (token) {
            case FDT_BEGIN_NODE: {
                char *node_name = (char *)ptr;
                int name_len = strlen(node_name) + 1;
                ptr = (uint32_t *)(((uintptr_t)ptr + name_len + 3) & ~((uintptr_t)3));
                break;
            }
            case FDT_END_NODE:
                break;
            case FDT_PROP: {
                uint32_t property_length = betole(ptr++);
                uint32_t name_offset     = betole(ptr++);
                char *prop_name = (char *)(strings + name_offset);
                if (strcmp(prop_name, "linux,initrd-start") == 0) {
                    puts("Found initrd");
                    uint32_t prop_val = betole(ptr);
                    function((void *)(uintptr_t)prop_val);
                }
                ptr = (uint32_t *)(((uintptr_t)ptr + property_length + 3) & ~((uintptr_t)3));
                break;
            }
            case FDT_NOP:
                break;
            case FDT_END:
                return;
            default:
                break;
        }
    }
}
