/*
 * ARM_common.h
 *
 *  Created on: 30.04.2015
 *      Author: hendrik
 */

#ifndef COMMON_ARM_COMMON_H_
#define COMMON_ARM_COMMON_H_

#ifndef __ASSEMBLY__

/* Placeholder, this should be replaced by external include (omap4460.h for example) */
#ifndef PERIPHBASE
#define PERIPHBASE 0x0
#error NO periphbase defined
#endif

#ifndef GIC_PLATFORM_BASE
#define GIC_PLATFORM_BASE 0x0
#endif

#endif

#define SPURIOUS_INT 0x3ff

#define CPU_MODE_USER 0x10
#define CPU_MODE_FIQ 0x11
#define CPU_MODE_IRQ 0x12
#define CPU_MODE_SUPERVISOR 0x13
#define CPU_MODE_MONITOR 0x16
#define CPU_MODE_ABORT 0x17
#define CPU_MODE_HYP 0x1A
#define CPU_MODE_UNDEFINED 0x1B
#define CPU_MODE_SYS 0x1F

#define CPSR_I_BIT (1 << 7)
#define CPSR_F_BIT (1 << 6)

#endif /* COMMON_ARM_COMMON_H_ */
