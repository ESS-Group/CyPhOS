/*
 * TLBInfo.cpp
 *
 *  Created on: 02.03.2017
 *      Author: christian
 */

#include "TLBInfo.h"
#include <common/OutputStream.h>

#define TAG "TLBInfo"

BEGIN_OSC_IMPLEMENTATION(TLBInfo,TLBInfo)

TLBInfo TLBInfo::mInstance;

SECTION_CRITICAL_DATA OSC* TLBInfo::trigger_deps[] = { nullptr };

SECTION_CRITICAL_DATA EventHandling::Trigger TLBInfo::trigger_show = { (OSC*) &TLBInfo::mInstance, (void (OSC::*)(dword_t))&TLBInfo::triggerFunc_start, (OSC**)&TLBInfo::trigger_deps, EventHandling::Trigger::cMIN_Priority + 800};

TLBInfo::TLBInfo() {
	// TODO Auto-generated constructor stub

}

void TLBInfo::triggerFunc_start(dword_t arg) {
	//Read the TLBTR Register
	dword_t regValue;
	asm ( "MRC p15, 0, %0, c0, c0, 3;" : "=&r"(regValue));
	DEBUG_STREAM(TAG, "TLB Information:");

	//Bit 16-23: # Instruction lockable entries (should be 0 for cortex-a9)
	uint32_t nILSize = (regValue & 0x00FF0000) >> 16;
	DEBUG_STREAM(TAG, "# Instruction lockable entries: " << dec << nILSize);
	//Bit 08-15: # Unified lockable entries (should be 4 for cortex-a9)
	uint32_t nDLSize = (regValue & 0x0000FF00) >> 8;
	DEBUG_STREAM(TAG, "# Data/Unified lockable entries: " << dec << nDLSize);
	//Bit 1-2: # TLB Entries
	uint32_t nEnt = (regValue & 0x000006) >> 1;
	uint32_t numberEntries[4] = { 64, 128, 256, 512 };
	DEBUG_STREAM(TAG, "# TLB Entries: " << dec << numberEntries[nEnt]);
	//Bit 0: If the cache is unified or not (should be 0 for cortex-a9)
	uint32_t nU = (regValue & 0x000001);
	DEBUG_STREAM(TAG, "Is the cache unified ? (0 for true): " << dec << nU);
}

END_OSC_IMPLEMENTATION

