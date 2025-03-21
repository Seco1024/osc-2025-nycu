#ifndef SHELL_H
#define SHELL_H

#include "drivers/uart.h"
#include "lib/string.h"

// Lab1: Simple Shell & Mailbox
void run_shell(void);
void send_welcome(void);
void send_prompt(void);
void send_hello(void);
void send_help(void);
void send_mailbox(void);
void receive_command(void);
void parse_command(void);

typedef struct {
    char *name;
    char *description;
    void (*function)(void);
} Command;

#endif