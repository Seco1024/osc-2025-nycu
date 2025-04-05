#ifndef STDLIB_H
#define STDLIB_H

unsigned int atoi(const char *);
int htoi(const char *, int);
void align(unsigned long *, unsigned long);
int align_int(unsigned int, unsigned int);

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long uintptr_t; 

#endif