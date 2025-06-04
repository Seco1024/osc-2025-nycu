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

void itos(int value, char *str) {
    char temp[20]; 
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    if (is_negative) {
        temp[i++] = '-';
    }

    int j = 0;
    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}
