/*
 * IMX6ClockManagement.cc
 *
 *  Created on: 27.10.2016
 *      Author: hendrik
 */

#include "IMX6ClockManagement.h"

#include <common/debug.h>

#include <common/memreg.h>

#define TAG "IMX6Clocks"

IMX6ClockManagement IMX6ClockManagement::mInstance;

/**
 * Allowed frequency/voltage values for the i.MX6 SoC. This table is derived from the
 * i.MX6 of the Linux kernel 4.8.8 (arch/arm/boot/dts/imx6q.dtsi).
 * It is important to always set both the frequency and the voltage.
 */
IMX6ClockManagement::IMX6_FREQUENCY_VOLTAGE IMX6ClockManagement::mFrequencies[] = {
		{396000000,  975000},
		{792000000,  1175000},
		{852000000,  1250000},
		{996000000,  1250000},
		{1200000000, 1275000},
};

IMX6ClockManagement::IMX6ClockManagement() {
}

/**
 * Returns the main arm frequency in Hz.
 */
clock_t IMX6ClockManagement::getCoreClock() {
	return getARMPLLFrequency();
}

/**
 * Peripheral clock is half of the core frequency.
 */
clock_t IMX6ClockManagement::getPeripheralClock() {
	return getCoreClock()/2;
}

/**
 * Dumps the content of the registers of the power management unit.
 */
void IMX6ClockManagement::dumpPMURegisters() {
	DEBUG_STREAM(TAG,"1P1: " << hex << READ_REGISTER(PMU_REG_1P1));
	DEBUG_STREAM(TAG,"3P0: " << hex << READ_REGISTER(PMU_REG_3P0));
	DEBUG_STREAM(TAG,"2P5: " << hex << READ_REGISTER(PMU_REG_2P5));

	DEBUG_STREAM(TAG,"CORE: " << hex << READ_REGISTER(PMU_REG_CORE));
	DEBUG_STREAM(TAG,"MISC0: " << hex << READ_REGISTER(PMU_REG_MISC0));

	DEBUG_STREAM(TAG,"MISC1: " << hex << READ_REGISTER(PMU_REG_MISC1));
	DEBUG_STREAM(TAG,"MISC1_SET: " << hex << READ_REGISTER(PMU_REG_MISC1_SET));
	DEBUG_STREAM(TAG,"MISC1_CLR: " << hex << READ_REGISTER(PMU_REG_MISC1_CLR));
	DEBUG_STREAM(TAG,"MISC1_TOG: " << hex << READ_REGISTER(PMU_REG_MISC1_TOG));

	DEBUG_STREAM(TAG,"MISC2: " << hex << READ_REGISTER(PMU_REG_MISC2));
	DEBUG_STREAM(TAG,"MISC2_SET: " << hex << READ_REGISTER(PMU_REG_MISC2_SET));
	DEBUG_STREAM(TAG,"MISC2_CLR: " << hex << READ_REGISTER(PMU_REG_MISC2_CLR));
	DEBUG_STREAM(TAG,"MISC2_TOG: " << hex << READ_REGISTER(PMU_REG_MISC2_TOG));
}

/**
 * Dumps the content of the registers of the power management unit.
 */
void IMX6ClockManagement::dumpCCMRegisters() {
	DEBUG_STREAM(TAG,"PLL_ARM: " << hex << READ_REGISTER(CCM_ANALOG_PLL_ARM));
	DEBUG_STREAM(TAG,"PLL_USB1: " << hex << READ_REGISTER(CCM_ANALOG_PLL_USB1));
	DEBUG_STREAM(TAG,"PLL_USB2: " << hex << READ_REGISTER(CCM_ANALOG_PLL_USB2));
	DEBUG_STREAM(TAG,"PLL_SYS: " << hex << READ_REGISTER(CCM_ANALOG_PLL_SYS));

	DEBUG_STREAM(TAG,"CCSOR: " << hex << READ_REGISTER(CCM_CCOSR));
	DEBUG_STREAM(TAG,"CCGR1: " << hex << READ_REGISTER(CCM_CCGR1));
}

/**
 * Calculates the clock of the ARM cores. This is done by reading out the PLL divider
 * (and assuming it is not bypassed). If it is bypassed the clock source is defined by
 * the bypass source bit field. Otherwise the core frequency is Fout = Fref * (divider/2).
 * The reference frequency is defined in the header and should be 24MHz.
 */
clock_t IMX6ClockManagement::getARMPLLFrequency() {
	/* Read the PLL control register for the ARM core PLL. */
	dword_t armPLL = READ_REGISTER(CCM_ANALOG_PLL_ARM);

	/* Mask out the divider bits 0-6 */
	dword_t div_sel = armPLL & (0x7F);

	/* Return the calculated frequency (s. above) */
	return IMX6_REFERENCE_CLOCK * (div_sel/2);
}

/**
 * This method enables the bypass function of the ARM core PLL. This allows to reconfigure
 * the clock of the PLL without cutting of the main core clock. The bypass clock is defined by
 * the corresponding CCM register (CCM_ANALOG_PLL_ARM) i.MX6 TRM p. 900.
 */
void IMX6ClockManagement::bypassARMClkToReferenceFrequency(bool bypass) {
	dword_t armPLL = READ_REGISTER(CCM_ANALOG_PLL_ARM);

	if ( bypass ) {
		armPLL |= (1 << 16);
	} else {
		armPLL &= ~(1 << 16);
	}
	WRITE_REGISTER(CCM_ANALOG_PLL_ARM, armPLL);
}

void IMX6ClockManagement::setARMPLLClock(clock_t clk) {
	/* Calculate the new divider value */
	uint8_t divider = 2*clk/IMX6_REFERENCE_CLOCK;
#ifdef CONFIG_IMX6_CLOCK_DEBUG
	DEBUG_STREAM(TAG,"New calulated divider value: " << dec << (uint32_t)divider);
#endif

	dword_t armPLL = READ_REGISTER(CCM_ANALOG_PLL_ARM);

	/* Remove old divider value */
	armPLL &= ~(0x7F);

	/* Set new value */
	armPLL |= divider;

#ifdef CONFIG_IMX6_CLOCK_DEBUG
	DEBUG_STREAM(TAG,"Writing ARM PLL register to: " << hex << armPLL);
#endif

	/* Set the PLL to the new clock divider */
	WRITE_REGISTER(CCM_ANALOG_PLL_ARM, armPLL);

	/* Wait for the PLL to relock to its new frequency */
	while ( !(READ_REGISTER(CCM_ANALOG_PLL_ARM) & (1 << 31)) ) {}
#ifdef CONFIG_IMX6_CLOCK_DEBUG
	DEBUG_STREAM(TAG, "ARM PLL locked again");
	DEBUG_STREAM(TAG, "New ARM core frequency: " << dec << (getARMPLLFrequency() /(1000000)) << "MHz");
#endif
}

void IMX6ClockManagement::setCoreVoltage(uint32_t voltage) {
	/* Get the current value from the PMU */
	dword_t pmuCore = READ_REGISTER(PMU_REG_CORE);

	/* Calculate the new voltage value (i.MX6 TRM p. 4467) */
	uint32_t targetVoltage = (voltage - IMX6_ARM_CORE_BASE_VOLTAGE) / 25000;
#ifdef CONFIG_IMX6_CLOCK_DEBUG
	DEBUG_STREAM(TAG,"New voltage target value: " << bin << targetVoltage);
#endif

	/* Set the new target value */
	pmuCore |= targetVoltage;

	/* Write it to the PMU register */
	WRITE_REGISTER(PMU_REG_CORE, pmuCore);
}

void IMX6ClockManagement::setCoreFrequencyVoltage(IMX6_FREQUENCY_VOLTAGE value) {
#ifdef CONFIG_IMX6_CLOCK_DEBUG
	DEBUG_STREAM(TAG, "Setting core clock to: " << dec << value.frequency/1000000 << "MHz with a voltage of: " << value.voltage/1000 << "mV" );
#endif

	/* Start by bypassing the PLL to the reference clock (otherwise the CPU would halt) */
	bypassARMClkToReferenceFrequency(true);

	/* First ramp up the voltage, to compensate for the higher frequency */
	setCoreVoltage(value.voltage);

	/* Set the new frequency and wait for the PLL to lock */
	setARMPLLClock(value.frequency);

	/* Disable PLL bypass again, as the ARM PLL should be locked again */
	bypassARMClkToReferenceFrequency(false);
}

/**
 * Enables the SPI peripheral clock of the controller. Documented in i.MX6 TRM: p. 799/887
 * @param spiNum Number of the SPI controller 1-5
 * @param enable If enable or disable (gated)
 */
void IMX6ClockManagement::enableSPIClock(uint8_t spiNum, bool enable) {
	uint8_t enableMask = 0x3;
	uint8_t shiftNum = (spiNum - 1) * 2;

	/* Read old register content */
	dword_t regTmp = READ_REGISTER(CCM_CCGR1);
	if (enable) {
		regTmp |= (enableMask << shiftNum);
	} else {
		regTmp &= ~(enableMask << shiftNum);
	}

	/* Write the changed clock-gating register */
	WRITE_REGISTER(CCM_CCGR1, regTmp);
}
