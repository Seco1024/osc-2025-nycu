#ifndef REBOOT_H
#define REBOOT_H

#include "lib/io.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC 0x3F10001c
#define PM_WDOG 0x3F100024


void set(long, unsigned int);
void reset(int);
void reboot(int, char **);

#endif