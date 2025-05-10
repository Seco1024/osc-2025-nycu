#ifndef _REGS_H
#define _REGS_H

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)
#define SCTLR_VALUE_MMU_DISABLED	    (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

#define SPSR_MASK_D         (1 << 9)
#define SPSR_MASK_A         (1 << 8)
#define SPSR_MASK_I         (1 << 7)
#define SPSR_MASK_F         (1 << 6)

#define SPSR_EL0t           (0b0000 << 0)
#define SPSR_EL1t           (0b0100 << 0)
#define SPSR_EL1h           (0b0101 << 0)	
#define SPSR_EL2t           (0b1000 << 0)	
#define SPSR_EL2h           (0b1001 << 0)	
#define SPSR_EL2_VALUE      (SPSR_MASK_D | SPSR_MASK_A | SPSR_MASK_I | SPSR_MASK_F | SPSR_EL1h)
#define SPSR_EL1_DISABLED   (SPSR_MASK_D | SPSR_MASK_A | SPSR_MASK_I | SPSR_MASK_F | SPSR_EL0t)
#define SPSR_EL1_ENABLED    0x00000000

#define HCR_RW	    		(1 << 31)
#define HCR_EL2_VALUE		HCR_RW

#define ESR_ELx_EC_SHIFT		26
#define ESR_ELx_EC_SVC64		0x15
#define __NR_syscalls	        11

#endif