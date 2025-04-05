#include "drivers/mailbox.h"
#include "lib/string.h"

void mailbox_call(unsigned int *mailbox) {
    unsigned int r = (((unsigned long)mailbox) & ~0xF) | 8;
    while(*MAILBOX_STATUS & MAILBOX_FULL){}
    *MAILBOX_WRITE = r;
    while (*MAILBOX_STATUS & MAILBOX_EMPTY){}
    if (r == *MAILBOX_READ) return ;
}

void get_board_revision(void){
    unsigned int mailbox[7];
    mailbox[0] = 7 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_BOARD_REVISION; // tag identifier
    mailbox[3] = 4; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    // tags end
    mailbox[6] = END_TAG;
  
    mailbox_call(mailbox); // message passing procedure call, you should implement it following the 6 steps provided above.

    puts("Board revision: ");
    puthex(mailbox[5]); // it should be 0xa020d3 for rpi3 b+
    puts("\n");
}

void get_memory_information(void) {
    unsigned int mailbox[8];
    mailbox[0] = 8 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_MEMORY_INFO; // tag identifier
    mailbox[3] = 8; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    mailbox[6] = 0; // value buffer
    // tags end
    mailbox[7] = END_TAG;
  
    mailbox_call(mailbox); // message passing procedure call, you should implement it following the 6 steps provided above.
    
    puts("ARM memory base: ");
    puthex(mailbox[5]); 
    puts("\n");
    puts("ARM memory size: ");
    puthex(mailbox[6]); 
    puts("\n");
}