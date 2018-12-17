/*
 * SystemMode.cc
 *
 *  Created on: 24.08.2017
 *      Author: hendrik
 */

#include <core/SystemMode.h>

namespace Core {


void SystemMode::disableInterrupts() {
	cpu_t cpuID = getCPUID();
	if(mIRQCounter[cpuID] == 0 ) {
		disableInterruptBit();
	}
	mIRQCounter[cpuID]++;
}

void SystemMode::enableInterrupts() {
	cpu_t cpuID = getCPUID();
	mIRQCounter[cpuID]--;
	if(mIRQCounter[cpuID] == 0 ) {
		enableInterruptBit();
	}
}

} /* namespace Core */
