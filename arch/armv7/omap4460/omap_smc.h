/*
 * omap_smc.h
 *
 *  Created on: 22.04.2016
 *      Author: hendrik
 */

#include <common/types.h>

#ifndef ARCH_ARMV7_OMAP4460_OMAP_SMC_H_
#define ARCH_ARMV7_OMAP4460_OMAP_SMC_H_

#define SMC_CMD_WRITE_PL310_CTRL_REG 0x102
#define SMC_CMD_WRITE_PL310_AUXCTRL_REG 0x109

inline void omap_smc(uint32_t opCode, uint32_t val) {
	__asm__ __volatile__ (
			".arch_extension sec\n"
			".arch_extension mp\n"
			"mov r0,%[opCode]\n"
			"mov r1,%[val]\n"
			"dsb \n"
			"smc	#0 \n" ::[opCode]"r"(opCode),[val]"r"(val):"r0","r1","memory");
}

#include "../driver/PL310_registers.h"
inline void omap4460_smc_enablePL310() {
	/*	// Set L2 auxiliary control register
	 //	dword_t reg = 0x0;
	 //	reg |= 1 << 16; // Associativity
	 //	reg |= 1 << 25; // reserved lol, see linux kernel omap4-common.c
	 //	reg |= 1 << 26; // NS lockdown enable
	 //	reg |= 1 << 27; // NS interrupt enable
	 //	reg |= 0x3 << 17; // Way_size
	 //	reg |= 1 << 22; // share override
	 //	reg |= 1 << 28; // data prefetch
	 //	reg |= 1 << 29; // instruction prefetch
	 //	reg |= 1 << 30; // early bresp */
	// FIXME CHECK for correctness
	omap_smc(SMC_CMD_WRITE_PL310_AUXCTRL_REG, 0x7C470001);
	/* Enable L2 PL310 controller */
	omap_smc(SMC_CMD_WRITE_PL310_CTRL_REG, 1);
}
inline void omap4460_smc_disablePL310() {
	/* Disable L2 PL310 controller */
	omap_smc(SMC_CMD_WRITE_PL310_CTRL_REG, 0);
}


#endif /* ARCH_ARMV7_OMAP4460_OMAP_SMC_H_ */
