/*
 * OSCOMAP4460ClockManagement.cc
 *
 *  Created on: 26.04.2016
 *      Author: hendrik
 */

#include <arch/armv7/omap4460/driver/OSCOMAP4460ClockManagement.h>

/* Include memory register helper */
#include <common/memreg.h>

/* Include bit manipulation */
#include <common/bitmanipulation.h>

#define TAG "CLKMGR"

OSC_OMAP4460ClockManagement OSC_OMAP4460ClockManagement::pInstance;

clock_t OSC_OMAP4460ClockManagement::getCoreClock() {
	unsigned int clkselDpllMpu = READ_REGISTER(CM_CLKSEL_DPLL_MPU);
//	DEBUG_STREAM(TAG,"CLKSEL_DPLL_MPU" << hex << clkselDpllMpu);

	unsigned int n = clkselDpllMpu & (0 | (0x3F << 0));
	unsigned int m = (clkselDpllMpu & (0 | (0x7FF << 8))) >> 8;
	DEBUG_STREAM(TAG,"n: " << dec << n << " m: " << dec << m);
	unsigned long long sysClk = (OMAP4_BASE_CORE);
	sysClk = sysClk * (unsigned long) (2 * m);
	sysClk /= (n + 1);
	if (!testbit(clkselDpllMpu, 22)) {
		sysClk /= 2;
	}

//	DEBUG_STREAM(TAG,"Sysclk before m2 divider: " << dec << sysClk);

// Get secondary divider (M2)
	unsigned int cmDivM2 = READ_REGISTER(CM_DIV_M2_DPLL_MPU);
	cmDivM2 = cmDivM2 & (0 | 0x1F);
	DEBUG_STREAM(TAG,"M2 divider: " << cmDivM2);
	if (cmDivM2 != 0) {
		sysClk /= cmDivM2;
	}
//	DEBUG_STREAM(TAG,"Sysclk after m2 divider: " << dec << sysClk);

	return (clock_t) (sysClk);
}

clock_t OSC_OMAP4460ClockManagement::getPeripheralClock() {
	return getCoreClock() / 2;

}

void OSC_OMAP4460ClockManagement::setCoreClk(PANDA_CLKS clk) {
	unsigned long int mpuClkCtrl;
	unsigned int dpllModeReg = READ_REGISTER(CM_CLKMODE_DPLL_MPU);

	// Set autoidle register
	unsigned int cmAutoidleReg = READ_REGISTER(CM_AUTOIDLE_DPLL_MPU);
	cmAutoidleReg &= ~(0x7 << 0);
	cmAutoidleReg |= 0x0;
	WRITE_REGISTER(CM_AUTOIDLE_DPLL_MPU, cmAutoidleReg);

	dpllModeReg = READ_REGISTER(CM_CLKMODE_DPLL_MPU);
	dpllModeReg &= ~(0x7 << 0);
	dpllModeReg &= ~(0x1 << 10);
	dpllModeReg |= 0x4;
	DEBUG_STREAM(TAG,"Bypassing DPLL");
	WRITE_REGISTER(CM_CLKMODE_DPLL_MPU, dpllModeReg);

	unsigned int dpllStatusReg = READ_REGISTER(CM_IDLEST_DPLL_MPU);

	unsigned int n = 0;
	unsigned int m = 0;
	unsigned int m2 = 0;
	switch (clk) {
	case PANDA_350MHZ:
		m = 73;
		n = 3;
		m2 = 2;
		// SET ABE EMIF freqs
		mpuClkCtrl = READ_REGISTER(CM_MPU_MPU_CLKCTRL);
		mpuClkCtrl = setbit(mpuClkCtrl, 25, false);
		mpuClkCtrl = setbit(mpuClkCtrl, 24, false);
		WRITE_REGISTER(CM_MPU_MPU_CLKCTRL, mpuClkCtrl);
		break;
	case PANDA_700MHZ:
		m = 73;
		n = 3;
		m2 = 1;
		// SET ABE EMIF freqs
		mpuClkCtrl = READ_REGISTER(CM_MPU_MPU_CLKCTRL);
		mpuClkCtrl = setbit(mpuClkCtrl, 25, false);
		mpuClkCtrl = setbit(mpuClkCtrl, 24, false);
		WRITE_REGISTER(CM_MPU_MPU_CLKCTRL, mpuClkCtrl);
		break;
	case PANDA_921MHZ:
		m = 24;
		n = 0;
		m2 = 1;
		// SET ABE EMIF freqs
		mpuClkCtrl = READ_REGISTER(CM_MPU_MPU_CLKCTRL);
		mpuClkCtrl = setbit(mpuClkCtrl, 25, true);
		mpuClkCtrl = setbit(mpuClkCtrl, 24, true);
		WRITE_REGISTER(CM_MPU_MPU_CLKCTRL, mpuClkCtrl);
		break;
	case PANDA_1200MHZ:
		m = 125;
		n = 7;
		m2 = 1;
		// SET ABE EMIF freqs
		mpuClkCtrl = READ_REGISTER(CM_MPU_MPU_CLKCTRL);
		mpuClkCtrl = setbit(mpuClkCtrl, 25, false);
		mpuClkCtrl = setbit(mpuClkCtrl, 24, false);
		WRITE_REGISTER(CM_MPU_MPU_CLKCTRL, mpuClkCtrl);
		// Special handling for M3 clks (see omap ref. p. 421)
		break;
	case PANDA_1500MHZ:
		m = 39;
		n = 1;
		m2 = 1;
		// SET ABE EMIF freqs
		mpuClkCtrl = READ_REGISTER(CM_MPU_MPU_CLKCTRL);
		mpuClkCtrl = setbit(mpuClkCtrl, 25, true);
		mpuClkCtrl = setbit(mpuClkCtrl, 24, true);
		WRITE_REGISTER(CM_MPU_MPU_CLKCTRL, mpuClkCtrl);
		break;
	default:
		break;
	}

	// Set new clk values
	unsigned int nMask = 0 | (n << 0);
	unsigned int mMask = 0 | (m << 8);

	// clear n and m values
	unsigned int clkSelDpllMpu = READ_REGISTER(CM_CLKSEL_DPLL_MPU);
	clkSelDpllMpu = clkSelDpllMpu & ~(0x3F << 0);
	clkSelDpllMpu = clkSelDpllMpu & ~(0x7FF << 8);
	DEBUG_STREAM(TAG,"clksel: " << hex << clkSelDpllMpu);
	DEBUG_STREAM(TAG,"nMask: " << nMask << " mMask: " << mMask);
	clkSelDpllMpu |= nMask;
	clkSelDpllMpu |= mMask;

	unsigned int clkM2reg = READ_REGISTER(CM_DIV_M2_DPLL_MPU);
	clkM2reg = clkM2reg & ~(0x1F);
	clkM2reg |= m2;

	// Disable DCC
	clkSelDpllMpu = setbit(clkSelDpllMpu, 22, false);

	WRITE_REGISTER(CM_DIV_M2_DPLL_MPU, clkM2reg);
	WRITE_REGISTER(CM_CLKSEL_DPLL_MPU, clkSelDpllMpu);



	if (clk == PANDA_1200MHZ || clk == PANDA_1500MHZ) {
		clkSelDpllMpu = clkSelDpllMpu & ~(0xF << 24);
		// DCC Count max = 8 = ~ 2.5ṃicroseconds
		clkSelDpllMpu |= (0x8 << 24);
//		WRITE_REGISTER(CM_CLKSEL_DPLL_MPU, clkSelDpllMpu);
		// Enable duty cycle correction
		clkSelDpllMpu = setbit(clkSelDpllMpu, 22, true);

		WRITE_REGISTER(CM_CLKSEL_DPLL_MPU, clkSelDpllMpu);
		clkSelDpllMpu = READ_REGISTER(CM_CLKSEL_DPLL_MPU);
		DEBUG_STREAM(TAG,"clkSelDpllMpu: " << hex << clkSelDpllMpu);
	}

	dpllModeReg = READ_REGISTER(CM_CLKMODE_DPLL_MPU);
	dpllModeReg &= ~(0x7 << 0);
	dpllModeReg |= 0x7;
	// Set dpll to lockmode
	DEBUG_STREAM(TAG,"Locking DPLL again");
	WRITE_REGISTER(CM_CLKMODE_DPLL_MPU, dpllModeReg);

	DEBUG_STREAM(TAG,"dpll status: " << hex << dpllStatusReg);
	while (!testbit(dpllStatusReg, 0)) {
		dpllStatusReg = READ_REGISTER(CM_IDLEST_DPLL_MPU);
		DEBUG_STREAM(TAG,"Waiting for dpll lock");
		DEBUG_STREAM(TAG,"dpll status: " << hex << dpllStatusReg);
	}

	DEBUG_STREAM(TAG,"dpll status: " << hex << dpllStatusReg);

}

