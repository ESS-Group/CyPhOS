/*
 * X86System.cc
 *
 *  Created on: 29.05.2018
 *      Author: hendrik
 */

#include <common/types.h>

#include <driver/SMPManager.h>
#include <driver/LocalAPIC.h>

#include <common/IOPort.h>

extern "C" {
	void restart_system() {
		// FIXME
		for(int i = 0; i < 10; i++) {
		IOPort kbd(0x64);
		kbd.writeByte(0xFE);
		}

	}


	cpu_t _getCPUID() {
		// FIXME return actual cpuID
		return SMPManager::sInstance.translateAPICIDToCPUID(LocalAPIC::mInstance.getAPICID());
	}
}
