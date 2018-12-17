/*
 * SMPManager.cc
 *
 *  Created on: 20.06.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/SMPManager.h>

#include <common/debug.h>

#include <driver/LocalAPIC.h>

#define TAG "SMPManager"

// Static instance
SMPManager SMPManager::sInstance;

extern uint8_t init_longmode_ap;

extern uint8_t startup_ap_start;
extern uint8_t startup_ap_end;

static void printMemory(uintptr_t start, uint64_t length) {
	while (length != 0) {
		DEBUG_STREAM(TAG,"" << hex << start << ": " << *(uint32_t*)start << " " << (start + 4) << ": " << *(uint32_t*)(start + 4));
		if (length < 8) {
			length = 0;
		} else {
			length -= 8;
		}
		start += 8;
	}
}

SMPManager::SMPManager() :
		mRegisteredCPUs(0), mHoldingPen(0) {
	for (uint8_t i = 0; i < NR_CPUS; i++) {
		mAPICIDs[i].apicID = 0;
		mAPICIDs[i].enabled = false;
		mAPICIDs[i].bsp = false;
	}
}

void SMPManager::addAPICID(uint8_t apicID, uint8_t processorID, bool ignore) {
	if (mRegisteredCPUs == NR_CPUS) {
		DEBUG_STREAM(TAG,"Maximum CPU count reached, ignoring CPU");
		return;
	}

	DEBUG_STREAM(TAG,"Registering CPU " << dec << (uint16_t)mRegisteredCPUs << " with apicID: " << (uint16_t)apicID << ", processorID: " << (uint16_t)processorID);
	mAPICIDs[mRegisteredCPUs].apicID = apicID;
	mAPICIDs[mRegisteredCPUs].processorID = processorID;
	mAPICIDs[mRegisteredCPUs].ignore = ignore;

	// Create a linear apicID->CPU mapping
	mCPUMapping[apicID] = mRegisteredCPUs;

	// Increment available CPU counter
	mRegisteredCPUs++;
}

void SMPManager::bootCPU(uint8_t apicID) {

}

void SMPManager::startRegisteredAPs() {
	for (uint8_t cpu = 1; (cpu < mRegisteredCPUs) && (cpu < NR_CPUS); cpu++) {
		DEBUG_STREAM(TAG,"Halt CPU: " << dec << (uint16_t)cpu << " with ID: " << (uint16_t)mAPICIDs[cpu].apicID);
		// Prepare INIT IPI (p. 547 AMD64 Vol.2) to reset CPU to INIT state waiting for another SIPI
		LocalAPIC::APIC_ICR_REGISTER_LOW_t icrData;
		icrData.messageType = LocalAPIC::MESSAGE_TYPE_t::MESSAGE_TYPE_INIT;
		// Physical destination
		icrData.destinationMode = 0;
		// Set vector to 0 (p. 547 AMD 64 Vol.2)
		icrData.vector = 0x0;
		icrData.level = LocalAPIC::ICR_LEVEL_ASSERT;
		icrData.triggerMode = LocalAPIC::ICR_TRIGGER_LEVEL;
		// Send INIT IPI to target
		LocalAPIC::mInstance.sendIPI(mAPICIDs[cpu].apicID, icrData);
		BUSY_WAITING_SHORT;
		// Deassert IPI
		icrData.level = LocalAPIC::ICR_LEVEL_DEASSERT;
		LocalAPIC::mInstance.sendIPI(mAPICIDs[cpu].apicID, icrData);

		while (LocalAPIC::mInstance.getIPIDeliveryStatus()) {}
		//DEBUG_STREAM(TAG,"Delivered INIT IPI to CPU with APIC ID: " << (uint16_t)mAPICIDs[cpu].apicID);

		for (uint8_t startupCount = 0; startupCount < 2; startupCount++) {
			icrData.messageType = LocalAPIC::MESSAGE_TYPE_t::MESSAGE_TYPE_STARTUP;
			// Physical destination
			icrData.destinationMode = 0;

			icrData.vector = 0x40;
			icrData.level = 0x0;
			icrData.triggerMode = 0x0;
			DEBUG_STREAM(TAG,"Starting AP at: " << hex << (uint16_t)icrData.vector);
			// Send STARTUP IPI to target
			LocalAPIC::mInstance.sendIPI(mAPICIDs[cpu].apicID, icrData);
			BUSY_WAITING_SHORT;

			// Check delivery state
			if (!LocalAPIC::mInstance.getIPIDeliveryStatus()) {
				// IPI delivered successfully break loop
				break;
			}
		}
		while (mHoldingPen != cpu) {

		}
		DEBUG_STREAM(TAG,"Confirmed startup of CPU: " << dec << mHoldingPen);
	}


	for (uint8_t cpu = 1; cpu < mRegisteredCPUs; cpu++) {

	}
//	DEBUG_STREAM(TAG,"mHoldingPen: " << dec << mHoldingPen << " registered: " << (uint16_t)mRegisteredCPUs);
}

void SMPManager::initializeSMP() {
	relocateStartupCode();
	startRegisteredAPs();
}

void SMPManager::relocateStartupCode() {
	uintptr_t start = (uintptr_t) &startup_ap_start;
	uintptr_t end = (uintptr_t) &startup_ap_end;

	uint64_t length = (end - start);
	uintptr_t destination = 0x40000;
	DEBUG_STREAM(TAG,"Copy code from "<< hex << start << " - " << end);

	while (start < end) {
		*((uint64_t*) destination) = *((uint64_t*) start);
		start += 8;
		destination += 8;
	}

	printMemory(0x40000, length);
	printMemory((uintptr_t) &startup_ap_start, length);
}

void SMPManager::confirmStartup(uint8_t apicID) {
	mHoldingPen++;

	while ((mHoldingPen + 1) != mRegisteredCPUs && (mHoldingPen + 1) != NR_CPUS) {
	}
}
