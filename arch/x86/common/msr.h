/*
 * msr.h
 *
 *  Created on: 14.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_COMMON_MSR_H_
#define ARCH_X86_COMMON_MSR_H_

#include <common/types.h>

#define COMMON_MSR_SMI_COUNT 0x34

inline void writeMSR(uint32_t address, uint64_t value) {
	uint32_t low = value & 0xFFFFFFFF;
	uint32_t high = value >> 32;

	asm volatile(
			"mov %0, %%eax;"
			"mov %1, %%edx;"
			"mov %2, %%ecx;"
			"wrmsr;":
			:"r"(low),"r"(high),"r"(address):"eax","edx","ecx");
}

inline uint64_t readMSR(uint32_t address) {
	uint32_t low = 0;
	uint32_t high = 0;
	asm volatile(
			"mov %2, %%ecx;"
			"rdmsr;"
			"mov %%edx, %0;"
			"mov %%eax, %1;":
			"=r"(high),"=r"(low):"r"(address):"eax","edx","ecx");
	uint64_t result = high;
	result <<= 32;
	result |= low;
	return result;
}

#endif /* ARCH_X86_COMMON_MSR_H_ */
