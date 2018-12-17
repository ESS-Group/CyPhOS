/*
 * LegacyPIT.h
 *
 *  Created on: 22.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_LEGACYPIT_H_
#define ARCH_X86_DRIVER_LEGACYPIT_H_

#include <common/types.h>
#include <common/IOPort.h>

class LegacyPIT {

private:
	/**
	 * Frequency in Hz
	 */
	static constexpr uint64_t cPITFrequency = 1193182;
	static constexpr uint8_t cChannel0PortAddress = 0x40;
	static constexpr uint8_t cChannel1PortAddress = 0x41;
	static constexpr uint8_t cChannel2PortAddress = 0x42;
	static constexpr uint8_t cModeCommandRegisterPortAddress = 0x43;

	IOPort mPortChannel0;
	IOPort mPortChannel1;
	IOPort mPortChannel2;
	IOPort mPortModeCommand;

public:
	void prepareBlock(uint64_t us);
	void block();

};

#endif /* ARCH_X86_DRIVER_LEGACYPIT_H_ */
