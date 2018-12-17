/*
 * cpuid.h
 *
 *  Created on: 04.07.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_COMMON_CPUID_H_
#define ARCH_X86_COMMON_CPUID_H_

#include <common/types.h>

void inline readCPUID(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
	__asm__ __volatile__ ("cpuid":"=a"(*eax),"=b"(*ebx),"=c"(*ecx),"=d"(*edx):"a"(*eax),"c"(*ecx):"memory");
}


#endif /* ARCH_X86_COMMON_CPUID_H_ */
