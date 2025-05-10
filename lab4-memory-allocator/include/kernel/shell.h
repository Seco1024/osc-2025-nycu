#ifndef SHELL_H
#define SHELL_H

#include "drivers/uart.h"
#include "lib/string.h"
#include "lib/io.h"
#include "kernel/reboot.h"
#include "drivers/mailbox.h"
#include "kernel/cpio.h"
#include "kernel/mm.h"

void run_shell(void);
void send_welcome(void);
void send_prompt(void);
void send_hello(int, char **);
void send_help(int, char **);
void send_mailbox(int, char **);
void receive_command(void);
void parse_command(void);
void test_dtb(void);

typedef struct {
    char *name;
    char *description;
    void (*function)(int, char**);
} Command;

#endif