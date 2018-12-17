
#include <common/types.h>

#ifndef ARCH_ARMV7_IMX6_IMX6_SMC_H_
#define ARCH_ARMV7_IMX6_IMX6_SMC_H_

#define SMC_CMD_WRITE_PL310_CTRL_REG 0x102
#define SMC_CMD_WRITE_PL310_AUXCTRL_REG 0x109

#include "../driver/PL310_registers.h"

#include <common/memreg.h>

#include <common/armhelper.h>

inline void imx6_smc_enablePL310() {
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
//	omap_smc(SMC_CMD_WRITE_PL310_AUXCTRL_REG, 0x7C470001);
//	/* Enable L2 PL310 controller */
//	omap_smc(SMC_CMD_WRITE_PL310_CTRL_REG, 1);
//	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_AUXILARY_CONTROL,0x32470000);
	DSB;
	ISB;
	dword_t auxCtrl = READ_REGISTER(PL310_BASE_ADDRESS + PL310_AUXILARY_CONTROL);
	// Disable prefetching, DON'T
//	auxCtrl &= ~(0x30000000);
	// Enable event monitoring bus
	auxCtrl |= (0x100000);
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_AUXILARY_CONTROL, auxCtrl);



	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_CACHE_CONTROL,0x1);
	// Enable performance monitoring
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_COUNT0_CFG, (PL310_PERF_COUNTER_DRHIT << 0x2)|0x3 );
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_COUNT1_CFG, (PL310_PERF_COUNTER_DRREQ << 0x2)|0x3 );

	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_POWER_CONTROL, 0x0);
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_CONTROL, 0x7);

}
inline void imx6_smc_disablePL310() {
	/* Disable L2 PL310 controller */
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_CACHE_CONTROL,0x0);
}


#endif /* ARCH_ARMV7_IMX6_IMX6_SMC_H_ */
