/*
 * IOAPIC.cc
 *
 *  Created on: 14.06.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/IOAPIC.h>

#include <common/debug.h>
#include <common/memreg.h>

#include <driver/X86MMU.h>

#define TAG "IOAPIC"

IOAPIC IOAPIC::mInstances[IOAPIC::cMAX_INSTANCES];
uint32_t IOAPIC::sRegisteredIOAPICs;

IOAPIC::IOAPIC() : mID(0), mBaseAddress(0), mGlobalInterruptBase(0x0) {

}

void IOAPIC::configureNewInstance(uintptr_t baseAddress, uint8_t ioapicID, uint32_t globalInterruptBase) {
	// Set properties for the discovered IOAPIC
	mInstances[sRegisteredIOAPICs].mBaseAddress = baseAddress;
	mInstances[sRegisteredIOAPICs].mID = ioapicID;
	mInstances[sRegisteredIOAPICs].mGlobalInterruptBase = globalInterruptBase;

	// To be sure set the MMIO page of the IOAPIC to not-cacheable.
	X86MMU::mInstance.setRangeCacheable(baseAddress, 0x200000, false);

	// Debug output
#if defined(CONFIG_X86_DEBUG_IOAPIC) & defined(CONFIG_DEBUG)
	IOAPIC_ID_REGISTER_t idReg = mInstances[sRegisteredIOAPICs].getIDRegister();
	IOAPIC_VERSION_REGISTER_t versionReg = mInstances[sRegisteredIOAPICs].getVersionRegister();
	DEBUG_STREAM(TAG, "ID: " << dec << idReg.ioapicID << ", version: " << versionReg.version << ". max. red. entries: " << versionReg.maximumRedirectionEntries);
#endif

	// Call the initialize method for the discovered instance to ensure proper working condition
	mInstances[sRegisteredIOAPICs].initialize();

	// Increment global static counter
	sRegisteredIOAPICs++;
}

uint32_t IOAPIC::readIOAPICRegister(uint16_t offset) {
	// Select register with selection MMIO register
	writeRegister_32((uint32_t*)(mBaseAddress + cREG_SEL), (uint32_t)offset);

	// Read register window (MMIO)
	return readRegister_32((uint32_t*)(mBaseAddress + cREG_WIN));
}

void IOAPIC::writeIOAPICRegister(uint16_t offset, uint32_t value) {
	//DEBUG_STREAM(TAG,"Writing: " << hex << value << " to offset: " << hex << offset);
	// Select register with selection MMIO register
	writeRegister_32((uint32_t*)(mBaseAddress + cREG_SEL), (uint32_t)offset);

	writeRegister_32((uint32_t*)(mBaseAddress + cREG_WIN), value);
}

IOAPIC::IOAPIC_ID_REGISTER_t IOAPIC::getIDRegister() {
	IOAPIC_ID_REGISTER_t retVal;
	*((volatile uint32_t*)&retVal) = readIOAPICRegister(cREG_ID);
	return retVal;
}

IOAPIC::IOAPIC_VERSION_REGISTER_t IOAPIC::getVersionRegister() {
	IOAPIC_VERSION_REGISTER_t retVal;
	*((volatile uint32_t*)&retVal) = readIOAPICRegister(cREG_VERSION);
	return retVal;
}

IOAPIC::IOAPIC_ARBITRATION_REGISTER_t IOAPIC::getArbitrationRegister() {
	IOAPIC_ARBITRATION_REGISTER_t retVal;
	*((volatile uint32_t*)&retVal) = readIOAPICRegister(cREG_ARB);
	return retVal;
}

uint8_t IOAPIC::getMaximumRedirectionEntryCount() {
	return getVersionRegister().maximumRedirectionEntries;
}

IOAPIC::IOAPIC_ARBITRATION_ENTRY_t IOAPIC::getRedirectionEntry(uint8_t index) {
	IOAPIC_ARBITRATION_ENTRY_t retVal;

	// Read both 32 bit values that spawn over two internal registers
	*(((volatile uint32_t*)&retVal)) = readIOAPICRegister(cREG_REDIRECTION_TABLE_START + (index * 2));
	*(((volatile uint32_t*)&retVal) + 1) = readIOAPICRegister(cREG_REDIRECTION_TABLE_START + (index * 2 + 1));

	return retVal;
}

void IOAPIC::setRedirectionEntry(uint8_t index,	IOAPIC::IOAPIC_ARBITRATION_ENTRY_t &entry) {
	// Write both 32 bit values that spawn over two internal registers
	writeIOAPICRegister(cREG_REDIRECTION_TABLE_START + (index * 2), *((uint32_t*)&entry));
	writeIOAPICRegister(cREG_REDIRECTION_TABLE_START + (index * 2 + 1), *(((uint32_t*)&entry) + 1));
}

void IOAPIC::initialize() {

	setID(0);

	IOAPIC_ARBITRATION_ENTRY_t entry;
	// Reset all redirection entries
	uint8_t maxEntries = getMaximumRedirectionEntryCount();
	for (uint8_t i = 0; i < maxEntries; i++) {
		entry = getRedirectionEntry(i);
		entry.interruptVector = 0x20 + i;
		entry.deliveryMode = DELIVERY_MODE_FIXED;
		entry.destinationMode = DESTINATION_PHYSICAL;
		entry.inputPinPolarity = POLARITY_HIGH_ACTIVE;
		entry.triggerMode = TRIGGER_EDGE_SENSITIVE;
		entry.interruptMask = MASKED;
//		entry.destination.logicalMode.logicalDestination = 0x0;
		entry.destination.physicalMode.destinationAPICID = 0x0;
		entry.destination.physicalMode.reserved_2 = 0x0;
		setRedirectionEntry(i, entry);
	}
}

void IOAPIC::setInterruptMask(uint8_t slot, IOAPIC::INTERRUPT_MASK_t mask) {
	IOAPIC_ARBITRATION_ENTRY_t entry = getRedirectionEntry(slot);

	entry.interruptMask = mask;

	setRedirectionEntry(slot, entry);
}

void IOAPIC::setID(uint8_t ioapicID) {
	IOAPIC_ID_REGISTER_t reg = getIDRegister();

	reg.ioapicID = ioapicID;

	setIDRegister(reg);
}

void IOAPIC::setIDRegister(IOAPIC_ID_REGISTER_t regVal) {
	writeIOAPICRegister(cREG_ID, *(uint32_t*)&regVal);
}

void IOAPIC::printRegisters() {
	DEBUG_STREAM(TAG,"IOAPICID: " << hex << readIOAPICRegister(cREG_ID));
	DEBUG_STREAM(TAG,"IOAPICVERSION: " << hex << readIOAPICRegister(cREG_VERSION));
	DEBUG_STREAM(TAG,"IOAPICARB: " << hex << readIOAPICRegister(cREG_ARB));

#ifdef CONFIG_DEBUG
	uint8_t maxEntries = getMaximumRedirectionEntryCount();
	for (uint8_t i = 0; i < maxEntries; i++) {
		IOAPIC_ARBITRATION_ENTRY_t entry = getRedirectionEntry(i);
		DEBUG_STREAM(TAG,"Entry[" << dec << (uint16_t)i << "]: " << hex << *(uint64_t*)&entry);
	}
#endif
}
