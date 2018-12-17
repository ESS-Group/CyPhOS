/*
 * cpu.h
 *
 *  Created on: 28.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_COMMON_CPU_H_
#define ARCH_ARMV7_COMMON_CPU_H_

#include <common/types.h>

inline cpu_t getCPUID() {
	cpu_t id = 0;
	asm ( "MRC p15,0,%0,c0,c0,5;" : "=&r"(id));
	return id;
}

#define SEND_WAKE __asm__ __volatile__ ("DSB;SEV;ISB;")
#define WAIT_EVENT __asm__ __volatile__ ("DSB;ISB;WFE")


#define BUSY_WAITING_SHORT_NOCACHE for ( volatile unsigned int i = 0; i< 200000;i++) {}
#define BUSY_WAITING_LONG_NOCACHE for ( volatile unsigned int i = 0; i< 10000000;i++) {}

#define BUSY_WAITING_SHORT for ( volatile unsigned int i = 0; i< 2500000;i++) {}
#define BUSY_WAITING_LONG for ( volatile unsigned int i = 0; i< 200000000;i++) {}


#endif /* ARCH_ARMV7_COMMON_CPU_H_ */
