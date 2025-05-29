#ifndef MAILBOX_H
#define MAILBOX_H

#include "lib/io.h"
#include "kernel/interrupt_handler.h"

#define MMIO_BASE       0x3f000000
#define MAILBOX_BASE    MMIO_BASE + 0xb880

#define MAILBOX_READ    ((volatile unsigned int *)MAILBOX_BASE)
#define MAILBOX_STATUS  ((volatile unsigned int *)(MAILBOX_BASE+0x18))
#define MAILBOX_WRITE   ((volatile unsigned int *)(MAILBOX_BASE+0x20))

#define MAILBOX_EMPTY       0x40000000
#define MAILBOX_FULL        0x80000000

#define GET_BOARD_REVISION  0x00010002
#define GET_MEMORY_INFO     0x00010005
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x0))
#define MBOX_POLL       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x10))
#define MBOX_SENDER     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x14))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x18))
#define MBOX_CONFIG     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x1C))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX+0x20))
#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

void get_board_revision(void);
void get_memory_information(void);
void mailbox_call(unsigned int*);
int sys_mailbox_call(unsigned char, unsigned int *);

#endif