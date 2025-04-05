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