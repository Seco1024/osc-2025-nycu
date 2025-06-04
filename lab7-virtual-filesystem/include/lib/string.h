#ifndef STRING_H
#define STRING_H

int strcmp(const char *, const char *);
int strlen(const char *);
char *strtok(char *, const char *);
void *memncpy(void *, const void *, unsigned int);
char *strcpy(char *, const char *);
void *memset(void *, int, unsigned long long);
char *strcat(char *dest, const char *src);

#endif