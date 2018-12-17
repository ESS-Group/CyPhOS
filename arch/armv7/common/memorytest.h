/*
 * memorytest.h
 *
 *  Created on: 20.11.2018
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_COMMON_MEMORYTEST_H_
#define ARCH_ARMV7_COMMON_MEMORYTEST_H_

#define MEASURE_MEMORY_ACCESS(address, retVal) __asm__ __volatile__ ( \
				"ISB;" \
				"DSB;" \
				"MRC p15, 0, r1, c9, c13, 0;" \
				"LDR r0,[%[addr]];" \
				"ISB;" \
				"DSB;" \
				"MRC p15, 0, %[cycles], c9, c13, 0;" \
				"SUB %[cycles],%[cycles], R1" \
				:[cycles]"=r"(retVal):[addr]"r"(address):"memory","r0","r1");



#endif /* ARCH_ARMV7_COMMON_MEMORYTEST_H_ */
