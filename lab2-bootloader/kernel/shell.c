#include "kernel/shell.h"
#include "kernel/dtb.h"
#include "kernel/cpio.h"

#define PADDING 10

Command command_table[] = {
    {.name="hello", .description="print Hello World!\n", .function=send_hello},
    {.name="help", .description="print this help menu\n", .function=send_help},
    {.name="mailbox", .description="print hardware's information\n", .function=send_mailbox},
    {.name="reboot", .description="reboot\n", .function=reboot},
    {.name="ls", .description="list initramfs\n", .function=ramfs_list},
    {.name="cat", .description="print file contents inside initramfs\n", .function=ramfs_cat},
    {.name="memAlloc", .description="test simple_alloc\n", .function=test_simple_alloc}
};

void test_dtb(void) {
    fdt_traverse(ramfs_init);
}

void send_welcome(void) {
    puts("\nWelcome to OSC Lab1!\n");
}

void send_prompt(void) {
    puts("# ");
}

void send_hello(void) {
    puts("Hello World!\n");
}

void send_help(void) {
    int n_commands = sizeof(command_table) / sizeof(Command);
    for (int i = 0; i < n_commands; ++i) {
        puts(command_table[i].name);
        for (int j = 0; j < PADDING - strlen(command_table[i].name); ++j) {
            putchar(' ');
        }
        puts(": ");
        puts(command_table[i].description);
    }
}

void send_mailbox(void) {
    get_board_revision();
    get_memory_information();
}

void receive_command(void) {
    char c;
    int idx = 0;
    while((c = getchar()) != '\n') {
        if (c == 0x08 || c == 0x7F) {
            if (idx > 0) {
                putchar('\b');
                putchar(' ');
                putchar('\b');
                *(buffer + --idx) = '\0';
            }
            continue;
        }
        putchar(c);
        *(buffer + idx++) = c;
    }
    *(buffer + idx++) = '\0';
    puts("\n");
}

void parse_command(void) {
    int n_commands = sizeof(command_table) / sizeof(Command);
    int flag = 0;
    for (int i = 0; i < n_commands; ++i) {
        if (!strcmp(command_table[i].name, buffer)) {
            command_table[i].function();
            flag = 1;
            break;
        }
    }
    if (!flag) {
        puts("Command not found!\r\n");
    }
}

void run_shell(void) {
    send_welcome();
    while(1) {
        send_prompt();
        // receive_command();
        gets();
        parse_command();
        clear_buffer();
    }
}