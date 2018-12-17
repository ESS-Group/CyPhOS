/*
 * OSCOMAP4460ClockManagement.h
 *
 *  Created on: 26.04.2016
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_OMAP4460_DRIVER_OSCOMAP4460CLOCKMANAGEMENT_H_
#define ARCH_ARMV7_OMAP4460_DRIVER_OSCOMAP4460CLOCKMANAGEMENT_H_

#include <component/OSC.h>

/* Include types */
#include <common/types.h>

#include "../../driver/ARMClockManagement.h"

#define CM_CLKMODE_DPLL_MPU 0x4A004160
#define CM_CLKSEL_DPLL_MPU 0x4A00416C
#define CM_DIV_M2_DPLL_MPU 0x4A004170
#define CM_IDLEST_DPLL_MPU 0x4A004164
#define CM_AUTOIDLE_DPLL_MPU 0x4A004168
#define CM_MPU_MPU_CLKCTRL 0x4A004320


// Pandaboards reference clock ticks at 38.4 MHz
#define OMAP4_BASE_CORE 38400000

class OSC_OMAP4460ClockManagement: public ARMClockManagement{
public:
	enum PANDA_CLKS {
		PANDA_350MHZ,
		PANDA_700MHZ,
		PANDA_921MHZ,
		PANDA_1200MHZ,
		PANDA_1500MHZ
	};


	clock_t getCoreClock();
	clock_t getPeripheralClock();
	void setCoreClk(PANDA_CLKS clk);


	static OSC_OMAP4460ClockManagement pInstance;
};



#endif /* ARCH_ARMV7_OMAP4460_DRIVER_OSCOMAP4460CLOCKMANAGEMENT_H_ */
