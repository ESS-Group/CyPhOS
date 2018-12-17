/*
 * cyclecounter.h
 *
 *  Created on: 28.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_COMMON_CYCLECOUNTER_H_
#define ARCH_ARMV7_COMMON_CYCLECOUNTER_H_

/**
 * This macro resets the cycle counter of this core to zero and
 * writes the value to the given value variable. Should be 32bit long.
 */
#define READ_CYCLE_COUNTER(value) __asm__ __volatile__(	\
		"ISB;" \
		"DSB;" \
		"MRC p15, 0, %0, c9, c13, 0;"		\
		: "=r" (value)\
		: : "r0","memory" \
	);


#define RESET_READ_CYCLE_COUNTER(value) __asm__ __volatile__(	\
		"ISB;" \
		"DSB;" \
		"MOV r0,#0x0;" \
		"MCR p15, 0, r0, c9, c13, 0;" \
		"MRC p15, 0, %0, c9, c13, 0;"		\
		: "=r" (value)\
		: : "r0","memory" \
	);

#endif /* ARCH_ARMV7_COMMON_CYCLECOUNTER_H_ */
