#include "lib/string.h"


int strcmp(const char *a, const char *b) {
    while(*a == *b) {
        if(*a == '\0') {
            return 0;
        }
        a++;
        b++;
    }
    return *a - *b;
}

int strlen(const char *a) {
    int counter = 0;
    while(*a++ != '\0') {
        counter++;
    }
    return counter;
}

char *strchr(const char *s, char c) {
    while (*s) {
        if (*s == c) {
            return (char *)s;
        }
        s++;
    }
    return 0;
}

char *strtok(char *str, const char *delim) {
    static char *saved;

    if (str != 0) {
        saved = str;
    } else if (saved == 0) {
        return 0;
    }

    while (*saved && strchr(delim, *saved)) {
        saved++;
    }

    if (*saved == '\0') {
        return 0;
    }

    char *token_start = saved;

    while (*saved && !strchr(delim, *saved)) {
        saved++;
    }

    if (*saved) {
        *saved = '\0';
        saved++;
    } else {
        saved = 0;
    }

    return token_start;
}

void *memncpy(void *dst, const void *src, unsigned int n) {
    char *d = (char *)dst;
    const char *s = (const char *)src;

    while (n--) {
        *d++ = *s++;
    }

    return dst;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

void *memset(void *s, int c, unsigned long long n) {
    unsigned char *p = (unsigned char *)s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

char* strcat (char *dest, const char *src) {
  strcpy (dest + strlen (dest), src);
  return dest;
}