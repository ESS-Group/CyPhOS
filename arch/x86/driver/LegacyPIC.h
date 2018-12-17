/*
 * LegacyPIC.h
 *
 *  Created on: 11.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_LEGACYPIC_H_
#define ARCH_X86_DRIVER_LEGACYPIC_H_

#include <common/types.h>
#include <common/IOPort.h>

class LegacyPIC {

private:
	/**
	 * IOPorts to access the PICs
	 */
	IOPort mMasterCommand;
	IOPort mMasterData;
	IOPort mSlaveCommand;
	IOPort mSlaveData;

	void remapToOffset(uint32_t offset);

	void maskIRQLine(uint8_t line, bool masked);
public:
	LegacyPIC();

	void disable();
	static LegacyPIC mInstance;
};

#endif /* ARCH_X86_DRIVER_LEGACYPIC_H_ */
