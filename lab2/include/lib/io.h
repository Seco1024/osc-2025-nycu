#ifndef IO_H
#define IO_H

#define HEX_CHARS "0123456789ABCDEF"
#define BUFFER_SIZE 1024

extern char buffer[]; 
char getchar(void);
char *gets(void);
void putchar(char);
void puts(char *);
void puthex(unsigned int);
void puthex64(unsigned long);
void clear_buffer(void);

#endif