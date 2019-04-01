/*
 * LocalAPIC.cc
 *
 *  Created on: 11.06.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/LocalAPIC.h>

// Include memory mapped operations helper
#include <common/memreg.h>

// Include MMU to set MMIO to not cacheable
#include <driver/X86MMU.h>

#include <common/msr.h>

#include <common/debug.h>

#include <interrupt_table.h>

// FIXME move to legacypit class
#include <common/IOPort.h>

#define TAG "LocalAPIC"

LocalAPIC LocalAPIC::mInstance;


uint8_t getLocalAPICID() {
	//DEBUG_STREAM(TAG,"LocalAPICID: " << hex << (uint16_t)LocalAPIC::mInstance.getAPICID());
	return LocalAPIC::mInstance.getAPICID();
}

LocalAPIC::LocalAPIC() : mBaseAddress(0), mTimeBase(0) {
	// Determine APIC base address
	uint64_t baseReg = readMSR(cMSR_APIC_BASE_ADDRESS_REGISTER);
	APIC_BASE_REGISTER_t *regStruct = (APIC_BASE_REGISTER_t*)&baseReg;

	mBaseAddress = regStruct->aba << 12;
}


void LocalAPIC::printDebug() {
	// Determine APIC base address
#ifdef CONFIG_DEBUG
	uint64_t baseReg = readMSR(cMSR_APIC_BASE_ADDRESS_REGISTER);
	APIC_BASE_REGISTER_t *regStruct = (APIC_BASE_REGISTER_t*)&baseReg;
	DEBUG_STREAM(TAG, "Base Address Register = BSP: " << (uint16_t)regStruct->bsc << " AE: " << (uint16_t)regStruct->ae);

	DEBUG_STREAM(TAG, "APIC base reg: " << hex << baseReg);
	DEBUG_STREAM(TAG, "APIC base address: " << hex << mBaseAddress << " physical: " << X86MMU::mInstance.virtualToPhysical(mBaseAddress));
	DEBUG_STREAM(TAG, "APIC id: " << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_ID)));


	uint32_t versionRegVal = readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_VERSION));
	DEBUG_STREAM(TAG, "APIC Version raw: " << hex << versionRegVal);
	APIC_VERSION_REGISTER_t *versionReg = (APIC_VERSION_REGISTER_t*)&versionRegVal;
	DEBUG_STREAM(TAG, "APIC version: " << dec << versionReg->version << ", max LVT: " << versionReg->mle << ", ext. APIC: " << (uint16_t)versionReg->eas);
#endif
}

void LocalAPIC::printRegisters() {
	DEBUG_STREAM(TAG,"APICID: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_ID)));
	DEBUG_STREAM(TAG,"APIC VERSION: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_VERSION)));
	DEBUG_STREAM(TAG,"APIC TPR: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TASK_PRIORITY)));
	DEBUG_STREAM(TAG,"APIC ARB: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_ARBITRATION_PRIORITY)));
	DEBUG_STREAM(TAG,"APIC SPIV: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_SPURIOUS_INTERRUPT_VECTOR)));
	DEBUG_STREAM(TAG,"APIC ESR: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_ERROR_STATUS)));
	DEBUG_STREAM(TAG,"APIC LDR: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_LOCAL_DESTINATION)));
	DEBUG_STREAM(TAG,"APIC PPR: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_PROCESS_PRIORITY)));
	DEBUG_STREAM(TAG,"APIC ICR_low: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_INTERRUPT_COMMAND_LOW)));
	DEBUG_STREAM(TAG,"APIC ICR_high: " << hex << readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_INTERRUPT_COMMAND_HIGH)));
}

void LocalAPIC::sendEOI() {
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_END_OF_INTERRUPT), 0x0);
}

void LocalAPIC::setTimebase(uint64_t timebase) {
	mTimeBase = timebase;
}

void LocalAPIC::windupPeriodic(uint64_t interval) {
	if (mTimeBase == 0) {
		DEBUG_STREAM(TAG,"ERROR, no timebase set. Aborting timer windup :'(");
	}
	// Calculate the number of ticks for the given interval (given in us but our timebase is in ps so multiply by 1000000)
	uint32_t timerVal = (interval * 1000000) / mTimeBase;

	DEBUG_STREAM(TAG,"Windup periodic timer with counter value: " << hex << timerVal);

	// Set timer divider
	setTimerDivider(DIVIDE_BY_1);

	// Program the LVT
	APIC_TIMER_LVT_REGISTER_t lvtReg;

	// Read old value
	*((uint32_t*)&lvtReg) = readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_LOCAL_VECTOR));

	lvtReg.vec = HW_INT_LOCAL_APIC_TIMER;
	lvtReg.tmm = TIMER_MODE_PERIODIC;
	lvtReg.m = 0; // Unmasked
	lvtReg.reserved_3 = 0;
	// Activate the timer by setting the entry
	DEBUG_STREAM(TAG,"Writing LVT: " << hex << *((uint32_t*)&lvtReg));
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_LOCAL_VECTOR), *((uint32_t*)&lvtReg));


	// Set the initial counter value
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_INITIAL_COUNT), timerVal);
}

void LocalAPIC::setTaskPriority(uint8_t taskPriority, uint8_t taskPrioritySubClass) {
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TASK_PRIORITY), ((taskPriority << 4)| taskPrioritySubClass));
}

void LocalAPIC::setDestinationMode(LocalAPIC::DESTINATION_MODE_t mode) {
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_DESTINATION_FORMAT), (mode << 28));
}

void LocalAPIC::initialize() {


	// Set memory mapped range to not-cacheable as per definition (AMD64 Vol.2. p. 534)
	X86MMU::mInstance.setRangeCacheable(mBaseAddress, mBaseAddress + 0x200000, false);

//	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_LOCAL_DESTINATION), 1<<24);

	// Set flat destination mode
	setDestinationMode(LocalAPIC::DESTINATION_MODE_FLAT);

	// Enable APIC
	APIC_SPURIOUS_VECTOR_REGISTER_t spuriousReg;
	spuriousReg.ase = 1;
	spuriousReg.vector = 0xFF;
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_SPURIOUS_INTERRUPT_VECTOR), *(uint32_t*)&spuriousReg);

	sendEOI();

	// Clear error register
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_ERROR_STATUS), 0);


	// Accept all interrupts for now
	setTaskPriority(0,0);

	// If processor is BSP calibrate the local APIC
	if (getCPUID() == 0) {
		calibrate();
	}
}

void LocalAPIC::sendIPI(uint8_t destination, APIC_ICR_REGISTER_LOW_t icrReg) {
	// Read upper part of ICR
	uint32_t icrHighReg = readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_INTERRUPT_COMMAND_HIGH));

	APIC_ICR_REGISTER_HIGH_t *highReg = (APIC_ICR_REGISTER_HIGH_t*)&icrHighReg;
	highReg->destination = destination;

	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_INTERRUPT_COMMAND_HIGH), icrHighReg);

	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_INTERRUPT_COMMAND_LOW), *(uint32_t*)&icrReg);
}

uint8_t LocalAPIC::getIPIDeliveryStatus() {
	uint32_t icrLowReg = readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_INTERRUPT_COMMAND_LOW));
//	DEBUG_STREAM(TAG,"icrLowReg: " << hex << icrLowReg);
	return ((APIC_ICR_REGISTER_LOW_t*)&icrLowReg)->deliveryStatus;
}

void LocalAPIC::setTimerDivider(TIMER_DIVIDER_VALUE_t dividerVal) {
	// Map divider value to register definition (AMD64 Vol.2 p 557 (24593—Rev. 3.29—December 2017))
	uint32_t regVal = ((dividerVal & 0x4) << 1) | (dividerVal & 0x3);
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_DIVIDER_CONFIGURATION), regVal);
	DEBUG_STREAM(TAG,"Wrote timer divider register to: " << hex << regVal);
}

void LocalAPIC::setTimerInterrupt(bool enable) {
	// Program the LVT
	APIC_TIMER_LVT_REGISTER_t lvtReg;

	// Read old value
	*((uint32_t*)&lvtReg) = readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_LOCAL_VECTOR));

	lvtReg.m = enable;
	// Activate the timer by setting the entry
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_LOCAL_VECTOR), *((uint32_t*)&lvtReg));
}

void LocalAPIC::calibrate() {
	// Set timer divider
	setTimerDivider(DIVIDE_BY_1);

	IOPort ctrl(0x43);
	IOPort data(0x42);
	IOPort help(0x61);

	// Set timer without interrupts.
	// Set the initial counter value
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_INITIAL_COUNT), 0xFFFFFFFF);

	// Program the LVT
	APIC_TIMER_LVT_REGISTER_t lvtReg;

	// Read old value
	*((uint32_t*)&lvtReg) = readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_LOCAL_VECTOR));

	lvtReg.vec = HW_INT_LOCAL_APIC_TIMER;
	lvtReg.tmm = TIMER_MODE_ONESHOT;
	lvtReg.m = 1; // Unmasked
	lvtReg.reserved_3 = 0;
	// Activate the timer by setting the entry
	writeRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_LOCAL_VECTOR), *((uint32_t*)&lvtReg));

	help.writeByte(0x01);
	ctrl.writeByte(0xB0);
	data.writeByte(0xFF);
	data.writeByte(0xFF);

	// Read current timer count.
	uint32_t ticks = readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_CURRENT_COUNT));

	// Wait for PIT to finish.
	while(!(help.readByte() & 0x20));

	ticks = ticks - readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_TIMER_CURRENT_COUNT));
	//uint32_t used_ticks = Math::div64(((unsigned long long) ticks) * 1000 * 1000, 838 * 65535);
	DEBUG_STREAM(TAG,"LAPIC ticks in " << dec << (65535*838)<<"ns : " << ticks);


	uint64_t tempVal = ((uint64_t)838 * (uint64_t)65535 * (uint64_t)1000)/(uint64_t)(ticks);
	DEBUG_STREAM(TAG,"ps per tick: " << dec << tempVal);


#ifdef CONFIG_DEBUG
	uint64_t frequency = (1*1000000000000)/tempVal;
	DEBUG_STREAM(TAG,"Timer frequency in kHz: " << dec << frequency / 1000);
#endif
	mTimeBase = tempVal;

	DEBUG_STREAM(TAG,"LocalAPIC timer calibrated to timebase: " << dec << mTimeBase);
}
