/*
 * exynos_smc.h
 *
 *  Created on: 14.07.2015
 *      Author: hendrik
 */

#ifndef ARCH_EXYNOS4412_EXYNOS_SMC_H_
#define ARCH_EXYNOS4412_EXYNOS_SMC_H_

#include <common/types.h>

#include <common/memreg.h>

#define SMC_CMD_INIT		(-1)
#define SMC_CMD_INFO		(-2)
/* For Power Management */
#define SMC_CMD_SLEEP		(-3)
#define SMC_CMD_CPU1BOOT	(-4)
#define SMC_CMD_CPU0AFTR	(-5)
/* For CP15 Access */
#define SMC_CMD_C15RESUME	(-11)
/* For L2 Cache Access */
#define SMC_CMD_L2X0CTRL	(-21)
#define SMC_CMD_L2X0SETUP1	(-22)
#define SMC_CMD_L2X0SETUP2	(-23)
#define SMC_CMD_L2X0INVALL	(-24)
#define SMC_CMD_L2X0DEBUG	(-25)

/* Assembly code for secure monitor (ROM) call */
extern "C" {
//	void exynos_smc(uint32_t cmd, uint32_t arg1, uint32_t arg2, uint32_t arg3);
}

inline void exynos_smc(uint32_t cmd, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
	__asm__ __volatile__ (".arch_extension sec\n"
			"mov r0,%[cmd]\n"
			"mov r1,%[arg1]\n"
			"mov r2,%[arg2]\n"
			"mov r3,%[arg3]\n"
			"dsb \n"
			"smc	#0 \n"
	"dsb;" "isb;"
			 ::[cmd]"r"(cmd),[arg1]"r"(arg1),[arg2]"r"(arg2),[arg3]"r"(arg3):"r0","r1","r2","r3","memory");
}

#include "../driver/PL310_registers.h"
inline void exynos_smc_enablePL310() {
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
	// Enable performance monitoring
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_COUNT0_CFG, (PL310_PERF_COUNTER_DRHIT << 0x2)|0x3 );
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_COUNT1_CFG, (PL310_PERF_COUNTER_DRREQ << 0x2)|0x3);

	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_CONTROL, 0x7);

	// TAG RAM, DATA RAM, PREFETCH CONTROL
	exynos_smc(SMC_CMD_L2X0SETUP1, READ_REGISTER(PL310_BASE_ADDRESS + PL310_TAG_RAM_CONTROL), READ_REGISTER(PL310_BASE_ADDRESS + PL310_DATA_RAM_CONTROL), 0x70800000);

	// POWER CONTROL, AUX CONTROL, ?
	exynos_smc(SMC_CMD_L2X0SETUP2, 0x0, 0x7C470000, 0xC200ffff);
	/* Enable L2 PL310 controller */
	exynos_smc(SMC_CMD_L2X0CTRL, 1, 0, 0);
}
inline void exynos_smc_disablePL310() {
	if ( READ_REGISTER(PL310_BASE_ADDRESS + PL310_CACHE_CONTROL) & 0x1) {
		/* Disable L2 PL310 controller */
		exynos_smc(SMC_CMD_L2X0CTRL, 0, 0, 0);
	}
}


#endif /* ARCH_EXYNOS4412_EXYNOS_SMC_H_ */
