/*
 * cyclecounter.h
 *
 *  Created on: 28.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_COMMON_CYCLECOUNTER_H_
#define ARCH_ARMV7_COMMON_CYCLECOUNTER_H_

#include <common/types.h>

/**
 * This macro resets the cycle counter of this core to zero and
 * writes the value to the given value variable. Should be 64bit long.
 */
#define READ_CYCLE_COUNTER(value) __asm__ __volatile__(	\
		"RDTSCP\n" \
		"SHL $32,%%rdx\n" \
		"OR %%rdx,%%rax\n" \
		"MOVQ %%rax,%[cycles]\n" \
		:[cycles]"=r"(value)::"memory","%rax","%rdx","rcx"\
	);

/**
 * Reset not implemented on AMD64 because writing of TSC is not possible.
 */
#define RESET_READ_CYCLE_COUNTER(value) __asm__ __volatile__(	\
		"RDTSCP\n" \
		"SHL $32,%%rdx\n" \
		"OR %%rdx,%%rax\n" \
		"MOVQ %%rax,%[cycles]\n" \
		:[cycles]"=r"(value)::"memory","%rax","%rdx","rcx"\
	);


#endif /* ARCH_ARMV7_COMMON_CYCLECOUNTER_H_ */
