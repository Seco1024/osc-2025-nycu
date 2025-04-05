#ifndef STRING_H
#define STRING_H

#define HEX_CHARS "0123456789ABCDEF"

char getchar(void);
void putchar(char);
void puts(char *);
void puthex(unsigned int);
int strcmp(const char *, const char *);
int strlen(const char *);

#endif