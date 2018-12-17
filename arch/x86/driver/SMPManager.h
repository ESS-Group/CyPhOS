/*
 * SMPManager.h
 *
 *  Created on: 20.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_SMPMANAGER_H_
#define ARCH_X86_DRIVER_SMPMANAGER_H_

#include <common/types.h>

class SMPManager {
public:
	struct CPU_ENTRY_t {
		uint8_t apicID;
		uint8_t processorID;
		bool enabled;
		bool bsp;
		bool ignore;
	} __attribute__((packed));

	SMPManager();

	static SMPManager sInstance;

	/**
	 * Called from the ACPI parser when a new APIC entry is found in the MADT table
	 * @param apicID ID to add to the manager. Will be started up later
	 */
	void addAPICID(uint8_t apicID, uint8_t processorID, bool ignore);

	void bootCPU(uint8_t apicID);

	void startRegisteredAPs();

	void initializeSMP();

	void relocateStartupCode();

	void confirmStartup(uint8_t apicID);

	inline uint8_t translateAPICIDToCPUID(uint8_t apicID) {
		return mCPUMapping[apicID];
	}
private:
	/**
	 * Array of all possible APIC ids in the running system. Limited by NR_CPUS
	 */
	CPU_ENTRY_t mAPICIDs[NR_CPUS];

	uint8_t mCPUMapping[256];

	/**
	 * Counts the number of discovered CPUs
	 */
	uint8_t mRegisteredCPUs;

	volatile uint32_t mHoldingPen;
};

#endif /* ARCH_X86_DRIVER_SMPMANAGER_H_ */
