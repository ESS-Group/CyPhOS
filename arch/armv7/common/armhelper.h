/*
 * armhelper.h
 *
 *  Created on: 25.03.2015
 *      Author: hendrik
 */

#ifndef COMMON_ARMHELPER_H_
#define COMMON_ARMHELPER_H_

#include <common/types.h>



dword_t inline getSCTLR() {
	dword_t reg = 0;
	asm ( "MRC p15, 0,%0, c1, c0, 0;" : "=&r"(reg));
	return reg;
}

dword_t inline getCPSR() {
	dword_t reg = 0;
	asm ( "MRS %0, CPSR;" : "=&r"(reg));
	return reg;
}

dword_t inline getStackpointer() {
	dword_t reg = 0;
	asm ( "MOV %0,SP;" : "=&r"(reg));
	return reg;
}


dword_t inline getACTLR() {
	dword_t reg = 0;
	asm ( "MRC p15, 0,%0, c1, c0, 1;" : "=&r"(reg));
	return reg;
}

inline void enableInterruptBit() {
	__asm__ __volatile__ ("cpsie i;ISB;DSB;":::"memory");
}

inline void disableInterruptBit() {
	__asm__ __volatile__ ("cpsid i;ISB;DSB;":::"memory");
}

inline void enableFastInterruptBit() {
	__asm__ __volatile__ ("cpsie f":::"memory");
}

inline void disableFastInterruptBit() {
	__asm__ __volatile__ ("cpsid f":::"memory");
}

#define DSBISB __asm__ __volatile__("dsb\nisb\n":::"memory")
#define DSB __asm__ __volatile__("dsb\n":::"memory")
#define ISB __asm__ __volatile__("isb\n":::"memory")



#endif /* COMMON_ARMHELPER_H_ */
