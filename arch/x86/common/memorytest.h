/*
 * memorytest.h
 *
 *  Created on: 20.11.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_COMMON_MEMORYTEST_H_
#define ARCH_X86_COMMON_MEMORYTEST_H_

#define MEASURE_MEMORY_ACCESS(address, retVal) __asm__ __volatile__ ( \
				"RDTSCP\n" \
				"SHL $32,%%rdx\n" \
				"OR %%rdx,%%rax\n" \
				"MOVQ %%rax,%%r8\n" \
				"MOVQ (%[addr]),%%rax\n" \
				"RDTSCP\n" \
				"SHL $32,%%rdx\n" \
				"OR %%rdx,%%rax\n" \
				"SUB %%r8,%%rax\n" \
				"MOVQ %%rax,%[cycles]\n" \
				"CPUID\n" \
				:[cycles]"=r"(retVal):[addr]"r"(address):"memory","%rax","%rbx","%rcx","%rdx","r8");




#endif /* ARCH_X86_COMMON_MEMORYTEST_H_ */
