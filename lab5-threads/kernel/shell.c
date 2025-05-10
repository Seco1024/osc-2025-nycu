#include "kernel/shell.h"
#include "kernel/dtb.h"
#include "kernel/cpio.h"
#include "kernel/timer.h"
#include "kernel/timeout.h"
#include "kernel/thread.h"
#include "lib/string.h"
#include "lib/stdlib.h"
#include "drivers/uart.h"
#include "kernel/syscall.h"
#include "kernel/kmalloc.h"

#define PADDING 10

static void *g_user_image;

Command command_table[] = {
    {.name="hello", .description="print Hello World!\n", .function=send_hello},
    {.name="help", .description="print this help menu\n", .function=send_help},
    {.name="mailbox", .description="print hardware's information\n", .function=send_mailbox},
    {.name="reboot", .description="reboot\n", .function=reboot},
    {.name="ls", .description="list initramfs\n", .function=ramfs_list},
    {.name="cat", .description="print file contents inside initramfs\n", .function=ramfs_cat},
    {.name="exec", .description="execute a binary\n", .function=exec_demo},
    {.name="memAlloc", .description="test simple_alloc\n", .function=test_simple_alloc},
    {.name="setTimeout", .description="set timeout task\n", .function=set_timeout},
    {.name="timer_demo", .description="enable timer demo\n", .function=timer_demo},
    {.name="thread_demo", .description="demo thread\n", .function=test_threads},
    {.name="fork_demo", .description="demo fork\n", .function=fork_demo}
};

void test_dtb(void) {
    fdt_traverse(ramfs_init);
}

void send_welcome(void) {
    puts("\nWelcome to OSC Lab!\n");
}

void send_prompt(void) {
    puts("# ");
}

void send_hello(int argc, char **argv) {
    puts("Hello World!\n");
}

void send_help(int argc, char **argv) {
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

void send_mailbox(int argc, char **argv) {
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
    int argc = 0;
    char *argv[32];
    char *tok = strtok(buffer, " ");
    while (tok != NULL) {
        argv[argc++] = tok;
        tok = strtok(NULL, " ");
    }

    int n_commands = sizeof(command_table) / sizeof(Command);
    int flag = 0;
    for (int i = 0; i < n_commands; ++i) {
        if (!strcmp(command_table[i].name, argv[0])) {
            command_table[i].function(argc, argv);
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
        // async_gets();
        receive_command();
        parse_command();
        clear_buffer();
    }
}

void spawn_user_program(const char *file_name) {
    unsigned long file_size = ramfs_get_file_size(file_name);
    if (file_size == 0) {
        puts("[ERROR] File not found\n");
        return ;
    }
    char *raw_image = allocate(file_size);
    g_user_image = allocate(file_size);
    ramfs_get_file_contents(file_name, raw_image);
    memncpy(g_user_image, raw_image, file_size);
    // enable_user_to_physical_timer();
    task_struct *task = thread_create((void *)g_user_image);

    asm volatile("msr tpidr_el1, %0;" ::"r"(task));
    asm volatile("msr spsr_el1, %0" ::"r"(0x0));
    asm volatile("msr elr_el1, %0" ::"r"(task->context.lr));
    asm volatile("msr sp_el0, %0" ::"r"(task->user_stack + STACK_SIZE));
    asm volatile("mov sp, %0" ::"r"(task->stack + STACK_SIZE));
    asm volatile("eret");
}

void exec_demo(int argc, char **argv) {
    if (argc < 2) {
        puts("Usage: exec <filename>\n");
        return;
    }
}

void fork_test() {
    puts("Fork Test (pid = ");
    puti(getpid());
    puts(")\n");
    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) {
        long long cur_sp;
        asm volatile("mov %0, sp" : "=r"(cur_sp));

        puts("first child pid: ");
        puti(getpid());
        puts(", cnt: ");
        puti(cnt);
        puts(", ptr: ");
        puthex64((unsigned long long)&cnt);
        puts(", cur_sp: ");
        puthex64((unsigned long long)cur_sp);
        puts("\n");
        cnt++;
        if ((ret = fork()) != 0) {
            asm volatile("mov %0, sp" : "=r"(cur_sp));
            puts("first child pid: ");
            puti(getpid());
            puts(", cnt: ");
            puti(cnt);
            puts(", ptr: ");
            puthex64((unsigned long long)&cnt);
            puts(", cur_sp: ");
            puthex64((unsigned long long)cur_sp);
            puts("\n");
        } else {
            while (cnt < 5) {
                asm volatile("mov %0, sp" : "=r"(cur_sp));
                puts("second child pid: ");
                puti(getpid());
                puts(", cnt: ");
                puti(cnt);
                puts(", ptr: ");
                puthex64((unsigned long long)&cnt);
                puts(", cur_sp: ");
                puthex64((unsigned long long)cur_sp);
                puts("\n");
                for (int i = 0; i < 1000000; i++);
                cnt++;
            }
        }
        exit();
    } else {
        puts("parent here, pid ");
        puti(getpid());
        puts(", child ");
        puti(ret);
        puts("\n");
    }
    exit();
}

void fork_demo(int argc, char **argv) {
    thread_create(run_fork_test);
}

void run_fork_test(void) {
    from_el1_to_el0((uint64_t)fork_test, (uint64_t)get_current()->user_stack + STACK_SIZE);
}
