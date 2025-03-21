#include "lib/stdlib.h"

unsigned int atoi(const char *str) {
    int sign = 1;
    unsigned int result = 0;

    while (*str == ' ' || *str == '\t') {
        str++;
    }

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

int htoi(const char *hex, int len) {
    int result = 0;
    while (len-- > 0) {
        result <<= 4;
        if (*hex <= 'F' && *hex >= 'A') {
            result += *hex++ - 0x37;
        } else {
            result += *hex++ - '0';
        }
    }
    return result;
}

void align(unsigned long *val, unsigned long align_val) {
    if (*val % align_val != 0) {
        *val += align_val - (*val % align_val);
    }
}

int align_int(unsigned int val, unsigned int align_val) {
    if (val % align_val != 0) {
        val += align_val - (val % align_val);
    }

    return val;
}