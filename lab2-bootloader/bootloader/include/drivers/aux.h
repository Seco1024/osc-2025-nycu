#ifndef AUX_H
#define AUX_H

#include "gpio.h"

#define AUX_ENB         ((volatile unsigned int*)(PBASE+0x00215004))
#define AUX_MU_CNTL     ((volatile unsigned int*)(PBASE+0x00215060))
#define AUX_MU_IER      ((volatile unsigned int*)(PBASE+0x00215044))
#define AUX_MU_LCR      ((volatile unsigned int*)(PBASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(PBASE+0x00215050))
#define AUX_MU_BAUD     ((volatile unsigned int*)(PBASE+0x00215068))
#define AUX_MU_IIR      ((volatile unsigned int*)(PBASE+0x00215048))
#define AUX_MU_LSR      ((volatile unsigned int*)(PBASE+0x00215054))
#define AUX_MU_IO       ((volatile unsigned int*)(PBASE+0x00215040))

#endif