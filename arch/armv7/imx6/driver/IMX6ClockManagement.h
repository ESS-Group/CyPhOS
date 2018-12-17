/*
 * IMX6ClockManagement.h
 *
 *  Created on: 27.10.2016
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_IMX6_DRIVER_IMX6CLOCKMANAGEMENT_H_
#define ARCH_ARMV7_IMX6_DRIVER_IMX6CLOCKMANAGEMENT_H_

#include "../../driver/ARMClockManagement.h"

#include "IMX6PMURegisters.h"
#include "IMX6CCMRegisters.h"

/* The manual defines a reference clock of 24MHz */
#define IMX6_REFERENCE_CLOCK 24000000

/* Base volume of the register that defines the core voltage. (i.MX6 TRM p. 4467) */
#define IMX6_ARM_CORE_BASE_VOLTAGE 700000


/**
 * Class to control the voltage and frequency levels of the i.MX6 SoC. For more details
 * look into the .cc file.
 */
class IMX6ClockManagement : public ARMClockManagement {
	private:
	struct IMX6_FREQUENCY_VOLTAGE {
		clock_t frequency;
		uint32_t voltage;
	};

	void bypassARMClkToReferenceFrequency(bool bypass);

	void setARMPLLClock(clock_t clk);
	void setCoreVoltage(uint32_t voltage);

public:
	IMX6ClockManagement();

	clock_t getCoreClock();
	clock_t getPeripheralClock();

	clock_t getARMPLLFrequency();

	void setCoreFrequencyVoltage(IMX6_FREQUENCY_VOLTAGE value);

	void dumpPMURegisters();
	void dumpCCMRegisters();

	void enableSPIClock(uint8_t spiNum, bool enable);

	static IMX6ClockManagement mInstance;
	static IMX6_FREQUENCY_VOLTAGE mFrequencies[];
};

#endif /* ARCH_ARMV7_IMX6_DRIVER_IMX6CLOCKMANAGEMENT_H_ */
